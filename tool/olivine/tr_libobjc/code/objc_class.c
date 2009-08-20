#include <string.h>
#include <stdio.h>
#include "objc_class.h"
#include "lock.h"

static SparseArray * classes;
DECLARE_LOCK(classes);

/**
 * This method is returned when the target is nil.
 */
id dummy_method(id self, SEL cmd)
{
	return nil;
}
/**
 * Custom lookup function used by classes.  Allows separate class and instance
 * methods.
 */
struct objc_slot * lookup_class_method(id *object, id obj, SEL sel, id sender)
{
	struct objc_slot * ret = NULL;
	//If we are looking up a class method:
	if(*object == obj)
	{
		Class class = (Class)obj;
		while(class != NULL && ret == NULL)
		{
			if(class->class_methods)
			{
				ret = SparseArrayLookup(class->class_methods, sel);
			}
			class = (Class)class->super_class;
		}
	}
	else
	{
		while(obj != nil && ret == NULL)
		{
			if(SLOTS(obj))
			{
				ret = SparseArrayLookup(SLOTS(obj), sel);
			}
			obj = obj->isa;
		}
	}
	if(ret == NULL && sel != forwardSelector)
	{
		return slot_lookup(object, forwardSelector, sender);
	}
	return ret;
}
struct objc_slot * lookup_instance_method(id *object, id obj, SEL sel, id sender)
{
	struct objc_slot * ret = NULL;
	//If we are looking up a class method:
	while(obj != nil && ret == NULL)
	{
		if(SLOTS(obj))
		{
			ret = SparseArrayLookup(SLOTS(obj), sel);
		}
		obj = obj->isa;
	}
	if(ret == NULL && sel != forwardSelector)
	{
		return slot_lookup(object, forwardSelector, sender);
	}
	return ret;
}

/**
 * Hash a class.
 */
static inline uint32_t hash_class(char * name)
{
	uint32_t hash = 5381;
	uint32_t c;
	if(name != NULL)
	{
		while((c = (uint32_t)*name++))
		{
			hash = hash * 33 + c;
		}
	}
	return hash;
}

static Class protocol_class;

void sarray_init(void);

__attribute__((constructor))
void objc_class_init()
{
	sarray_init();
	objc_object_init();
	INIT_LOCK(classes);
	classes = SparseArrayNew();
	protocol_class = objc_new_subclass("*Protocol", NULL, 0);
}

#define HASH_LOOKUP(result, table)\
	while(NULL != (result = SparseArrayLookup(table, hash)))\
	{\
		if(result == Nil)\
		{\
			UNLOCK(table);\
			return Nil;\
		}\
		if(strcmp(result->name, name) == 0)\
		{\
			break;\
		}\
		hash++;\
	}\

#define HASH_INSERT(value, temp, table)\
	do\
	{\
		while(NULL != (temp = SparseArrayLookup(table, hash)))\
		{\
			if(strcmp(temp->name, name) != 0)\
			{\
				hash++;\
			}\
		}\
		SparseArrayInsert(table, hash, value);\
	} while(0)

#define MANGLE_PROTOCOL_NAME() \
	int namelength = strlen(name);\
	char protocol_name[namelength + 10];\
	strncpy(protocol_name, name, namelength);\
	strncpy(protocol_name + namelength, "*Protocol", 9);\
	protocol_name[namelength + 9] = '\0';

Class objc_protocol_create(char * name)
{
	MANGLE_PROTOCOL_NAME();
	return objc_new_subclass(protocol_name, protocol_class, 0);
}

Class objc_protocol_for_name(char * name)
{
	MANGLE_PROTOCOL_NAME();
	return objc_class_for_name(protocol_name);
}
void objc_class_adopt_protocol(Class class, Class protocol)
{
	struct protocol_list * list = calloc(1, sizeof(struct protocol_list));
	list->protocol = protocol;
	objc_lock_object((id)class);
	list->next = class->protocols;
	class->protocols = list;
	//Add class methods
	for(SEL sel=0 ; sel<(SEL)-1 ; )
	{
		struct objc_slot * slot = SparseArrayNext(protocol->class_methods, &sel);
		if(slot != NULL && slot->offset == 0 && slot->method != (IMP)dummy_method)
		{
			//Install concrete methods only if the adopting class does not
			//already contain an implementation.
			if(!objc_object_responds_to_selector((id)class, sel))
			{
				objc_install_class_method(class, sel, slot->method);
			}
		}
	}
	//Add instance methods
	for(SEL sel=0 ; sel<(SEL)-1 ; )
	{
		struct objc_slot * slot = objc_object_next_slot((id)protocol, &sel);
		if(slot != NULL && slot->offset == 0 && slot->method != (IMP)dummy_method)
		{
			//Install concrete methods only if the adopting class does not
			//already contain an implementation.
			if(!objc_object_responds_to_selector((id)class, sel))
			{
				objc_install_instance_method(class, sel, slot->method);
			}
		}
	}
	objc_unlock_object((id)class);
}
bool objc_class_conforms_to_protocol(Class class, Class protocol)
{
	struct protocol_list * list = class->protocols;
	while(list != NULL)
	{
		if(strcmp(list->protocol->name, protocol->name) == 0)
		{
			return true;
		}
	}
	return false;
}

Class objc_new_subclass(char * name, Class super_class, int ivar_space)
{
	uint32_t hash = hash_class(name);
	Class oldClass;
	LOCK(classes);
	HASH_LOOKUP(oldClass, classes);
	Class class = (Class)alloc_object(NULL, sizeof(struct objc_class));
	class->super_class = super_class;
	class->instance_size = ivar_space - sizeof(struct objc_object);
	if(super_class != Nil)
	{
		class->instance_size += super_class->instance_size;
	}
	class->name = strdup(name);
	install_custom_lookup((id)class, lookup_class_method);
	Class temp = NULL;
	HASH_INSERT(class, temp, classes);
	UNLOCK(classes);
	return class;
}

void objc_register_ivar(Class class, char * name, char * type, int offset)
{
	if(class == Nil)
	{
		return;
	}
	struct ivar_list * ivar = malloc(sizeof(struct ivar_list));
	ivar->name = strdup(name);
	ivar->type = strdup(type);
	ivar->offset = offset;
	ivar->next = class->ivars;
	class->ivars = ivar;
}


void objc_class_pose_as(Class class, char * name)
{
	uint32_t hash = hash_class(name);
	Class oldclass;
	while(NULL != (oldclass = SparseArrayLookup(classes, hash)))
	{
		if(strcmp(oldclass->name, name) == 0)
		{
			LOCK(classes);
			SparseArrayInsert(classes, hash, class);
			UNLOCK(classes);
			return;
		}
		hash++;
	}
	LOCK(classes);
	SparseArrayInsert(classes, hash, class);
	UNLOCK(classes);
}
Class objc_class_for_name(char * name)
{
	uint32_t hash = hash_class(name);
	Class class;
	HASH_LOOKUP(class, classes);
	return class;
}

id objc_instantiate_class(Class class)
{
	id object = alloc_object((id)class, class->instance_size);
	install_custom_lookup(object, lookup_instance_method);
	return object;
}
void objc_install_instance_method(Class class, SEL sel, IMP method)
{
	char * types = types_for_selector(sel);
	install_method_slot((id)class, method, types, sel);
	if(types != NULL)
	{
		char * name = name_for_selector(sel);
		SEL untyped = lookup_typed_selector(name, NULL);
		//Also install the method for the untyped version of the selector.
		install_method_slot((id)class, method, NULL, untyped);
	}
}
//TODO: This is copy-and-pasted from objc_object.c.  Factor it out.
void objc_install_class_method(Class class, SEL sel, IMP method)
{
	char * seltypes = types_for_selector(sel);
	objc_lock_object((id)class);
	if(class->class_methods== NULL)
	{
		class->class_methods = SparseArrayNew();
	}
	struct objc_slot * oldSlot = SparseArrayLookup(class->class_methods, sel);
	if(oldSlot)
	{
		oldSlot->offset = 0;
		oldSlot->method = method;
	}
	else
	{
		//We are adding a new slot, not modifying an old one, so we need to 
		//invalidate a load of caches.
		oldSlot = NULL;
		Class superclass = class->super_class;
		while(superclass != NULL && oldSlot == NULL)
		{
			if(superclass->class_methods)
			{
				oldSlot = SparseArrayLookup(class->class_methods, sel);
			}
			class = (Class)class->super_class;
		}
		struct objc_slot * newslot = calloc(1, sizeof(struct objc_slot));
		newslot->method = method;
		if(seltypes == NULL)
		{
			newslot->types = NULL;
		}
		else
		{
			newslot->types = strdup(seltypes);
		}
		if(oldSlot != NULL)
		{
			oldSlot->version++;
		}
		SparseArrayInsert(class->class_methods, sel, newslot);
	}
	objc_unlock_object((id)class);
	if(seltypes != NULL)
	{
		char * selname = name_for_selector(sel);
		SEL untyped = lookup_typed_selector(selname, NULL);
		objc_install_class_method(class, untyped, method);
	}
}
