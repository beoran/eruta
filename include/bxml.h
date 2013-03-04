#ifndef bxml_H_INCLUDED
#define bxml_H_INCLUDED


#include "str.h"
#include "ifa.h"

struct BXML_;
typedef struct BXML_ BXML;

BXML * bxml_init (BXML * self, int kind);
BXML * bxml_alloc(void);
BXML * bxml_new (int kind);
BXML * bxml_done (BXML * self );
BXML * bxml_free (BXML * self );
BXML * bxml_addsibling (BXML * self , BXML * other );
BXML * bxml_addattribute (BXML * self , BXML * attr );
BXML * bxml_addchild (BXML * self , BXML * child );


BXML * bxml_parse_strn(char * str, size_t size );
BXML * bxml_parse_str(char * str );
BXML * bxml_readfile(char * filename );


#endif




