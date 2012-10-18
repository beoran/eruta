#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct BXMLNode_ BXMLNode;

enum {
  BXML_NODE_NORMAL  = 1,
  BXML_NODE_TEXT    = 2,
  BXML_NODE_ATTR    = 3,
};

struct BXMLNode_ {
  int kind;
  char * tagname;
  BXMLNode * parent, *child, *sibling, *attribute;
  char * value;
}

BXMLNode * bxmlnode_alloc() {
  return calloc(sizeof(BXMLNode));
}



/** Struct to hold the state of the ragel parser. */
struct BXMLRagel_ {
  BXNLNode * root;
  BXMLNode * now;
  char * buffer;
  int bufp; 
};


/*
a Ragel Parser for XML.


attribute = ^(space | ‘/’ | ‘>’ | ‘=’)+ >buffer %attributeName space* ‘=’ space*
((‘\” ^’\”* >buffer %attribute ‘\”) | (‘”‘ ^’”‘* >buffer %attribute ‘”‘));
element = ‘<' space* ^(space | '/' | '>‘)+ >buffer %elementStart (space+ attribute)*
:>> (space* (‘/’ %elementEndSingle)? space* ‘>’ @element);
elementBody := space* <: ((^'<'+ >buffer %text) <: space*)?
element? :>> (‘<' space* '/' ^'>‘+ ‘>’ @elementEnd);

comment  :=
header   := open '?'
main     := space* element space*;
*/

%%{
  machine bxmlragel;
  action buffer {
    char * newbuf;
    parser->buffer[bufp] = fc;    
    bufp++;
    if (bufp >= bufs) {
      int newbufs = bufs*2 + 2;
      newbuf = calloc(newbufs, 1);
      if(!newbuf) abort();
      memmove(newbuf, buffer, bufs);
      parser->buffer = newbuf;
      bufs   = newbufs;
    }
  }

  action element_open { }
  action element_done { }
  
  

  namechar  = ^(space | '/' | '>' | '=')+ >buffer;
  tagname   = namechar+;
  emptytag  = '<' tagname '/>';
  try       = emptytag;
  main     := try @{ res = 1; };
}%%



%%write data;

int bxmlragel_parse(struct BXMLRagel_ * parser, char * xmltext, int size) {
  char * buffer;
  int bufp = 0; 
  int bufs = 1024;  
  parser->buffer = calloc(bufs, 1);
  int cs, res = -1;
  {
    char * p   = xmltext;
    char * pe  = p + size + 1;
    char * eof = pe;
    /** use %access parser-> later .*/
    %%write init;
    %%write exec;
  }
  printf("Result: %d, %s\n", res, parser->buffer);
  free(buffer);
  return 0;  
}

int main(int argc, char * argv) {
  char * str = "<hello/>";
  struct BXMLRagel_ parser;
  parser.root = NULL;
  parser.now = NULL;
  return bxmlragel_parse(&parser, str, strlen(str));
}




