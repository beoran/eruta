#ifndef bxml_H_INCLUDED
#define bxml_H_INCLUDED


#include "str.h"
#include "ifa.h"

/*
* Kind of the BXML node. Can be : ATTR, TEXT, TAG.
* Comments, entities and parsing instructions are ignored for now.
* These values have been chosen to be greater that parser state values.
*/
enum BXMLKind_
{
  /* Root node */
  BXML_ROOT     = 100,
  /* Normal tag*/
  BXML_TAG      = 101,
  /* Text node. */
  BXML_TEXT     = 102,
  /* Comment node. */
  BXML_COMMENT  = 104,
  /* Declare node. */
  BXML_DECLARE  = 105,
  BXML_LASTKIND = 106,
};



typedef struct Bxml_ Bxml;
typedef struct BxmlAttribute_ BxmlAttribute;
typedef struct BxmlBuffer_ BxmlBuffer;

struct BxmlAttribute_ {
    char          * name;
    char          * value;
    BxmlAttribute * next;
};


struct BxmlBuffer_ {
  char * text;
  size_t size;
  size_t space;
};


struct Bxml_ {
    int             kind;       /* Kind of tag. */ 
    char          * name;       /* Tag name. #text for text nodes. */
    char          * text;       /* Tag text content, NULL if none. */    
    BxmlAttribute * attributes; /* First of linked list of tag attributes.  */
    BxmlAttribute * last_attr;  /* Last atrribute.  */
    Bxml          * sibling;    /* Sibling tag. */
    Bxml          * child;      /* First child tag, NULL if none */
    Bxml          * parent;     /* Parent tag, NULL if current tag is root tag */
    int             flags;      /* Additional status flags. */
};


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
Bxml  * bxml_new(int kind, ...);
Bxml  * bxml_new_va(int kind, va_list args);
Bxml  * bxml_new_size_va(int kind, int size, va_list args);
Bxml  * bxml_new_size(int kind, int size, ...);

Bxml  * bxml_add_child(Bxml * xml, Bxml * child);
Bxml  * bxml_new_child_va(Bxml * xml, int kind, va_list args);
Bxml  * bxml_new_child(Bxml * xml, int kind, ...);

Bxml * bxml_merge_text(Bxml * text1, Bxml * text2);
Bxml * bxml_merge_adjacent_text(Bxml * text1, Bxml * text2);

Bxml * bxml_get_last_sibling(Bxml * me);
Bxml * bxml_get_last_child(Bxml * me);

Bxml * bxml_append_sibling(Bxml * me, Bxml * you);
Bxml * bxml_get_sibling_at(Bxml * me, int pos);
Bxml * bxml_get_child_at(Bxml * me, int pos);
BxmlAttribute *
bxml_set_attribute(Bxml * xml, char *name, char *value);

Bxml * bxml_append_buf(Bxml * me, char * buf, int bufsize);

BxmlAttribute * bxml_new_attribute_size(Bxml * me, char * name, int namesize, char * value, int valuesize);
BxmlAttribute * bxml_new_attribute(Bxml * me, char * name, char * value);
BxmlAttribute * bxml_get_attribute_pointer(Bxml * me, char * name);
char * bxml_get_attribute(Bxml * me, char * name);

Bxml  * bxml_parse_str(char * str);
Bxml  * bxml_parse_buf(char * str, size_t len);
Bxml  * bxml_parse_file(FILE * file);
Bxml  * bxml_parse_filename(char * filename);


void bxml_print_element_names(Bxml * node);
void bxml_print_all_attributes(Bxml * node);

Bxml * bxml_find_next_kind(Bxml * node, const char * name, int kind);
Bxml * bxml_find_next(Bxml * node, const char * name);
Bxml * bxml_find_child_kind(Bxml * node, const char * name, int kind);
Bxml * bxml_find_child(Bxml * node, const char * name);
Bxml * bxml_find_child_deep_va(Bxml * node, va_list args);
Bxml * bxml_find_child_deep(Bxml * node, ...);
long * bxml_get_attribute_long(Bxml * node, char * name, long * result);
double * bxml_get_attribute_double(Bxml * node, char * name, double * result);
Bxml * bxml_get_text_tag_under(Bxml * node);
char * bxml_get_text_under(Bxml * node);

int bxml_show_to(Bxml * xml, FILE * out, int depth);




#endif




