#ifndef __OBJC_OBJECT_H_INCLUDED__
#define __OBJC_OBJECT_H_INCLUDED__

#include <pthread.h>
#include <stdbool.h>
#include "sarray.h"
#include "selector.h"
#include "object_tags.h"

struct objc_object
{
	struct objc_object * isa;
};

typedef struct objc_call * CALL;
typedef struct objc_slot * SLOT;
#ifndef __OBJC__
typedef struct objc_object * id;
typedef id (*IMP)(id, CALL, ...);
#define nil (id)NULL
#endif

struct objc_slot
{
	int offset;
	IMP method;
	char * types;
	void * context;
	uint32_t version;
};

struct objc_call
{
	SLOT slot;
	SEL selector;
	id sender;
};
#define _cmd (_call->selector) 

/**
 * Methods should be implemented like this:
 * id method(id self, SLOT _call, ...);
*/

/**
 * Custom lookup function for when the standard selector to slot mapping is not
 * desired.
 */
typedef struct objc_slot* (*lookup_function)(id*, id, SEL, id sender);

/**
 * Macros for accessing slot dictionary and lookup function.
 */
extern int slots_offset;
#define SLOTS(obj) TAG(obj, slots_offset, SparseArray*)
extern int lookup_offset;
#define LOOKUP(obj) TAG(obj, lookup_offset, lookup_function)

/**
 * Selector identifying the slot to which forwarded messages should be
 * delivered.
 */
extern SEL forwardSelector;

/**
 * Initialise the object layer of the runtime.  Should be called one, on
 * loading the library.
 */
void objc_object_init(void);
/**
 * Marks a pointer as referring to an object.  
 */
int retain_object(struct objc_object ** obj);
/**
 * Marks an area of memory as no longer referring to an object.  Should be
 * called while it still does.
 */
int release_object(struct objc_object ** obj);
/**
 * Lock the object to prevent access from other threads.  Used to implement
 * @synchronized.
 */
void objc_lock_object(struct objc_object * obj);
/**
 * Unlock previously locked object.
 */
void objc_unlock_object(struct objc_object * obj);
/**
 * Create a new object that delegates to prototype.  Allocate extra additional
 * space at the end for storing instance variables.
 */
struct objc_object * alloc_object(struct objc_object * prototype, size_t extra);
/**
 * Destroy the specified object.
 */
void destroy_object(struct objc_object * obj);
/**
 * Install a custom lookup function.  This is called instead of the standard
 * lookup mechanism.
 */
void install_custom_lookup(struct objc_object *obj, lookup_function fun);
/**
 * Returns true if the object responds to the specified selector without
 * forwarding.
 */
bool objc_object_responds_to_selector(id obj, SEL sel);
/**
 * Returns the slot identified by the selector sel on object obj. sender 
 * represents the object who triggered the lookup by sending a message to obj.
 * sender can be nil when the function is called by the runtime itself, 
 * for example on invalidating the lookup cache or checking whether obj 
 * responds to selector sel.
 */
struct objc_slot * slot_lookup(struct objc_object ** obj, uint32_t sel, id sender);
/**
 * Install a slot which directly accesses an instance variable.  types should
 * match those of an accessor method.
 */
bool install_ivar_slot(struct objc_object *obj, 
		int offset,
		char * types,
		uint32_t sel);
/**
 * Install a method in a slot.  
 */
bool install_method_slot(struct objc_object *obj, 
		IMP method,
		char * types,
		uint32_t sel);
/**
 * Enumerate all of the slots in a given object.
 */
struct objc_slot * objc_object_next_slot(id obj, SEL * selector);

#endif // __OBJC_OBJECT_H_INCLUDED__
