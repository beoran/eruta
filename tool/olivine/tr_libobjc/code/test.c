#include <stdio.h>
#include <string.h>
#include "sarray.h"
#include "selector.h"
#include "objc_object.h"
#include "objc_class.h"

extern id __attribute__((unused))self;

#define TEST_LOG(condition, error, ...) do {if((condition)) { printf(error, ## __VA_ARGS__); return -1;} }while(0)
#define OBJC_CALL(call, slt, sel, caller)\
	call.slot = slt;\
	call.selector = sel;\
	call.sender = caller;

static int foo_global = 12;
id foo(id self, CALL _call, id bar)
{
	foo_global++;
	return NULL;
}
id bar(id self, CALL _call, id bar)
{
	foo_global--;
	return NULL;
}

int object_test(void)
{
	id object = alloc_object(NULL,0);
	SEL foo_sel = register_typed_selector("foo:", "@@:@");
	struct objc_slot * slot = NULL;
	struct objc_call call;

	install_method_slot(object, (IMP)foo, "@@:@", foo_sel);
	slot = slot_lookup(&object, foo_sel, nil);
	TEST_LOG((slot == NULL),"Slot lookup failed.\n");
	TEST_LOG((slot->method == NULL), "Method lookup failed.\n");	
	OBJC_CALL(call, slot, foo_sel, nil);
	slot->method(object, &call , NULL);
	TEST_LOG((foo_global != 13), "Method call failed.\n");
	
	id object2 = alloc_object(object, 0);
	slot = slot_lookup(&object2, foo_sel, nil);
	OBJC_CALL(call, slot, foo_sel, nil);
	slot->method(object2, &call, NULL);
	TEST_LOG((foo_global != 14), "Inherited method call failed.\n");
	
	install_method_slot(object2, (IMP)bar, "@@:@", foo_sel);
	slot = slot_lookup(&object2, foo_sel, nil);
	OBJC_CALL(call, slot, foo_sel, nil);
	slot->method(object2, &call, NULL);
	TEST_LOG((foo_global != 13), "Overridden method call failed.\n");
	
	printf("Object tests passed!\n");
	return 0;
}

int selector_test(void)
{
	SEL foo = register_typed_selector("foo:bar:", "@@");
	TEST_LOG((strcmp(name_for_selector(foo), "foo:bar:") != 0),
			"Incorrect name for registered selector\n");
	TEST_LOG((strcmp(types_for_selector(foo), "@@") != 0),
		"Incorrect type for registered selector\n");
	TEST_LOG((lookup_typed_selector("foo:bar:", "@@") != foo),
		"Incorrect value for registered selector\n");
	printf("Selector tests passed.\n");
	return 0;
}

int SparseArrayTest(void)
{
	SparseArray * s = SparseArrayNew();
#define SET(x) SparseArrayInsert(s, x, (void*)x)
	SET(1);
	SET(100000);
	SET(11234);
	SET(101);
	SET(550116370);
	SET(193457001);
#define TEST(x) do { TEST_LOG((SparseArrayLookup(s, x) != (void*) x), "Incorrect value for %d\n", x); } while(0)
	TEST(1);
	TEST(100000);
	TEST(11234);
	TEST(101);
	TEST(550116370);
	TEST(193457001);
	TEST_LOG((SparseArrayLookup(s, 193457000) != 0), "Incorrect value for empty element\n");
	TEST_LOG((SparseArrayLookup(s, 10) != 0), "Incorrect value for empty element\n");
	uint32_t i=0;
#define CHECK_NEXT(x) do { void * ret; TEST_LOG(((ret = SparseArrayNext(s, &i)) != ((void*) (unsigned long) x)) , "Incorrect next value (expected %d, got %d)\n", x, ((int) (long)ret)); }while(0)
	CHECK_NEXT(1);
	CHECK_NEXT(101);
	CHECK_NEXT(11234);
	CHECK_NEXT(100000);
	CHECK_NEXT(193457001);
	CHECK_NEXT(550116370);
	printf("Sparse array tests passed.\n");
	return 0;
}

struct Foo
{
	id isa;
	int i;
	float f;
};

void seti(struct Foo * self, CALL _call, int anInt)
{
	self->i = anInt;
}

int class_test(void)
{
	//Check class creation
	Class Foo = objc_new_subclass("Foo", NULL, sizeof(struct Foo));
	Class Foo2 = objc_class_for_name("Foo");
	struct objc_slot * slot = NULL;
	struct objc_call call;

	TEST_LOG((Foo != Foo2),
		"Class lookup by name failed (0x%x != 0x%x).\n", ((int) (long)Foo), ((int) (long)Foo2));
	SEL seti_sel = register_typed_selector("seti", "v@:i");
	SEL foo_sel = register_typed_selector("foo", "@@:@");
	
	//Check class and instance method dispatch
	objc_install_instance_method(Foo, seti_sel, (IMP)seti);
	objc_install_class_method(Foo, foo_sel, (IMP)foo);
	id aFoo = objc_instantiate_class(Foo);
	slot = slot_lookup(&aFoo, seti_sel, nil);
	TEST_LOG((slot==NULL || slot->method != (IMP)seti),
		"Instance method lookup failed.\n");		
	
	OBJC_CALL(call, slot, seti_sel, nil);
	slot->method(aFoo, &call, 12);
	TEST_LOG((((struct Foo*)aFoo)->i != 12), "Instance method failed\n");
	slot = slot_lookup(&aFoo->isa, foo_sel, nil);
	TEST_LOG((slot==NULL || slot->method != (IMP)foo),
		"Class method lookup failed.\n");
		
	//Test subclassing:
	Foo2 = objc_new_subclass("Foo2", Foo, sizeof(struct Foo));
	id aFoo2 = objc_instantiate_class(Foo2);
	slot = slot_lookup(&aFoo2, seti_sel, nil);
	TEST_LOG((slot==NULL || slot->method != (IMP)seti),
		"Inherited instance method lookup failed.\n");
	OBJC_CALL(call, slot, seti_sel, nil);
	slot->method(aFoo2, &call, 12);
	TEST_LOG((((struct Foo*)aFoo2)->i != 12), "Inherited instance method failed\n");
	slot = slot_lookup(&aFoo2->isa, foo_sel, nil);
	TEST_LOG((slot==NULL || slot->method != (IMP)foo),
		"Inherited class method lookup failed.\n");

	printf("Class tests passed!\n");
	return 0;
}


int main(void)
{
	//objc_class_init();
	return SparseArrayTest() || selector_test() || object_test() || class_test();
}
