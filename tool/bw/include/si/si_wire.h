#ifndef SI_WIRE_H
#define SI_WIRE_H

#include "si.h"
#include "si_macro.h"

/** A SiWire is a a simple byte buffer that can also hold constant C strings. */
struct SiWire_;
typedef struct SiWire_ SiWire;


/** Initializes a wire copying size characters from str.  */
SI_API SiWire * siwire_init_size(SiWire * self, char * str, size_t size);

/** Initializes a wire from str.strlen(str) characters will be copied.  */
SI_API SiWire * siwire_init(SiWire * self, char * str);

/** Initializes a wire from a constant string. */
SI_API SiWire * siwire_init_const(SiWire * self, const char * str);

/** Creates a new wire with given contents. */
SI_API SiWire * siwire_new(char * str);
/** Creates a new wire with given contents and size. */
SI_API SiWire * siwire_new_size(char * str, size_t size);
/** Creates a new constant wire */
SI_API SiWire * siwire_const(const char * str);

/** Duplicates a wire. Must be freed with si_wire_free */
SI_API SiWire * siwire_dup(SiWire * self);


/** Creates a new empty wire */
SI_API SiWire * siwire_empty(); 

/** Checks if a rewire is empty. */
SI_API int siwire_empty_p(SiWire *self);

/** Returns a char * pointer thatthe wire is wrapping. */
SI_API char *   siwire_cstr(SiWire * self);

/** Returns the size of the Wire. */
SI_API size_t   siwire_size(SiWire * self);

/** Returns the room (space) available in the Wire. */
SI_API size_t   siwire_room(SiWire * self);

/** Return nonzero if self is a constant SiWire, zero if not.*/
SI_API int      siwire_const_p(SiWire * self);

/** Frees the contents of the wire (if non-constant) , but does not delete 
the wire itself. */
SI_API SiWire * siwire_done(SiWire * wire);

/** Frees the wire and it's contents. */
SI_API SiWire * siwire_free(SiWire * wire);

/** Concatenates char to self, growing self if needed. */
SI_API SiWire * siwire_addc(SiWire * self, char c);

/** Concatenates wire to self, growing self if needed. */
SI_API SiWire * siwire_add(SiWire * self, SiWire * wire);

/** Concatenates two Wires. */
SI_API SiWire * siwire_cat(SiWire * w1, SiWire * w2);

/** Gets a substring of the Wire. */
SI_API SiWire * siwire_mid(SiWire * self, size_t start, size_t amount);

/** Gets a left substring of the Wire. */
SI_API SiWire * siwire_left(SiWire * self, size_t amount);

/** Gets a right substring of the Wire. */
SI_API SiWire * siwire_right(SiWire * self, size_t amount);

/** Checks if the given index is valid for the wire. */
SI_API int siwire_index_ok(SiWire * self, size_t index);

/** Gets a character at index index from the wire. 
Returns 0 if index is not valid. */
SI_API char siwire_index(SiWire * self, size_t index);

/** Joins a variable amount of wires together with join in between them. */
SI_API SiWire * siwire_join_va(SiWire * join, size_t amount, ...);

/** Joins a variable amount of wires together with join in between them. */
SI_API SiWire * siwire_join_ar(SiWire * join, size_t amount, SiWire ** ar);

/** Compares two wires for equality in size and contents. */
SI_API int siwire_equal_p(SiWire * self, SiWire * wire);

/** Compares a wire and a c string for equality with size given. */
SI_API int siwire_equalcstrsize_p(SiWire * self, char * str, size_t size);

/** Compares a wire and a c string for equality in size and contents. */
SI_API int siwire_equalcstr_p(SiWire * self, char * str);

/** Calls puts on the wire. Useful for debugging. */
SI_API int siwire_puts(SiWire * self);



#endif 
