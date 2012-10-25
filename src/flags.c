
#include "flags.h"



/** Simple bit twiddling functions for setting and getting 
flags (bits) on an integer field. */


/** Sets an individual flag on self. */
int flags_set(int * self, int flag) {
  return (*self) = (*self) | flag;
}

/** Returns the new flag that self should be set to for 
flag to be set in self. */
int flags_setvalue(int self, int flag) {
  return (self) | flag;
}


/** Unsets an individual flag on the widget. */
int flags_unset(int * self, int flag) {
  return (*self) = (*self) & (~flag);
}

/** Returns the new flag that self should be set to for 
flag to be unset in self. */
int flags_unsetvalue(int self, int flag) {
  return (self) & (~flag);
}


/** Sets or unsets an individual flag on the self. 
If set is true the flag is set, if false it's unset. */
int flags_put(int * self, int flag, int set) {
  if (set) { return flags_set(self, flag); }
  else { return flags_unset(self, flag); }
}

/** Sets or unsets an individual flag on the self. 
If set is true the flag is set, if false it's unset. */
int flags_putvalue(int self, int flag, int set) {
  if (set) { return flags_setvalue(self, flag); }
  else { return flags_unsetvalue(self, flag); }
}


/** Checks if an individual flag is set. */
int flags_get(int self, int flag) {
  return (self & flag) == flag;
}

/** Checks if an individual flag is set. Pass in a pointer. */
int flags_getptr(int * self, int flag) {
  return ((*self) & flag) == flag;
}
