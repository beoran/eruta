#ifndef ifa_H_INCLUDED
#define ifa_H_INCLUDED

/** Ifa contains functionality to emulate go language style interfaces in C. */

#define INTERFACE_BODY()    \
          void * self;
          

#define INTERFACE(NAME)     \
          struct name 

#define INTERFACE_SELF(IFA)  ((IFA)->self)

#define INTERFACE_INIT(IFA, SELF)  do { ((IFA)->self) = SELF; } while(0)


typedef struct Ifa_   Ifa;
typedef struct Metab_ Metab;

struct Metab_ {
  void * (*free)(void * ptr);
  void * (*done)(void * ptr);
};

struct Ifa_ {
  void         * data;
  struct Metab * methods;
};


/** Ref is a reference counted wrapper around arbitrary data pointer, */
struct Ref_ {
  struct Inter_ * inter;
  int             refcount;
};




#endif




