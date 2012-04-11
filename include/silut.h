#ifndef silut_H_INCLUDED
#define silut_H_INCLUDED

/** A Silut record  (String Integer LookUp Table consists of an integer and a constant character string. A Silut with string geing NULL is considered
to be the a guard record.
*/
struct Silut_;
typedef struct Silut_ Silut;

/** A Silut record  (String Integer LookUp Table consists of an integer and a constant character string. A Silut with string set to NULL is considered
to be the a guard record.
*/
struct Silut_ {
  int           integer;
  const char *  string;
};

/** Use this macro as the last guard element) of a Silut
lookup table. */
#define SILUT_DONE { -1, NULL } 

#include "silut_proto.h"


#endif




