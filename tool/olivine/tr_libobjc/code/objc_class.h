#ifndef __OBJC_CLASS_H_INCLUDED
#define __OBJC_CLASS_H_INCLUDED
#include "objc_object.h"
#include "selector.h"

typedef SparseArray * dtable;

/**
 * Instance variable structure from the ivar list.  Not used by the runtime,
 * but permits introspection in running programs.
 */
struct ivar_list
{
	char * name;
	char * type;
	int offset;
	struct ivar_list * next;
};

id dummy_method(id,SEL);

#ifndef __OBJC__
typedef struct objc_class* Class;
#define Nil (Class)NULL
#endif

struct protocol_list 
{
	Class protocol;
	struct protocol_list * next;
};

/**
 * Structure representing a class.  Classes are objects with a few static
 * instance variables.  This will become an opaque type in future.
 */
struct objc_class 
{
	Class super_class;
	dtable class_methods;
	char * name;
	struct ivar_list * ivars;
	struct protocol_list * protocols;
	int instance_size;
};

/**
 * Initialise the library.  
 */
void objc_class_init();
/**
 * Create a new protocol with the specified name.  Note that protocols
 * (concrete or otherwise) are just classes which inherit from Protocol.  
 */
Class objc_protocol_create(char * name);
/**
 * Returns the protocol for the specified name.
 */
Class objc_protocol_for_name(char * name);
/**
 * Add a method to a protocol.  For an abstract protocol, pass NULL as the
 * method.
 */
void objc_protocol_add_method(Class protocol, SEL selector, IMP method);
/**
 * Instruct a class to adopt a specified protocol.  All methods in the protocol
 * that are not already implemented by the class will be added.  Note: This can
 * also be used to implement mixins.
 */
void objc_class_adopt_protocol(Class class, Class protocol);
/**
 * Check a class for protocol conformance.  Note that this only checks whether
 * the class has adopted a protocol of the same name, it does not check whether
 * the methods have been implemented.
 */
bool objc_class_conforms_to_protocol(Class class, Class protocol);
/**
 * Create a new subclass with the specified name.
 */
Class objc_new_subclass(char * name, Class super_class, int ivar_space);
/**
 * Register the location and type of an instance variable.
 */
void objc_register_ivar(Class class, char * name, char * type, int offset);
/**
 * Look up the name of a class.
 */
Class objc_class_for_name(char * name);
/**
 * Causes lookups to the class specified by name to be redirected to the given
 * class.
 */
void objc_class_pose_as(Class class, char * name);
/**
 * Create an instance of a class.
 */
id objc_instantiate_class(Class class);
/**
 * Set a class method.
 */
void objc_install_class_method(Class class, SEL sel, IMP method);
/**
 * Set an instance method.
 */
void objc_install_instance_method(Class class, SEL sel, IMP method);
#endif //__OBJC_CLASS_H_INCLUDED
