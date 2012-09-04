#ifndef beo_H_INCLUDED
#define beo_H_INCLUDED

// beo uses strings, dynar and hatab extensively
#include "str.h"

#include "mem.h"
#include "dynar.h"
#include "hatab.h"

#include "beo_proto.h"

/** Type checking macros for BeoValues. */

#define BEO_TYPE_P(VALUE, TYPE) beo_type_p((VALUE), (TYPE))
#define BEO_STRING_P(VALUE) BEO_TYPE_P((VALUE), BEO_STRING)
#define BEO_FAIL_P(VALUE)   BEO_TYPE_P((VALUE), BEO_FAIL)
#define BEO_OK_P(VALUE)     BEO_TYPE_P((VALUE), BEO_OK)
#define BEO_LONG_P(VALUE)   BEO_TYPE_P((VALUE), BEO_LONG)
#define BEO_DOUBLE_P(VALUE) BEO_TYPE_P((VALUE), BEO_DOUBLE)
#define BEO_STRING_P(VALUE) BEO_TYPE_P((VALUE), BEO_STRING)
#define BEO_LIST_P(VALUE)   BEO_TYPE_P((VALUE), BEO_LIST)
#define BEO_DATA_P(VALUE)   BEO_TYPE_P((VALUE), BEO_DATAPTR)
#define BEO_FUNC_P(VALUE)   BEO_TYPE_P((VALUE), BEO_FUNCPTR)

/** Simple conversion macros for BeoValues. 
These do NOT do any type checking or conversions,
just fetch the matching memberfrom the value's data union.
Also don't check for NULL values.
*/

#define BEO_GETDATA(VALUE, MEMBER)    ((VALUE)->data.MEMBER)           
#define BEO_TODOUBLE(VALUE)           BEO_GETDATA(VALUE, d)
#define BEO_TOSTRING(VALUE)           BEO_GETDATA(VALUE, s)
#define BEO_TOLONG(VALUE)             BEO_GETDATA(VALUE, l)
#define BEO_TODATAPTR(VALUE)          BEO_GETDATA(VALUE, p)

/** The function type for Beo script functions and commands.*/
typedef BeoValue * (BeoFunction)(int argc, BeoValue * argv[]);



#endif




