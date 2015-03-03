#ifndef bxml_H_INCLUDED
#define bxml_H_INCLUDED


#include "str.h"
#include "ifa.h"

struct BXML_;
typedef struct BXML_ BXML;


/*
* Kind of the BXML node. Can be : ATTR, TEXT, TAG.
* Comments, entities and parsing instructions are ignored for now.
* These values have been chosen to be greater that parser state values.
*/
enum BXMLKind_
{
  BXML_TAG      = 101,
  BXML_TEXT     = 102,
  BXML_ATTR     = 103,
  BXML_COMMENT  = 104,
  BXML_DECLARE  = 105,
  BXML_LASTKIND = 106,
};



typedef struct Bxml_ Bxml;
typedef struct BxmlAttribute_ BxmlAttribute;

struct BxmlAttribute_ {
    char          * name;
    char          * value;
    BxmlAttribute * next;
};


struct Bxml_ {
    int             kind;       /* Kind of tag. */ 
    char          * name;       /* Tag name. #text for text nodes. */
    BxmlAttribute * attributes; /* First of linked list of tag attributes.  */
    BxmlAttribute * last_attr;  /* Last atrribute.  */
    char          * text;       /* Tag text content, NULL if none. */    
    Bxml          * sibling;    /* Sibling tag. */
    Bxml          * child;      /* First child tag, NULL if none */
    Bxml          * parent;     /* Parent tag, NULL if current tag is root tag */
    int             flags;      /* Additional status flags. */
    int             error;      /* Error flag. */
};

Bxml  * bxml_parse_str(char * str);
Bxml  * bxml_parse_buf(char * str, size_t len);
Bxml  * bxml_parse_file(FILE * file);
Bxml  * bxml_parse_filename(char * filename);

Bxml  * bxml_child(Bxml * xml, const char *name);
Bxml  * bxml_sibling(Bxml * xml);
Bxml  * bxml_next(Bxml * xml);
char  * bxml_text(Bxml * xml);
char  * bxml_name(Bxml * xml);
Bxml  * bxml_index(Bxml * xml);
char  * bxml_attribute(Bxml * xml, const char * name);
Bxml  * bxml_get(Bxml * xml, ...);
char  * bxml_to_str(Bxml * xml);
char ** bxml_processing_instructions(Bxml * xml);
char  * bxml_error(Bxml * xml);
Bxml  * bxml_free(Bxml * xml);
Bxml  * bxml_new(char *name, int kind);
Bxml  * bxml_add_child(Bxml * xml, Bxml * child);
Bxml  * bxml_add_new_child(Bxml * xml, const char *name);
Bxml  * bxml_set_attribute(Bxml * xml, const char *name, const char *value);



#endif




