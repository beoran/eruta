#ifndef xml_H_INCLUDED
#define xml_H_INCLUDED

#include <ctype.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

/* This file was generated with:
'cfunctions -c -aoff -n -w xml_proto src/xml.c' */
#ifndef CFH_XML_PROTO
#define CFH_XML_PROTO

/* From 'src/xml.c': */

xmlAttr * xmlFindAttr (xmlNode * node , const char * name );

char * xmlGetAttr (xmlNode * node , const char * name );

void print_element_names (xmlNode * node );

void print_all_attributes (xmlNode * node );

xmlNode * xmlFindNextType (xmlNode * node , const char * name , xmlElementType type );

xmlNode * xmlFindNext (xmlNode * node , const char * name );

xmlNode * xmlFindChild (xmlNode * node , const char * name );

xmlNode * xmlFindChildDeep (xmlNode * node , ... );

int xmlGetPropInt (xmlNode * node , const char * name );

char * xmlPropertyValue (xmlNode * firstprop , char * name );

int * xmlPropertyValueInt (xmlNode * firstprop , char * name , int * result );

char * xmlNodeChildContentRef (xmlNode * node );

#endif /* CFH_XML_PROTO */

/** Shortcut for xmlGetProp. Note: xmlFree() must be called on the result of this when done. */
#define XML_GET_PROP(NODE, STR)\
        ((char *) xmlGetProp(NODE, (const xmlChar *)STR))
        

#define XML_BAD_VALUE -0xbad


#endif




