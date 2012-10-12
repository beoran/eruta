
#include "bxml.h"


#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Simplistic, non-conforming XML parser that parses XML documents into 
structs.
*/

/**
* Kind of the self. Can be : ATTR, TEXT, TAG.
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


/** Initializes a BXML self. 
Parent will have self set as it's child if it's not NULL.  */
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
  if(parent) { parent->child = child; }
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

/** Cleans up a bxml  node, freeing the children recursively. */
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

/** Sets node siling to be the sibling node of this node, possibly inserting 
* between an existing sibling.
* returns other if OK, NULL on error.
**/
BXML * bxml_sibling(BXML * self, BXML * other) {
  BXML * oldnext;
  if(!self)  return NULL;
  if(!other) return NULL;
  oldnext = self->next;
  /* Ensure proper insersion. */
  if(oldnext) {
    oldnext->before  = other; 
  }
  other->next      = oldnext;
  other->before    = self;
  self->next       = other;
  return other;
}

/** Adds attr as an attribute to the BXML tag self.  
* attributes are added in REVERSE order, that is, self->attr will 
* point to the last attrbute added. Returns attr if ok, NULL on error.
*/
BXML * bxml_addattribute(BXML * self, BXML * attr) {
  BXML * oldnext;
  if(!self)  return NULL;
  if(!other) return NULL;
  oldnext = self->attr;
  /* Ensure proper insersion. */
  if(oldnext) {
    oldnext->before  = attr; 
  }
  attr->next       = oldnext;
  attr->before     = NULL;
  self->attr       = attr;
  attr->parent     = self;
  return self;
}


/* According to the XML standard, these are spaces */
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


/** State of the parser. Negative states indicate errors. 
* State zero means the parser is done. States greater than to 
* BXML_FOUND indicate that the parser has found the corresponding tag.
*/
enum BXMLState_ {
  BXML_STATE_MEMERROR   = -3,
  BXML_STATE_STACKERROR = -2,
  BXML_STATE_ERROR      = -1,
  BXML_STATE_DONE       = 0,
  BXML_STATE_START      = 1,
  BXML_STATE_TAGSTART   = 2,
  BXML_STATE_TAGNAME    = 3,
  BXML_STATE_ATTRLIST   = 4,
  BXML_STATE_ATTRSTART  = 5,
  BXML_STATE_ATTRNAME   = 6,
  BXML_STATE_VALSTART   = 7,
  BXML_STATE_SQVALUE    = 8,
  BXML_STATE_DQVALUE    = 9,
  BXML_STATE_TAGEND     = 10,
  BXML_STATE_TEXT       = 11,
  BXML_STATE_VALENTITY  = 12,
  BXML_STATE_TEXTENTITY = 13,
  BXML_STATE_COMMENT    = 14,
  BXML_STATE_DECLARE    = 15,
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
  BXMLState   stack[BXMLPARSER_STACKSIZE];
  int         sp;
  BXML     *  tag;
  BXML     *  root;
  USTR     *  buffer;
};

/* Pushes a state on the parser's state stack. Returns state if 
OK, negative on error. */
BXMLState bxmlparser_push(BXMLParser * self, BXMLState state) {
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
  // Push a done state on the stack. 
  // If the parser reaches this, parsing is done. 
  PUSH(self, BXML_STATE_DONE);
  // Then push a start state.
  PUSH(self, BXML_STATE_START);
  self->tag       = NULL;
  self->root      = NULL;
  self->buffer    = ustr_new("");
  return self;
}


/* Parses when in the start state.  */
BXMLState
bxmlparser_parse_start(BXMLParser * self, int ch) {
  /* We should get a < first  */
  if (ch == '<') { 
    /* Go on to parse the tag, preparing it for use. */
    BXML * newtag = bxml_new(BXML_TAG, self->tag);
    if(!newtag) { 
      return PUSH(self, BXML_STATE_MEMERROR);
    }
    /* Initialize root tag if not set yet. */
    if(!self->root) {
      self->root = newtag; 
    }
    self->tag = newtag;
    return PUSH(self, BXML_STATE_TAGSTART); 
  } else if (isblank(ch)) { 
  /* Normally we should get a < character right away,
  but be lenient and skip spaces. So do nothing. */
    return PEEK(self);
  } 
  /* Otherwise it's a parse error. */
  return PUSH(self, BXML_STATE_ERROR);
}

/* Parses when in the tag start state. This is the beginning of the   
tag name, to deal with comment and <? declarations. */
BXMLState
bxmlparser_parse_tagstart(BXMLParser * self, int ch) {
  /*    
  * If there's a ? at the beginning, it's a declaration. 
  * If there is a ! at the start it should a comment
    ( may not be so if it's a <!ENTITY>)  
  * If it's alphabetical, it's OK, the tag name begins.
  * Anything else is an error.
  */
  if (ch == '?')       {
      return PUSH(self, BXML_STATE_DECLARE);
  } else if(ch == '!') {
      return PUSH(self, BXML_STATE_COMMENT);
  } else if(isalpha(ch)) {
      ustr_appendch(self->tag->name, ch);
      return PUT(self, BXML_STATE_TAG);
  } else {
      /* Anything else is a parse error at the start of the tag name.  */
      return PUSH(self, BXML_STATE_ERROR);
  }
  /* If we get here it's inside of the tag */
}

/* Parses when in the tag name state. This is the rest of the the tag name. */
BXMLState
bxmlparser_parse_tagname(BXMLParser * self, int ch) {
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
    return PEEK(state);
  } else if (isspace(ch)) {
    return SWAP(self, BXML_STATE_ATTRLIST);
  } else if (ch == '/') {
    return SWAP(SELF, BXML_STATE_TAGEND);
  } else if (ch == '>') {
    /* a > after the tag name (without having seen a /, 
      means that a text node wil follow as the child of the current node. 
      Prepare that text node.
    */
    BXML * newtag = bxml_new(BXML_TAG, self->tag);
    if(!newtag) { 
      return PUSH(self, BXML_STATE_MEMERROR);
    }
    self->tag = newtag;
    return SWAP(SELF, BXML_STATE_TEXT);
  } else {
    /* Anything else is a parse error  */
    return PUSH(self, BXML_STATE_ERROR);
  }

}


/* Parses an attribute list */
BXMLState
bxmlparser_parse_attrlist(BXMLParser * self, int ch) {
  return PUSH(self, BXML_STATE_ERROR);
  /*
  * Possible cases: 
  * * if there is a / the tag should be ending
  * * if there is a  > the tag is done
  * * if there is whitespace, skip it
  * * if there is an alphabetical character, it's the start of an attribute
  */
  if(isalpha(ch)) {
    /* we have a new attribute. Create it and set it as the attribute.
     of the current tag.  */
    BXML * attr = bxml_new(self->tag);
    if(!bxml_addattribute(self->tag, attr)) {
      return return PUSH(self, BXML_STATE_MEMERROR);
    }
    ustr_appendch(self->attr->name, ch);
    return PUSH(state, BXML_STATE_ATTRNAME);
  } else if (isspace(ch)) {
    /* skip spaces. */
    return PEEK(self);
  } else if (ch == '/') {
    return SWAP(self, BXML_STATE_TAGEND);
  } else if (ch == '>') {
    /* a > after the tag name (without having seen a /, 
      means that a text node wil follow as the child of the current node. 
      Prepare that text node.
    */
    BXML * newtag = bxml_new(BXML_TAG, self->tag);
    if(!newtag) { 
      return PUSH(self, BXML_STATE_MEMERROR);
    }
    self->tag = newtag;
    return SWAP(SELF, BXML_STATE_TEXT);
  } else {
    /* Anything else is a parse error  */
    return PUSH(self, BXML_STATE_ERROR);
  }
}

BXMLState
bxmlparser_parse_attrname(BXMLParser * self, int ch) {
  /* an attibute cname continues until we find a isspace or an = character
  *  anything else isn't allowed.
  */
  if(isalnum(ch)) {
    ustr_appendch(self->attr->name, ch);
    return PEEK(state);
  } else if(isspace(ch)) { 
    /* atttribute name is done, but we still need the = */
    return SWAP(state, BXML_STATE_NAMEEND);
  } else if(ch == '=')
    /* value will start from here */
    return SWAP(state, BXML_STATE_VALSTART);
  } else {
    return PUSH(self, BXML_STATE_ERROR);
  }
}


BXMLState
bxmlparser_parse_nameend(BXMLParser * self, int ch) {
  /* attribute has endedn, but we still need to skip spaes and look for the =
  */
  if(isspace(ch)) { 
    /* atttribute name is done, but we still need the = */
    return PEEK(state);
  } else if(ch == '=')
    /* value will start from here */
    return SWAP(state, BXML_STATE_VALSTART);
  } else {
    return PUSH(self, BXML_STATE_ERROR);
  }
}

BXMLState
bxmlparser_parse_valstart(BXMLParser * self) {
  /* we have had the = , now skip spaces until we get the " or '
  */
  if(isspace(ch)) { 
    /* We saw the =, but we still need the " */
    return PEEK(state);
  } else if((ch == '"') {
    /* Double quoted value will start from here. Store the quote too,  */
    ustr_appendch(self->attr->value, ch);
    return SWAP(state, BXML_STATE_DQVALUE);
  } else if((ch == '\'') {
    /* Single quoted value will start from here. Store the quote too,  */
    ustr_appendch(self->attr->value, ch);
    return SWAP(state, BXML_STATE_DQVALUE);
  } else {
    return PUSH(self, BXML_STATE_ERROR);
  }
}

/* parses a single or qouble quoted value as per quote. */
BXMLState
bxmlparser_parse_value(BXMLParser * self, int ch, int quote) {
  /** copy data until we get the quote, but beware entities */
  if(ch == '&') { 
    /* Entity! */
    ustr_empty(state->buffer);
    return PUSH(state, BXML_STATE_VALENTITY);
  } else if (ch == quote) {
    /** atribute is all done. POP! */    
    return POP(state);
  } else {
    /* Just store the value. */
    ustr_appendch(self->attr->value, ch);
  }
}


/* Use as silut table to look up the character of the given entities. */
static Silut bxml_entity_lut[] = {
  { '"' , "quot"   },
  { '&' , "amp"    },
  { '\'', "apos"   },
  { '<' , "lt"     },
  { '>' , "gt"     },
  { -1  , NULL     }
};


/* Parses an entity. Uses target to append a character to in case the
 entity was found.  */
BXMLState
bxmlparser_parse_entity(BXMLParser * self, int ch, USTR * target) {
  /** copy data until we get the quote, but beware entities */
  if(ch == ';') { 
    /* Entity is done */
    Silut match = silut_lsearchcstr(bxml_entity_lut, ustr_c(state->buffer));
    if(match) {
      ustr_appendch(target, match.integer);
    } else  { 
      int base = 10;
      int res  = 0;
      char * aid = ustr_c(self->buffer);
      if(aid[0] != '#') { 
        /* must be a # entity if not known as a named one */
        return PUSH(state, BXML_STATE_ERROR);
      }
      aid++;
      if(aid[0] == 'x') { aid++; base = 16; }
      res = strtol(aid, NULL, base);
      /* XXX: wild ignore strtol failing :p */
      ustr_appendch(target, res);
    }
    ustr_empty(state->buffer);
    /* Return to previous state. */
    return POP(state);
  } else {
    /* Just store the value in the sschratch buffer . */
    ustr_appendch(self->buffer, ch);
  }
}



BXMLState
bxmlparser_parse_text (BXMLParser * self, int ch, BXML ** result) {
  return PUSH(self, BXML_STATE_ERROR);
}

BXMLResult 
bxmlparser_parse_entity (BXMLParser * self, int ch, BXML ** result) {
  return PUSH(self, BXML_STATE_ERROR);
}

BXMLResult 
bxmlparser_parse_comment (BXMLParser * self, int ch, BXML ** result) {
  return PUSH(self, BXML_STATE_ERROR);
}


BXMLResult 
bxmlparser_parse_declare (BXMLParser * self, int ch, BXML ** result) {
  return PUSH(self, BXML_STATE_ERROR);
}


BXMLResult bxmlparser_parse_dispatch(BXMLParser * self, int ch) { 
  // Handle the state.
  state          = PEEK(self);
  switch(state) {
    case BXML_STATE_START:
      return bxmlparser_parse_start(self, ch, result);
    case BXML_STATE_TAGSTART:
      return bxmlparser_parse_tagstart(self, ch, result);
    case BXML_STATE_TAGNAME:
      return bxmlparser_parse_tagname(self, ch, result);
    case BXML_STATE_ATTRLIST:
      return bxmlparser_parse_attrlist(self, ch, result);
    case BXML_STATE_ATTRSTART:
      return bxmlparser_parse_attrstart(self, ch, result);
    case BXML_STATE_ATTRNAME:
      return bxmlparser_parse_attrname(self, ch, result);
    case BXML_STATE_VALSTART:
      return bxmlparser_parse_valstart(self, ch, result);
    case BXML_STATE_VALUE:
      return bxmlparser_parse_value(self, ch, result);
    case BXML_STATE_TAGEND:
      return bxmlparser_parse_tagend(self, ch, result);
    case BXML_STATE_TEXT:
      return bxmlparser_parse_text(self, ch, result);
    case BXML_STATE_ENTITY:
      return bxmlparser_parse_entity(self, ch, result);
    case BXML_STATE_COMMENT:
      return bxmlparser_parse_comment(self, ch, result);
    case BXML_STATE_DECLARE:
      return bxmlparser_parse_declare(self, ch, result);
    default: 
      return SET(self, BXML_ERROR);
  }
}

/** Makes the parser accept a single character. 
The tag or attribute that is currently being parsed, if available, 
is stored in result. Otherwise NULL is stored. Negative values indicate a 
parse or parser error. */
BXMLResult 
bxmlparser_parse_core(BXMLParser * self, int ch, 
                      BXML ** result) {
  BXMLState newstate;
  if(!self) return BXML_ERROR;
  self->lastchar = self->nowchar;
  self->nowchar  = ch;
  // Advance line and column of the parser as needed. 
  self->col++;
  if (ch == '\n') {
    self->line++; self->col = 1;
  }
  newstate = bxmlparser_parse_dispatch(self, ch);
  if((newstate >= BXML_PARSE_OK) && (result)) {
    (*reult) = self->tag;
  }
  return result;
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



