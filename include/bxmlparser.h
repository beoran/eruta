#ifndef bxmlparser_H_INCLUDED
#define bxmlparser_H_INCLUDED

#include "bxml.h"

typedef struct BxmlParser_ BxmlParser;


BxmlParser * bxmlparser_new();
BxmlParser * bxmlparser_free(BxmlParser * me);
Bxml * bxmlparser_parse_buf(BxmlParser * me, char * buf, int size);
Bxml * bxmlparser_parse_str(BxmlParser * me, char * buf);
Bxml * bxmlparser_parse_file(BxmlParser * me, FILE * file);
Bxml * bxmlparser_parse_filename(BxmlParser * me, char * filename);
const char * bxmlparser_get_error(BxmlParser * me);

int bxmlparser_get_line(BxmlParser * me);
int bxmlparser_get_column(BxmlParser * me);



#endif




