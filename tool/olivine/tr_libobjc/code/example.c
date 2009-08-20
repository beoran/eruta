/**
 *
 * C API Example
 *
 * Author: David Chisnall
 *
 * This file contains an example of how the C API should be used.
 */
#include "capi.h"
#include <stdio.h>

/**
 * Consider the following simple Objective-C class:

@interface AnObject {
int integer;
}
+ (AnObject*) new;
- (void) addToInteger:(int)anInt;
- (int) getInteger;
@end

 * We will first see how to construct this object using the C API.
 *
 * The first step is to define a structure representing its instances:
 */

struct AnObject
{
	id isa;
	int integer;
};

/**
 * This will be used to hold a pointer to the class object.  For now we
 * initialise it to Nil.
 */
Class AnObject = Nil;
/**
 * With this defined, we next define the functions that implement the methods:
 */
struct AnObject * AnObject_new(id self, SEL cmd)
{
	return (struct AnObject *) objc_instantiate_class(AnObject);
}
void AnObject_addToInteger(struct AnObject *  self, SEL cmd, int anInt)
{
	self->integer += anInt;
}
int AnObject_getInteger(struct AnObject * self, SEL cmd)
{
	return self->integer;
}
/**
 * This takes care of all of the implementation.  Now we need to register it
 * all with the runtime.  We'll do this in a single function:
 */
void register_AnObject(void)
{
	//This class has no superclass
	AnObject = CLASS_FROM_STRUCT(AnObject, NULL);
	//Now we register the instance variables:
	REGISTER_IVAR(AnObject, AnObject, "i", integer);
	//Register the +new method:
	ADD_CLASS_METHOD(AnObject, "new", "@@:", AnObject_new);
	//And now the two instance methods:
	ADD_INSTANCE_METHOD(AnObject, "addToInteger:", "v@:i", AnObject_addToInteger);
	ADD_INSTANCE_METHOD(AnObject, "getInteger", "i@:", AnObject_getInteger);
}
/**
 * That's all we need to construct the object.  Now let's try using it:
 */
int main(void)
{
	//Do the initial setup:
	register_AnObject();
	//Instantiate it:
	id anObject;
	//anObject = [AnObject new];
	SLOT_LOOKUP(AnObject, "new", "@@:", nil, {
			anObject = MSG_SEND(AnObject);
			});
	//[anObject addToInteger:12];
	//[anObject addToInteger:30];
	SLOT_LOOKUP(anObject, "addToInteger:", "v@:i", nil, 
			{
				MSG_SEND(anObject, 12);
				MSG_SEND(anObject, 30);
			});
	//int i = [anObject getInteger];
	int i;
	SLOT_LOOKUP(anObject, "getInteger", "i@:", nil, 
			{
				i = (int)(long)MSG_SEND(anObject);
			});
	printf("Integer is %d (the ivar is %d)\n", i, ((struct AnObject *)anObject)->integer);

	return 0;
}
