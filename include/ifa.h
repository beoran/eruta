#ifndef ifa_H_INCLUDED
#define ifa_H_INCLUDED

/*
#include "ifa_proto.h"
*/

/** ifa contains functionality to emulate go language style interfaces in C. */

#define INTERFACE_BODY()    \
          void * self;
          

#define INTERFACE(NAME)     \
          struct name 

#define INTERFACE_SELF(IFA)  ((IFA)->self)

#define INTERFACE_INIT(IFA, SELF)  do { ((IFA)->self) = SELF; } while(0)

#endif




