
#include "bxml.h"


#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Simplistic, non-conforming XML parser that parses XML documents into 
structs. Has  special handling for tags that have an encoding attribute
*/

/**
* Kind of the self. Can be : ATTR, TEXT, TAG, IDATA.
* The latter is for a 2D array of data, for use with the tilemap loading. 
* Comments, entities and parsing instructions are ignored for now
*/
enum BXMLKind_
{
  BXML_TAG      = 1,
  BXML_TEXT     = 2,
  BXML_ATTR     = 3,
  BXML_IDATA    = 4,
  BXML_COMMENT  = 5,
  BXML_DECLARE  = 6,
  BXML_LASTKIND = 7
};

/**
* An BXML represents one <tag></tag> self and it's children and attributes,
* OR the attributes of a tag.
*/
struct BXML_
{
  BXMLKind   kind; /* kind of tag it is. */
  BXML     * parent, * next, * before, * child;
  BXML     * attr;
  /* Different linked selfs: parent is the parent, next is the next sbling, 
  previous is the previous sibling, child is the first child,
  attribute is the first attribute. */
  USTR     * name; 
  /* Name of the tag or attribute. */
  USTR     * value; /* For text nodes, this contains the text. 
  For attribute nodes this contains the value of the attribute.
  */
  /* Extra data, depending on the tag kind, etc. */
  void     * data;
};


/** Initializes a BXML self */
BXML * bxml_init(BXML * self, int kind, BXML * parent) {
  if(!self) return NULL;
  self->parent = parent;
  self->next   = NULL;
  self->before = NULL;
  self->child  = NULL;
  self->attr   = NULL;
  self->kind   = kind;
  self->name   = ustr_new("");
  self->value  = ustr_new("");
  self->data   = NULL;
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
  ustr_free(self->name);
  ustr_free(self->value);
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



/**
* BXMLParser is the parser interface.
*/ 
struct BXMLParse_ {
  INTERFACE_BODY();
  int (*now) (struct BXMLParser_ * iface);
  int (*pos) (struct BXMLParser_ * iface);
  int (*pos_)(struct BXMLParser_ * iface, int pos);
};

/**
* Results of a parse. Negative states indicate errors.
*/
enum BXMLResult_
{    
  BXML_FOUND_TAG     =  BXML_TAG, /* A tag was found. */
  BXML_FOUND_ATTR    =  BXML_ATTR, /* An attribute was found. */
  BXML_FOUND_TEXT    =  BXML_TEXT,
  BXML_FOUND_IDATA   =  BXML_IDATA,
  BXML_FOUND_COMMENT =  BXML_COMMENT,
  BXML_FOUND_DECLARE =  BXML_DECLARE,
  /* A text node was found. */
  BXML_OK            =  BXML_LASTKIND + 1, 
  /* Parse went on OK. More input is needed. Nothing returned. */
  BXML_DONE          =  BXML_LASTKIND + 1, 
  /* Parse finished sucessfully. */
  BXML_ERROR       = -1,
  BXML_STACKERROR  = -2,
};

/** State of the parser. Negative states indicate errors. */
enum BXMLState_ {
  BXML_STATE_ERROR      = -1,
  BXML_STATE_STACKERROR = -2,
  BXML_STATE_START      = 0,
  BXML_STATE_TAG        = BXML_TAG,
  BXML_STATE_ATTRIBUTE  = BXML_ATTR,
  BXML_STATE_TEXT       = BXML_TEXT,
  BXML_STATE_IDATA      = BXML_IDATA,
  BXML_STATE_ENTITY     = BXML_LASTKIND + 2,
  BXML_STATE_COMMENT    = BXML_LASTKIND + 3,
  BXML_STATE_DECLARE    = BXML_LASTKIND + 4,
  BXML_STATE_TAGSTART   = BXML_LASTKIND + 5,  
  BXML_STATE_TAGEND     = BXML_LASTKIND + 6,
  BXML_STATE_ATTRLIST   = BXML_LASTKIND + 7,  
  BXML_STATE_ATTRSTART  = BXML_LASTKIND + 8,
  BXML_STATE_ATTRVALUE  = BXML_LASTKIND + 9,  
  BXML_STATE_ATTREND    = BXML_LASTKIND + 10,  
};

#define BXMLPARSER_STACKSIZE 1024

/**
* BXMLParse is the parser object. The parser works on a character to character 
* basis.
*/
struct BXMLParser_ {
  int         line;
  int         col;
  int         lastchar;
  int         nowchar;
  int         stack[BXMLPARSER_STACKSIZE];
  int         sp;
  BXML     *  tag;
};

/* Pushes a state on the parser's state stack. Returns state if 
OK, negative on error. */
int bxmlparser_push(BXMLParser * self, BXMLState state) {
  self->sp++;
  // stack overflow
  if( self->sp >= BXMLPARSER_STACKSIZE) {
    return BXML_STATE_STACKERROR;
  }
  self->stack[self->sp] = state;
  return state;
}

/* Returns the state of the top of the stack. 
Assumes the stack pointer is safe.
*/
BXMLState bxmlparser_peek(BXMLParser * self) {
  return self->stack[self->sp];
}


/* Pops a state from the parser's state stack. Returns negative if
an error (e.g. underflow) occurs. Returns the new stack top otherwise. */
BXMLState bxmlparser_pop(BXMLParser * self) {
  self->sp--;
  // Stack underflow.
  if(self->sp < 0) {
    return BXML_STATE_STACKERROR;
  }
  return self->stack[self->sp];
}

/* Sets the top of the state stack to astate and returns the NEW top */
BXMLState bxmlparser_put(BXMLParser * self, BXMLState state) {
  BXMLState old = self->stack[self->sp];
  return self->stack[self->sp] = state;
  return old;
}


/* Stack and parsing helper macros. */
#define PUSH(P, S) bxmlparser_push(P, S)
#define POP(P) bxmlparser_pop(P)
#define PEEK(P) bxmlparser_peek(P)
#define PUT(P, S) bxmlparser_put(P, S)

/* Store SELF->tag in R, and return S */
#define PRET(SELF, R, S) do { (*(R)) = (SELF)->tag ; return (S); } while(0)   


/* Initializes an BXML parser. */
BXMLParser * 
bxmlparser_init(BXMLParser * self) {
  if(!self) return NULL;
  self->line      = 1;
  self->col       = 1;
  self->lastchar  = -1;
  self->nowchar   = -1;
  self->sp        = -1;
  PUSH(self, BXML_STATE_START);
  self->tag       = NULL;
  return self;
}


/* Parses when in the start state.  */
BXMLResult 
bxmlparser_parse_start(BXMLParser * self, int ch, BXML ** result) {
  /* We should get a < first  */
  if (ch == '<') { 
    /* go on to parse the tag, preparing it for use. */    
    PUSH(self, BXML_STATE_TAG); 
    self->tag = bxml_new(BXML_TAG, self->tag);
    PRET(self, result, BXML_OK);
  } else if (isblank(ch)) { 
  /* normally we should get a < character right away,
  but be lenient and skip spaces. So do nothing. */    
    PRET(self, result, BXML_OK);
  } 
  /* Otherwise it's a parse error. */
  PRET(self, result, BXML_ERROR);
}

/* Parses when in the tag start state. This is the beginning of the   
tag name, to deal with comment and <? declarations. */
BXMLResult 
bxmlparser_parse_tagstart(BXMLParser * self, int ch, BXML ** result) {
  /*    
  * If there's a ? at the beginning, it's a declaration. 
  * If there is a ! at the start it should a comment
    ( may not be so if it's a <!ENTITY>)  
  * If it's alphabetical, it's OK, the tag name begins.
  * Anything else is an error.
  */
  if (ch == '?')       {
      PUSH(self, BXML_STATE_DECLARE);
      PRET(self, result, BXML_OK);
  } else if(ch == '!') {
      PUSH(self, BXML_STATE_COMMENT);
      PRET(self, result, BXML_OK);      
  } else if(isalpha(ch)) {
      ustr_appendch(self->tag->name, ch);
      SWAP(self, BXML_STATE_TAG);
      PRET(self, result, BXML_OK);
  } else {
      /* Anything else is a parse error at the start of the tag name.  */
      PUSH(self, BXML_STATE_ERROR);
      PRET(self, result, BXML_ERROR);
  }    
  /* If we get here it's inside of the tag */
}

/* Parses when in the tag state. This is the rest of the the tag name. */
BXMLResult 
bxmlparser_parse_tag(BXMLParser * self, int ch, BXML ** result) {
/*
  Possible cases:
  * If there is a > the tag is done, and we go on to text parsing mode
    for the possible contents of the tag.
  * If there is a / the tag SHOULD be ending,
  * If there are alphanumerical characters, continue with the tag name.
  * If there is whitespace, attribute list begins. 
*/
  if(isalnum(ch)) {
    ustr_appendch(self->tag->name, ch);
    PRET(self, result, BXML_OK);
  } else if (isspace(ch)) {
    SWAP(self, BXML_STATE_ATTRIBUTE);
    PRET(self, result, BXML_OK);
  } else if (ch == '/') {
    SWAP(SELF, BXML_STATE_TAGEND);
    PRET(self, result, BXML_OK);
  } else {
    /* Anything else is a parse error  */
    PUSH(self, BXML_STATE_ERROR);
    PRET(self, result, BXML_ERROR);
  }

}


BXMLResult 
bxmlparser_parse_attribute (BXMLParser * self, int ch, BXML ** result) {
  PRET(self, result, BXML_ERROR);
}

BXMLResult 
bxmlparser_parse_text (BXMLParser * self, int ch, BXML ** result) {
  PRET(self, result, BXML_ERROR);
}

BXMLResult 
bxmlparser_parse_entity (BXMLParser * self, int ch, BXML ** result) {
  PRET(self, result, BXML_ERROR);
}

BXMLResult 
bxmlparser_parse_comment (BXMLParser * self, int ch, BXML ** result) {
  PRET(self, result, BXML_ERROR);
}


BXMLResult 
bxmlparser_parse_declare (BXMLParser * self, int ch, BXML ** result) {
  PRET(self, result, BXML_ERROR);
}

/** Makes the parser accept a single character. 
The tag or attribute that is currently being parsed, if available, 
is stored in result. Otherwise NULL is stored. Negative values indicate a 
parse or parser error. */
BXMLResult 
bxmlparser_parse_core(BXMLParser * self, int ch, 
                      BXML ** result) {
  BXMLState state;
  if(!self) return BXML_ERROR;
  self->lastchar = self->nowchar;
  self->nowchar  = ch;
  // Advance line and column of the parser as needed. 
  self->col++;
  if (ch == '\n') {
    self->line++; self->col = 1;
  }
  // Handle the state.
  state          = PEEK(self);
  switch(state) {
    case BXML_STATE_START:
      return bxmlparser_parse_start(self, ch, result);      
    case BXML_STATE_TAGSTART:
      return bxmlparser_parse_tagstart(self, ch, result);
    case BXML_STATE_TAG:
      return bxmlparser_parse_tag(self, ch, result);
    case BXML_STATE_ATTRIBUTE:
      return bxmlparser_parse_attribute(self, ch, result);
    case BXML_STATE_TEXT:
      return bxmlparser_parse_text(self, ch, result);
    case BXML_STATE_ENTITY:
      return bxmlparser_parse_entity(self, ch, result);
    case BXML_STATE_COMMENT:
      return bxmlparser_parse_comment(self, ch, result);
    case BXML_STATE_DECLARE:
      return bxmlparser_parse_declare(self, ch, result);
    default: 
      PRET(self, result, BXML_ERROR);    
  }
}  








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
    //  ustr_nappend(node->name, start, stop - start); doesn't exist yet...
    /* XXX: todo... :p */
  
  return NULL;
}


BXML * bxml_parse_str(char * str) {
  return bxml_parse_strn(str, strlen(str));
}


BXML * bxml_readfile(char * filename) {
  return NULL;
}



