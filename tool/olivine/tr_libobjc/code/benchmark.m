/**
 *
 * C API Example
 *
 * Author: David Chisnall
 *
 * This file contains an example of how the C API should be used.
 */
#include <stdio.h>
#include <time.h>
#include <objc/Object.h>

#define BENCHMARK_ITERATION 1000000

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

@interface AnObject : Object {
	int integer;
}
- (void) addToInteger:(int) anInt;
- (int) getInteger;
@end
@implementation AnObject
- (void) addToInteger:(int) anInt
{
	integer += anInt;
}
- (int) getInteger
{
	return integer;
}
@end

/**
 * That's all we need to construct the object.  Now let's try using it:
 */
int main(void)
{
	//Do the initial setup:
	//Instantiate it:
	id anObject = [AnObject new];
	//anObject = [AnObject new];
	clock_t c1, c2;
	c1 = clock();
	for(unsigned long long i=0 ; i<BENCHMARK_ITERATION ; i++)
	{
		[anObject addToInteger:1];
	}
	c2 = clock();
	printf("Uncached version took %f seconds.\n", ((double)c2 - (double)c1)/ (double)CLOCKS_PER_SEC);
	int i = [anObject getInteger];
	printf("Integer is %d\n", i);

	return 0;
}
