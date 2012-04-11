#ifndef xml_H_INCLUDED
#define xml_H_INCLUDED

#include <ctype.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "xml_proto.h"

#define XML_GET_PROP(NODE, STR)\
        ((char *) xmlGetProp(NODE, (const xmlChar *)STR))

#define XML_BAD_VALUE -0xbad


#endif



