/* a Tiny wxml parser. Yes, I have to reinvent the wheel again,
but mini xml didn't work as expected! :p */
#include <ctype.h>
#include "dynar.h"
#include "xml.h"
#include "mem.h"
#include "str.h"

/** an attribute of an XML element. */
struct exmlattr_ {
  exmlattr     * before;
  exmlattr     * after;
  STR          * name;
  STR          * value;
};

/** An XML node or document. */
struct exml_ {
  exml      * parent;
  STR       * name;
  STR       * text;
  exml      * before;
  exml      * after;
  exml      * child;
  exmlattr  * attributes;
  int         refs;
};

exmlattr *
exmlattr_init(exmlattr * self, const char * name, const char * value,
              exmlattr * after, exmlattr * before) {
  if(!self) return NULL;
  self->name   = str_new(name);
  self->value  = str_new(value);
  self->after  = after;
  self->before = before;
  return self;
}

exmlattr * exmlattr_new(const char * name, const char * value,
              exmlattr * after, exmlattr * before) {
  exmlattr * self = STRUCT_ALLOC(exmlattr);
  return exmlattr_init(self, name, value, after, before);
}

exmlattr * exmlattr_done(exmlattr * self) {
  if(!self) return NULL;
  str_free(self->name);
  str_free(self->value);
  return self;
}


exmlattr * exmlattr_free(exmlattr * self) {
  if(!exmlattr_done(self)) return NULL;
  STRUCT_FREE(self);
  return NULL;
}


exml *
exml_init(exml * self, exml * parent, const char * name, const char * text,
          exml * after, exml * before) {
  if(!self) return NULL;
  self->name   = str_new(name);
  self->text   = str_new(text);
  self->after  = after;
  self->before = before;
  self->child  = NULL;
  self->refs   = 1;
  return self;
}

exml * exml_done(exml * self) {
  if(!self) return NULL;
  str_free(self->name);
  str_free(self->text);
  return self;
}

exml * exml_free(exml * self) {
  if(!exml_done(self)) return NULL;
  STRUCT_FREE(self);
  return NULL;
}

exml * exml_new(exml * parent, const char * name, const char * text,
                exml * after, exml * before) {
  exml * self = STRUCT_ALLOC(exml);
  return exml_init(self, parent, name, text, after, before);
}

exml * exml_ref(exml * self) {
  self->refs++;
  return self;
}

exml * exml_toss(exml * self) {
  self->refs--;
  if(self->refs == 0 ) {
    return exml_free(self);
  }
  return self;
}

typedef int (exml_getc_func)(void * source);

#define LEX_EOF       -2
#define LEX_NOTFOUND  -1
#define LEX_OK(TOK)   ((TOK)>=0)
#define LEX_FAIL(TOK) (TOK<0)

struct exmlparser_ {
  exml_getc_func * get;
  void * source;
  int line;
  int col;
  int last;
  int error;
};

typedef struct exmlparser_ exmlparser;

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

static char * XML_LEX_BLANK = " \n\t\r\v";

static int exml_lex_findsb(exmlparser * parser, const int wanted) {
  return exml_lex_find(parser, wanted, XML_LEX_BLANK);
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

