
#include "bxml.h"
#include "bstraux.h"

#include <stdlib.h>
#include <string.h>

/* Simplistic, non-conforming XML parser that parses XML documents into 
structs. */

/**
* Kind of the self. Can be : ATTR, TEXT, TAG, 
* Comments, entities and parsing instructions are ignored for now
*/
enum BXMLKind_
{
  BXML_TAG  = 1,
  BXML_TEXT = 2,
  BXML_ATTR = 3
};


/**
* Results of a parse
*/
enum BXMLResult_
{
  BXML_OK         = 0,
  BXML_INCOMPLETE = 1,
  BXML_ERROR      = 2,
};



/**
* An BXML represents one <tag></tag> self and it's children and attributes,
* or the attributes of a tag.
*/
struct BXML_
{
  BXML * parent, * next, * before, * child, * attr;
  /* Different linked selfs: parent is the parent, next is the next sbling, previous is the previous sibling, child is the first child self,
  attribute is the first attribute (linked by next and previous too) */
  bstring name; 
  /* for tags, this is the name of the tag, for attributes, 
  the name of the attribute. */
  bstring value; /* For attributes this is the value of the attribute.*/
  int kind; /* kind of tag it is. */
};

/**
* BXMLParser is the parser interface.
*/ 
struct BXMLParser_ {
  INTERFACE_BODY();
  int (*now) (struct BXMLParse_ * iface);
  int (*pos) (struct BXMLParse_ * iface);
  int (*pos_)(struct BXMLParse_ * iface, int pos);
};


/**
* BXMLParse is the parser object.
*/
struct BXMLParse_ 
{
  char * data;
  size_t size;
  size_t now;
};


/* Initializes a BXML self */
BXML * bxml_init(BXML * self, int kind, BXML * parent) {
  if(!self) return NULL;
  self->parent = parent;
  self->next   = NULL;
  self->before = NULL;
  self->child  = NULL;
  self->attr   = NULL;
  self->kind   = kind;
  self->name   = bfromcstr("");
  self->value  = bfromcstr("");
  return self;
}

/** Allocates a new bxml node */
BXML * bxml_alloc() {
  return calloc(sizeof(BXML), 1);
}

/** Allocates and initialzes a new bxml node. */
BXML * bxml_new(int kind, BXML * parent) {
  return bxml_init(bxml_alloc(), kind, parent);
}

/** Clean usp a bxml  node, freeing the children recursively. */
BXML * bxml_done(BXML * self) {
  BXML * aid  = self->child;
  BXML * next = NULL;
  while(aid) { 
    next = aid->next; // get next, since aid will be destroyed.
    bxml_free(aid);   // free first child
    aid  = next;      // move to next child. 
  }
  aid = self->attr;
  while(aid) { 
    next = aid->next; // get next, since aid will be destroyed.
    bxml_free(aid);  // free first attribute
    aid  = next;     // move to next attribute.
  }
  bdestroy(self->name);
  bdestroy(self->value);
  return self;
}

/** Frees this node and cleans up its children recursively. returns NULL. */
BXML * bxml_free(BXML * self) {
  if(!self) return NULL;
  bxml_done(self);
  free(self);
  return NULL;
}

/* according to the XML standard, these are spaces */
static const char BXML_SPACE_STR[] = { 0x20,  0x09, 0x0D, 0x0A, 0x00 };



/* This macro advances the current character. */
#define STATE_SKIP(STR, NOW, INDEX, SIZE, BY)           \
        do {                                            \
          INDEX+=BY;                                    \
          if(INDEX>=SIZE) return NULL;                  \
          NOW = STR[INDEX];                             \
        } while(0);
        
        
/* This macro advances the current character as long as it is in SET. */
#define STATE_SKIP_SET(STR, NOW, INDEX, SIZE, SET)      \
        do {                                            \
          STATE_SKIP(STR, NOW, INDEX, SIZE, 1);         \
        } while(strchr(SET, NOW));                      \

/* This macro advances the current character as long as it is NOT in SET. */
#define STATE_SKIP_CSET(STR, NOW, INDEX, SIZE, SET)     \
        do {                                            \
          STATE_SKIP(STR, NOW, INDEX, SIZE, 1);         \
        } while(!strchr(SET, NOW));                     \


/* This macro switches the state, advancing the current character as well */
#define STATE_NEXT(STATE_NAME, STR, NOW, INDEX, SIZE)   \
        do {                                            \
          STATE_SKIP(STR, NOW, INDEX, SIZE, 1);         \
          goto STATE_NAME;                              \
        } while (0);

/* Parses an XML string into a BXML stree struct. 
Return NULL on failure. Uses a goto-based state switcher. */
BXML * bxml_parse_strn(char * str, size_t size) {
  BXML * root; /* root node  */
  BXML * node; /* active node */
  int  index = 0; /* index of active character character */
  char now   = str[0];   /* active character */
  int aid;    /* aid count for various uses. */
  char * start;/* aid string for begin of string uses */
  char * stop; /* aid string for end of string uses */

  root = bxml_new(BXML_TAG, NULL);
  node = root;
  
  goto state_start;
  /* begin of states */
  state_start :
    /* skip whitespace, and then we should get a < to open the first tag or 
     <?xml declaration */
    STATE_SKIP_SET(str, now, index, size, BXML_SPACE_STR);
    /* Must get < now. */
    if(now != '<') return NULL;
    /* on to tag parsing state */
    STATE_NEXT(state_tag, str, now, index, size);
    
  state_tag: /* tag parsing state, must already have pased beginning < */
    node = bxml_new(BXML_TAG, NULL);  /* new node for the current tag. */
    
    STATE_SKIP_SET(str, now, index, size, BXML_SPACE_STR);
    /* skip spaces before name. */
    start = str + index; /* name of node certainly starts here. */
    stop  = strpbrk(start, " \t\n\r\"/>"); 
    
    stop  = strpbrk(start, " \t\n\r\"/>"); 
    /* name of node ends on space or / of >. MUST be found. */
    if(!stop) { return bxml_free(node); } 
    /* copy in name of node. */
    bcatblk(node->name, start, stop - start);
    /* XXX: todo... :p */
  
  return NULL;
}


BXML * bxml_parse_str(char * str) {
  return bxml_parse_strn(str, strlen(str));
}






