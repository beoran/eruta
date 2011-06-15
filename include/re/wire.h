#ifndef RE_WIRE_H
#define RE_WIRE_H

/** A ReWire is a a simple byte buffer that can also hold constant C strings. */
struct ReWire_;
typedef struct ReWire_ ReWire;


/** Initializes a wire copying size characters from str.  */
ReWire * rewire_init_size(ReWire * self, char * str, size_t size);

/** Initializes a wire from str.strlen(str) characters will be copied.  */
ReWire * rewire_init(ReWire * self, char * str);

/** Initializes a wire from a constant string. */
ReWire * rewire_init_const(ReWire * self, const char * str);

/** Creates a new wire with given contents. */
ReWire * rewire_new(char * str);
/** Creates a new wire with given contents and size. */
ReWire * rewire_new_size(char * str, size_t size);
/** Creates a new constant wire */
ReWire * rewire_const(const char * str);

/** Creates a new empty wire */
ReWire * rewire_empty(); 

/** Checks if a rewire is empty. */
int rewire_empty_p(ReWire *self);

/** Returns a char * pointer thatthe wire is wrapping. */
char *   rewire_cstr(ReWire * self);

/** Returns the size of the Wire. */
size_t   rewire_size(ReWire * self);

/** Returns the room (space) available in the Wire. */
size_t   rewire_room(ReWire * self);

/** Return nonzero if self is a constant ReWire, zero if not.*/
int      rewire_const_p(ReWire * self);

/** Frees the contents of the wire (if non-constant) , but does not delete 
the wire itself. */
ReWire * rewire_done(ReWire * wire);

/** Frees the wire and it's contents. */
ReWire * rewire_free(ReWire * wire);

/** Concatenates char to self, growing self if needed. */
ReWire * rewire_addc(ReWire * self, char c);

/** Concatenates wire to self, growing self if needed. */
ReWire * rewire_add(ReWire * self, ReWire * wire);

/** Concatenates two Wires. */
ReWire * rewire_cat(ReWire * w1, ReWire * w2);

/** Gets a substring of the Wire. */
ReWire * rewire_mid(ReWire * self, size_t start, size_t amount);

/** Gets a left substring of the Wire. */
ReWire * rewire_left(ReWire * self, size_t amount);

/** Gets a right substring of the Wire. */
ReWire * rewire_right(ReWire * self, size_t amount);

/** Checks if the given index is valid for the wire. */
int rewire_index_ok(ReWire * self, size_t index);

/** Gets a character at index index from the wire. 
Returns 0 if index if not valid. */
char rewire_index(ReWire * self, size_t index);

/** Joins a variable amount of wires together with join in between them. */
ReWire * rewire_join_va(ReWire * join, size_t amount, ...);

/** Joins a variable amount of wires together with join in between them. */
ReWire * rewire_join_ar(ReWire * join, size_t amount, ReWire ** ar);

/** Compares two wires for equality in size and contents. */
int rewire_equal_p(ReWire * self, ReWire * wire);

/** Compares a wire and a c string for equality with size given. */
int rewire_equalcstrsize_p(ReWire * self, char * str, size_t size);

/** Compares a wire and a c string for equality in size and contents. */
int rewire_equalcstr_p(ReWire * self, char * str);


/** Calls puts on the wire. Useful for debugging. */
int rewire_puts(ReWire * self);


#endif 
