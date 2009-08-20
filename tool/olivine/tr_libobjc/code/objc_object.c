
#define _POSIX_C_SOURCE 200112L
#define _XOPEN_SOURCE 500

#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include "objc_object.h"
#include "selector.h"
#include "object_tags.h"
#include "atomic.h"

//This is a Linuxism, but quite a nice one.  It allows a recursive mutex to
//be lazy-initialised.  On other platforms we have to do this the expensive
//way.
#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
#	define RECURSIVE_INIT(x) x = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
#elif defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER)
#	define RECURSIVE_INIT(x) x = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
#else
#	define RECURSIVE_INIT(x) pthread_mutex_init(&(x), &recursiveAttributes)
#endif
static pthread_mutexattr_t recursiveAttributes;

//Size of tags associated with an object
SEL forwardSelector = 0;
//Self for the entire program.  Can be modified to point to NSApp or similar.
id self = nil;

static inline int safestrcmp(const char * a, const char * b)
{
	if(a == b)
	{
		return 0;
	}
	if(a == 0 || b == 0)
	{
		return -1;
	}
	return strcmp(a, b);
}

//Tag offsets:
static int mutex_offset;
#define MUTEX(obj) TAG(obj, mutex_offset, pthread_mutex_t)
static int refcount_offset;
#define REFCOUNT(obj) TAG(obj, refcount_offset, uint32_t)
static int size_offset;
#define SIZE(obj) TAG(obj, size_offset, size_t)
//These two are extenally-visible
int slots_offset;
#define SLOTS(obj) TAG(obj, slots_offset, SparseArray*)
int lookup_offset;
#define LOOKUP(obj) TAG(obj, lookup_offset, lookup_function)

void objc_object_init(void)
{
	lookup_offset = OBJ_TAG(lookup_function);
	slots_offset = OBJ_TAG(SparseArray*);
	mutex_offset = OBJ_TAG(pthread_mutex_t);
	refcount_offset = OBJ_TAG(uint32_t);
	size_offset = OBJ_TAG(size_t);
	objc_selector_init();
	pthread_mutexattr_init(&recursiveAttributes);
	pthread_mutexattr_settype(&recursiveAttributes, PTHREAD_MUTEX_RECURSIVE);
}

int retain_object(struct objc_object ** obj)
{	
	return ATOMIC_INCREMENT(REFCOUNT(*obj));
}

int release_object(struct objc_object ** obj)
{
	return ATOMIC_DECREMENT(REFCOUNT(*obj));
}

void objc_lock_object(struct objc_object * obj)
{
	pthread_mutex_lock(&MUTEX(obj));
}

void objc_unlock_object(struct objc_object * obj)
{
	pthread_mutex_unlock(&MUTEX(obj));
}

struct objc_slot * objc_object_next_slot(id obj, SEL * selector)
{
	return SparseArrayNext(SLOTS(obj), selector);
}


struct objc_object * alloc_object(struct objc_object * prototype, size_t extra)
{
	size_t size = extra;
	if(prototype != NULL)
	{
		size += SIZE(prototype);
	}
	else
	{
		size += object_tags_size;
	}
	struct objc_object * obj = (void*)((char*)calloc(1, size) + object_tags_size);
	RECURSIVE_INIT(MUTEX(obj));
	SIZE(obj) = size;
	obj->isa = prototype;
	if(prototype != NULL)
	{
		retain_object(&obj->isa);
	}
	return obj;
}

void destroy_object(struct objc_object * obj)
{
	release_object(&obj->isa);
	pthread_mutex_destroy(&MUTEX(obj));
	free(((char*)obj - object_tags_size));
}

void install_custom_lookup(struct objc_object *obj, lookup_function fun)
{
	LOOKUP(obj) = fun;
}
static inline struct objc_slot * real_slot_lookup(struct objc_object ** obj, uint32_t sel, id sender)
{
	id object = *obj;
	struct objc_slot * ret = NULL;
	do
	{
		if(LOOKUP(object))
		{
			return (*LOOKUP(object))(obj, object, sel, sender);
		}
		if(SLOTS(object))
		{
			ret = SparseArrayLookup(SLOTS(object), sel);
		}
		object = object->isa;
	} while(object != NULL && ret == NULL);
	return ret;
}

bool objc_object_responds_to_selector(id obj, SEL sel)
{
	return real_slot_lookup(&obj, sel, nil) == NULL;
}
struct objc_slot * slot_lookup(struct objc_object ** obj, uint32_t sel, id sender)
{
	//0 is an invalid selector.
	if(sel == 0)
	{
		return NULL;
	}
	struct objc_slot * ret = real_slot_lookup(obj, sel, sender);
	if(ret == NULL && sel != forwardSelector)
	{
		return slot_lookup(obj, forwardSelector, sender);
	}
	return ret;
}

bool install_slot(struct objc_object *obj, 
		int offset,
		IMP method,
		char * types,
		uint32_t sel)
{
	//Type must match selector type
	char * seltypes = types_for_selector(sel);
	if(safestrcmp(seltypes, types) != 0)
	{
		return false;
	}
	objc_lock_object(obj);
	if(SLOTS(obj) == NULL)
	{
		SLOTS(obj) = SparseArrayNew();
	}
	struct objc_slot * oldSlot = SparseArrayLookup(SLOTS(obj), sel);
	if(oldSlot)
	{
		if(safestrcmp(types, oldSlot->types) != 0)
		{
			objc_unlock_object(obj);
			return false;
		}
		oldSlot->offset = offset;
		oldSlot->method = method;
	}
	else
	{
		//We are adding a new slot, not modifying an old one, so we need to 
		//invalidate a load of caches.
		id object = obj;
		oldSlot = slot_lookup(&object, sel, nil);
		struct objc_slot * newslot = calloc(1, sizeof(struct objc_slot));
		//TODO: Factor this out into a slot constructor
		newslot->offset = offset;
		newslot->method = method;
		if(types == NULL)
		{
			newslot->types = NULL;
		}
		else
		{
			newslot->types = strdup(types);
		}
		if(oldSlot != NULL)
		{
			oldSlot->version++;
		}
		SparseArrayInsert(SLOTS(obj), sel, newslot);
	}
	objc_unlock_object(obj);
	return true;
}
bool install_ivar_slot(struct objc_object *obj, 
		int offset,
		char * types,
		uint32_t sel)
{
	return install_slot(obj, offset, NULL, types, sel);
}
bool install_method_slot(struct objc_object *obj, 
		IMP method,
		char * types,
		uint32_t sel)
{
	return install_slot(obj, 0, method, types, sel);
}

