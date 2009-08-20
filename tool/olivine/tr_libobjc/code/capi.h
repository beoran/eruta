#include <stddef.h>
#include "objc_object.h"
#include "selector.h"
#include "objc_class.h"

#ifdef __APPLE__
#define __thread 
#endif

/**
 * Definition of self visible at the global scope. 
 */
extern id __attribute__((unused))self;
/**
 * Defines a new class whose objects have the layout of struct name.  If the
 * class inherits from another class then the fields in the structure
 * representing the superclass must be replicated in the child.  This macro
 * does not register the location of the instance variables with the runtime.
 * That must be done with the REGISTER_IVAR() macro.
 */
#define CLASS_FROM_STRUCT(name, superclass)\
	objc_new_subclass(#name, superclass, sizeof(struct name))
/**
 * Registers an instance variable as belonging to a class.  The first
 * parameter, class, should be the class returned by CLASS_FROM_STRUCT() and
 * the structname the same as the name passed to that macro.  The typestring
 * should be a string in the standard Objective-C type encoding and the
 * ivarname the name of the instance variable being registered.
 */
#define REGISTER_IVAR(class, structname, typestring, ivarname)\
	objc_register_ivar(class, #ivarname, typestring, offsetof(struct structname, ivarname))
/**
 * Install an instance method in a specific class.  The selname and seltypes
 * arguments give the name and type string of the selector and the method
 * implementation.  Every method should accept an object pointer (id) as the
 * first argument and a selector as the second.  The type string for a simple
 * method with no additional arguments, returning an object, is thus "@@:"
 * (object, object, selector - the first type specified is the return type).
 */
#define ADD_INSTANCE_METHOD(class, selname, seltypes, method)\
	do\
	{\
		SEL method ## selector = register_typed_selector(selname, seltypes);\
		objc_install_instance_method(class, method ## selector, (IMP)method);\
	} while(0)
/**
 * Adds a class method to the specified class.  Arguments are the same as for
 * ADD_INSTANCE_METHOD().
 */
#define ADD_CLASS_METHOD(class, selname, seltypes, method)\
	do\
	{\
		SEL method ## selector = register_typed_selector(selname, seltypes);\
		objc_install_class_method(class, method ## selector, (IMP)method);\
	} while(0)

/**
 * Perform message dispatch.  Called from other macros that perform message
 * lookup as well.
 */
#define CALL_SLOT(ret, slot, obj, ...)\
			if(slot != NULL)\
			{\
				struct objc_call call = { slot; selector; sender };\
				ret = slot->method((id)object, &call, ##__VA_ARGS__);\
			}\
			else\
			{\
				ret = NULL;\
			}\

#define MSG_SEND_SUPER(ret, sel_name, sel_types, ...)\
		do\
		{\
			static SEL selector = lookup_typed_selector(sel_name, sel_types);\
			id object = self->isa;\
			struct objc_slot * slot = slot_lookup(&object, selector, sender);\
			CALL_SLOT(ret, slot, self, __VA_ARGS__);\
		} while(0)
struct objc_slot * lookup_instance_method(id *object, id obj, SEL sel, id sender);
/**
 * Perform slot lookup and (simple version, no caching).
 * obj is the receiver.
 * sel_name is the name of the selector.
 * sel_types is the type of the selector.
 * action is the action to be performed on the slot (typically a MSG_SEND()).
 * See example.c for examples of this.
 */
#define SLOT_LOOKUP(obj, sel_name, sel_types, sender, action)\
		do\
		{\
			static SEL selector = 0;\
			if(selector == 0)\
			{\
				selector = lookup_typed_selector(sel_name, sel_types);\
			}\
			id object = (id)obj;\
			struct objc_slot * slot;\
			if(LOOKUP(object))\
			{\
				slot = LOOKUP(object)(&object, object, selector, sender);\
			}\
			else\
			{\
				slot = slot_lookup(&object, selector, sender);\
			}\
			struct objc_call call = { slot, selector, sender };\
			action\
		} while(0)

			
struct inline_cache_line
{
	id type;
	struct objc_slot * slot;
	uint32_t version;
};

/**
 * Perform message lookup with monomorphic inline caching.  Use this where the
 * same message is sent to the same type of object a lot.
 */
#define SLOT_LOOKUP_MIC(obj, sel_name, sel_types, sender, action)\
		do\
		{\
			static SEL selector = 0;\
			struct objc_slot * slot;\
			if(selector == 0)\
			{\
				selector = lookup_typed_selector(sel_name, sel_types);\
			}\
			static __thread struct inline_cache_line cache;\
			if(cache.slot != NULL \
				&&\
				cache.type == (id)obj->isa\
				&&\
				cache.version == cache.slot->version)\
			{\
				slot = cache.slot;\
			}\
			else\
			{\
				id object = (id)obj;\
				slot = slot_lookup(&obj, selector, sender);\
				if(obj == object)\
				{\
					cache.version = slot->version;\
					cache.slot = slot;\
					cache.type = obj->isa;\
				}\
			}\
			struct objc_call call = { slot, selector, sender };\
			action\
		} while(0)

/**
 * Perform message lookup with polymorphic inline caching.  degree indicates
 * the size of the inline cache to create (the number of cache lines).  Calling
 * this with degree set to one is equivalent SLOT_LOOKUP_MIC(), although likely
 * to be less efficient.  Use this where profiling indicates that a lot of time
 * is spent performing message lookups.  Increase degree until no further
 * performance gain occurs.
 */
#define SLOT_LOOKUP_PIC(degree, obj, sel_name, sel_types, sender, action)\
		do\
		{\
			static SEL selector = 0;\
			if(selector == 0)\
			{\
				selector = lookup_typed_selector(sel_name, sel_types);\
			}\
			static __thread struct inline_cache_line cache[degree];\
			static __thread unsigned int replace = 0;\
			struct objc_slot * slot = NULL;\
			for(unsigned int i=0 ; i<degree ; i++)\
			{\
				if(cache[i].slot != NULL \
					&&\
					cache[i].type == obj->isa\
					&&\
					cache[i].version == cache[i].slot->version)\
				{\
					slot = cache[i].slot;\
					break;\
				}\
			}\
			if(slot == NULL)\
			{\
				id object = (id)obj;\
				slot = slot_lookup(&obj, selector, sender);\
				if(obj == object)\
				{\
					replace++;\
					if(replace >= degree)\
					{\
						replace = 0;\
					}\
					cache[replace].version = slot->version;\
					cache[replace].slot = slot;\
					cache[replace].type = obj->isa;\
				}\
			}\
			struct objc_call call = { slot, selector, sender };\
			action\
		} while(0)

/**
 * Send a message to an object.
 */

#define MSG_SEND(object, ...)\
	((slot != NULL) ? ((slot)->method((id)object, &call, ## __VA_ARGS__)) : nil )
	
/* MSG_SEND Variants

inline struct objc_call make_objc_call(struct objc_slot * slot, SEL selector, id sender)
{
	struct objc_call call = { slot, selector, sender };
	return call;
}

#define MSG_SEND(object, ...)\
	((slot != NULL) ? (slot->method((id)object, make_objc_call(slot, selector, sender), ## __VA_ARGS__)) : nil )	
#define MSG_CALL(object, call, ...)\
	((call != NULL && call.slot != NULL) ? (call.slot->method((id)object, call, ## __VA_ARGS__)) : nil )
*/

/**
 * Call a 'get' method.
 */
#define CALL_GET_METHOD(object, value)\
	if(slot != NULL)\
	{\
		if(slot->offset != 0)\
		{\
			value = *(__typeof__(value)*)(((char*)object) + slot->offset);\
		}\
		else\
		{\
			value = (__typeof__(value))slot->method((id)object, &call);\
		}\
	}\
	else\
	{\
		value = (__typeof__(value))0;\
	}
/**
 * Call a 'set' method.
 */
#define CALL_SET_METHOD(object, value)\
	if(slot != NULL)\
	{\
		if(slot->offset != 0)\
		{\
			*(__typeof__(value)*)(((char*)object) + slot->offset) = value;\
		}\
		else\
		{\
			slot->method((id)object, &call);\
		}\
	}
