/* a Tiny wxml parser. Yes, I have to reinvent the wheel again,
but mini xml didn't work as expected! :p */
#include <ctype.h>
#include "dynar.h"
#include "xml.h"
#include "mem.h"
#include "str.h"


/** An XML node or document.
* The node has a pointer to a linked list of it's attributes, a pointer to
* the linked list of it's children , and pointer to it's next sibling on the
* same level
**/
struct exml_ {
  STR       * name;
  STR       * value;
  exml      * parent;
  exml      * after;
  exml      * child;
  exml      * attrs;
  int         refs;
};

exml *
exml_init(exml * self, const char * name, const char * value) {
  if(!self) return NULL;
  self->name   = str_new(name);
  self->value  = str_new(value);
  self->after  = NULL;
  self->child  = NULL;
  self->attrs  = NULL;
  self->refs   = 1;
  return self;
}

exml * exml_free(exml * self);

exml * exml_done(exml * self) {
  exml * attr;
  if(!self) return NULL;
  str_free(self->name);
  str_free(self->value);
  // recursively free attributes
  exml_free(self->attrs);
  self->attrs = NULL;
  // recursively free children
  exml_free(self->child);
  self->child = NULL;
  // recursively free siblings
  exml_free(self->after);
  self->after = NULL;
  return self;
}

exml * exml_free(exml * self) {
  if(!exml_done(self)) return NULL;
  STRUCT_FREE(self);
  return NULL;
}

exml * exml_new(const char * name, const char * text) {
  exml * self = STRUCT_ALLOC(exml);
  return exml_init(self, name, text);
}

exml * exml_ref(exml * self) {
  self->refs++;
  return self;
}

exml * exml_toss(exml * self) {
  self->refs--;
  if(self->refs == 0 ) {
    self->refs = -1;
    return exml_free(self);
  }
  return self;
}

/**
* Adds a new attribute to the xml node, appended to the attribute * last,
* and returns it.
*/
exml * exml_newattr(exml * self, exml * last, char * name, char * value) {
  exmlattr * attr = exml_new(name, value);
  if(!attr) return NULL;
  if(!last) {
    // set new attribute as as first attribute
    self->attrs = attr;
  } else {
    // link attribute
    last->after = attr;
  }    
  return attr;
}

/** Adds sibling as a sibling node of self. */
exml * exml_addafter(exml * self, exml * sibling) {
  if(!self) return NULL;
  if(self->after) return exml_addafter(self->after, sibling);
  self->after = sibling;
  return sibling;  
}

/** Adds child as a child node of self. */
exml * exml_addchild(exml * self, exml * child) {
  if(!self) return NULL;
  if(self->child) return exml_addafter(self->child, child);
  self->child = child;
  return child;
}


typedef int (exml_getc_func)(void * source);


#define LEX_EOF       -2
#define LEX_NOTFOUND  -1
#define LEX_OK(TOK)   ((TOK)>=0)
#define LEX_FAIL(TOK) (TOK<0)

enum exml_state_ { 
  EXML_STATE_ERROR        = -3,
  EXML_STATE_EOF          = -2,
  EXML_STATE_STOP         = -1,
  EXML_STATE_START        = 0,
  EXML_STATE_TAG          = 1,
  EXML_STATE_ATTR_NAME    = 2,
  EXML_STATE_ATTR_BETWEEN = 3,
  EXML_STATE_ATTR_VALUE   = 4,
  EXML_STATE_COMMENT      = 5,
  EXML_STATE_ENTITY       = 6,
  // <?xml ... >
  EXML_STATE_DECLARE      = 7,
  EXML_STATE_TEXT         = 8,
  // for &amp; etc.
  EXML_STATE_TAG_END      = 9,
  EXML_STATE_DECLARE_END  = 10,
  EXML_STATE_COMMENT_END  = 11,
  EXML_STATE_COMMENT_END2 = 12,
  EXML_STATE_ANY          = 13,
  
  // EXML_STATE_SIZE         = 13,
};

typedef enum exml_state_ exml_state;

struct exmlparser_;
typedef struct exmlparser_ exmlparser;

typedef int (exml_state_func)(exmlparser * parser, int ch);

struct exmlparser_ {
  exml_getc_func  * get;
  exml_state_func * statefunc[EXML_STATE_SIZE];
  exml            * root;
  exml            * now; // last added node. 
  exml            * last_attr; // last added attribute
  void            * source;
  int line;
  int col;
  int last;
  int error;
  exml_state state;
  char            * storage;
  // storage for characters parsed  

};

enum exml_action_ {
  EXML_ACTION_NONE,
  EXML_ACTION_STORE,
  EXML_ACTION_TAGNAME,
  EXML_ACTION_ATTRNAME,
  EXML_ACTION_ATTRVALUE,
  EXML_ACTION_TEXT,
  EXML_ACTION_ENTITY,
};

typedef enum exml_action_ exml_action;

struct exmlrule_;
typedef struct exmlrule_ exmlrule;

struct exmlrule_ {
  exml_state  state;
  char      * input;
  exml_action action;
  exml_state  next;  
};

#define EXML_BLANK " \t\n\r\v"
#define EXML_ANY   NULL
#define EXML_NAME "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-+"

/** The rule table, every rule consists of repectively the state where the rule
applies, the input for which the rule applies as a string of accepted
characters, or NULL as a catch-all wildcard, the action to take for this rule,
and the new state reached after this rule. */
static exmlrule exml_rules[] = {
  // When in start state skip all blanks
  { EXML_STATE_START, EXML_BLANK, EXML_ACTION_NONE    , EXML_STATE_START    },
  // When in start state go to tag state if we get a < character
  { EXML_STATE_START, "<"       , EXML_ACTION_NONE    , EXML_STATE_TAG      },
  // any other character we see in start state is an error.
  { EXML_STATE_START, EXML_ANY  , EXML_ACTION_NONE    , EXML_STATE_ERROR    },

  // When in tag state and there is a ! input, go to comment state
  { EXML_STATE_TAG  , "!"       , EXML_ACTION_NONE    , EXML_STATE_COMMENT  },
  // When in tag state and there is a ? input, go to declare state
  { EXML_STATE_TAG  , "?"       , EXML_ACTION_NONE    , EXML_STATE_DECLARE  },
  // When in tag state, and we see a name, store the name characters. 
  { EXML_STATE_TAG  , EXML_NAME , EXML_ACTION_STORE   , EXML_STATE_TAG      },
  // When in tag state, and we see a blank, the name is complete
  // next, we expect an attribute name
  { EXML_STATE_TAG  , EXML_BLANK , EXML_ACTION_TAGNAME, EXML_STATE_ATTR_NAME},
  // When in comment state

  // Final catch all state
  { EXML_STATE_ANY , EXML_ANY    , EXML_ACTION_NONE, EXML_STATE_ERROR       },
};

int exmlparser_getc(exmlparser * parser) {
  int ch;
  ch = parser->get(parser->source);
  if (ch == '\n') {
    parser->line++; parser->col = 1;
  } else {
    parser->col++;
  }
  parser->last = ch;
  return ch;
}

int exmlparser_getskip(exmlparser * parser, char * skip) {
  int ch = exmlparser_getc(parser);
  while(ch != EOF) {
    if(!isspace(ch)) return ch;
    ch = exmlparser_getc(parser);
  }
  return EOF;
}

STR * exmlparser_getname(exmlparser * parser) {
  STR * result = str_new("");
  int ch = exmlparser_getc(parser);
  while(ch != EOF) {
    if(isalnum(ch) || strchr("_:.", ch)) {
      str_appendch(result, ch);
    } else if (isspace(ch) || strchr("=", ch)) {
      return result;
    } else { // unknown chars not allowed. 
      break;
    }
  }
  str_free(result);
  return NULL;
}

exml* exml_parser_newnode(exmlparser * self) {
  exml * node = exml_new("", "");
  if(!self->root) { self->root = node; }
  return node;
}

exml * exml_parse_tag(exmlparser * self) {
  exml * result = exml_parser_newnode(self);
  // skip any blanks, and see what we get.
  int ch = exmlparser_getskip(self, " \t\n\r\v");
  if(ch == '<') {
    self->now = result;
   // exml_parse_tagname(self);
   // exml_parse_attributes(self);
   // exml_parse_tagname(self);
    
  } else {
    return exml_free(result);
  }
}

exml * exml_parse_start(exmlparser * self) {
  exml * result = NULL;  
  // parse a tag (which may be an <? ?> or a <! --> as well,
  // and may have some blanks before it.
  // result = exmlparser_parse_tag(  
  return result;
}



#ifdef COMMENT_

exml * exmlparser_newtag(exmlparser * self, int child) {
  exml * result = exml_new(self->now, "", "", NULL, NULL);
  if(!result) return NULL;
  if (child)  {
    // child node of now, link them together
    result->parent       = self->now;
    self->now->child     = result; 
  } else {
    // sibling node 
    result->before       = self->now;
    self->now->after     = result;
  }  
  // now result is the active node
  self->now            = result;
  // fill in root if not yet set.
  if(!self->root) self->root = self->now;
  return self->now;  
}

// process characters in the start state
int exml_state_start(exmlparser * self, int ch) {
  // skip blanks. 
  if (strchr(EXML_PARSE_BLANK, ch)) {
    // do nothing on blanks.
  } else if (ch == '<') {
    // new child tag
    exmlparser_newtag(self, TRUE);
    self->state    = EXML_STATE_TAG;
  } else {
    self->state = EXML_STATE_ERROR;
  }
  return self->state;
}

// process characters in the tag state
int exml_state_tag(exmlparser * self, int ch) {
  if (strchr(EXML_PARSE_BLANK, ch)) {
    // on blanks, attribute names begin, so create a new attribute.
    exml_newattr(self->state->now, "", "");
    self->state = EXML_STATE_ATTR_NAME;
  } else if (ch == '!') {
    // comment starts here
    self->state = EXML_STATE_COMMENT;
  } else if (ch == '?') {
    // declaration starts here
    self->state = EXML_STATE_DECLARE;
  } else if (ch == '/') {
    self->state = EXML_STATE_TAG_END;
  } else if (ch == '>') {    
    self->state = EXML_STATE_TEXT;
  } else { // should be tag's name, append it to name. 
    str_appendch(self->now->name, ch);  
  }      
  return self->state;
}

// Process characters in declare state
int exml_state_declare(exmlparser * self, int ch) {
  if(ch=='?') { // near end of declaration? 
    self->state = EXML_STATE_DECLARE_END;
  } else { // just accept anything else, ignoring it
    // do nothing
  }
  return self->state;
}  
  
// Process characters in declare end state
int exml_state_declare_end(exmlparser * self, int ch) {
  if(ch=='>') { // must have an end of tag > here, back to start state.
    self->state = EXML_STATE_START;
  } else { //  anything else is an error
    self->state = EXML_STATE_ERROR;
  }
  return self->state;
}

// Process characters in comment state
int exml_state_comment(exmlparser * self, int ch) {
  if(ch=='-') { // near end of comment
    self->state = EXML_STATE_COMMENT_END;
  } else { // just accept anything else, ignoring it
    // do nothing
  }
  return self->state;
}

// Process characters in comment ending state
int exml_state_comment_end(exmlparser * self, int ch) {
  if(ch=='-') { // near end of comment, second - 
    self->state = EXML_STATE_COMMENT_END2;
  } else { // anything else means the comment is not done yet.
    self->state = EXML_STATE_COMMENT;
  }
  return self->state;
}

// Process characters in comment ending state 2
int exml_state_comment_end2(exmlparser * self, int ch) {
  if(ch=='>') { // end of comment, back to start mode or text mode
    // needd to memoize something to decide which it was? 
    self->state = EXML_STATE_START;
  } else { // anything else means the comment is not done yet.
    self->state = EXML_STATE_COMMENT;
  }
  return self->state;
}


  

// process characters in attribute name state
int exml_state_attr_name(exmlparser * self, int ch) {
  if (strchr(EXML_PARSE_BLANK, ch)) {
    // skip blanks    
  } else if (ch == '/') {
    // these are actually errors, but parse them anyway
    self->state = EXML_STATE_TAG_END;
  } else if (ch == '>') {
    // these are actually errors, but parse them anyway
    self->state = EXML_STATE_TEXT;
  } else if(ch == '=') { 
    // between tag and value
    self->state = EXML_STATE_ATTR_BETWEEN
  else { // should be tag's last attribute's name
    str_appendch(self->now->last_attr->name, ch);
  }
  return self->state;
}

// process characters in between attribute name and value state
int exml_state_attr_between(exmlparser * self, int ch) {
  if (strchr(EXML_PARSE_BLANK, ch)) {
    // skip blanks
  } else if (ch == '"') {
    // ok, the tag's value has begun.
    self->state = EXML_STATE_TAG_VALUE;        
  } else { // anything else but a " or blanks is an error now. 
    self->state = EXML_STATE_ERROR;
  }
  return self->state;
}

// process characters in attribute value state
int exml_state_attr_value(exmlparser * self, int ch) {
  return 0;
}


#endif




exmlparser * exmlparser_init(exmlparser * parser,
                             exml_getc_func * get, void * source) {
  if(!parser) return NULL;
  parser->get     = get;
  parser->source  = source;
  parser->line    =  1;
  parser->col     =  1;
  parser->last    = -1;
  parser->error   =  0;
  parser->state   = EXML_STATE_START;
  parser->root    = NULL;
  parser->now     = NULL;
  return parser;  
}


int exmlparser_error_(exmlparser * parser, int error) {
  parser->error = error;
  return error;
}

static int exml_lex_find(exmlparser * parser,
                          const int wanted, const char * skip) {
  int ch = exmlparser_getc(parser);
  while(ch != EOF) {
    if (ch == wanted)      return ch;
    // not allowed to skip, so ch was not found
    if (!strchr(skip, ch)) return exmlparser_error_(parser, LEX_NOTFOUND);
    ch = exmlparser_getc(parser);
  }
  return LEX_EOF;
}

static int exml_lex_upto(exmlparser * parser, const char * stop) {
  int ch = exmlparser_getc(parser);
  while(ch != EOF) {
    if (strchr(stop, ch)) return ch;
    ch = exmlparser_getc(parser);
  }
  return exmlparser_error_(parser, LEX_EOF);
}



static int exml_lex_findsb(exmlparser * parser, const int wanted) {
  return exml_lex_find(parser, wanted, " \t\n\r\v");
}

/*

possible parser states: 1) before first tag (skip whitespace)
2) inside tag -> comment, ? or tag name, attributes, end of tag or tag body
3) inside tag body -> collect text and &entities, sub-tag if we find <
4) done 


*/

exml * exml_parse(exmlparser * parser) {
  int ch             ;
  STR * text   = NULL;
  exml * root  = NULL;
  // exml_new(NULL, "xml", NULL, NULL, NULL);
  ch           = exmlparser_getc(parser);
  text         = str_new("");
  // skip white space before first tag, and get the < .
  while(isspace(ch)) {
    ch         = exmlparser_getc(parser);
  }
  if(LEX_FAIL(ch)) {
    exmlparser_error_(parser, LEX_EOF);
    return NULL;
  }
  
  
/*
  switch(ch) {
    case '<': return exml_parse_tag(parser);
    case '&': exml_parse_entity(parser,


  // Find first opening <
  if(LEX_FAIL(exml_lex_noblanks(parser, '<')) return NULL;
*/
  return NULL;



}



#ifdef COMMENT_






#endif

