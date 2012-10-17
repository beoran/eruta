#include "stdio.h"
#include "str.h"
#include "bxml.h"

/** Struct to hold the state of the ragel parser. */
struct BXMLRagel_ {
  BXML * root;
  BXML * top;
  USTR * buffer;
};


/*
A Ragel Parser for XML.


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
  machine bxmlparser;
  action buffer {
    str_appendch(parser->buffer, fpc);
  }

  action tag_open { };
  action tag_done { };
  
  

  name          = ^(space | '/' | '>' | '=')+ >buffer;
  tagopen       = '<';
  fulltagclose  = '>';
  emptytagclose = '/>';
  emptytag      = tagopen name emptytagclose;
  endtag        = '</' name? '>' ;
  fulltag       = tagopen name fulltagclose tagbody endtag ;
  bodytext      = ^[<&] ;
  entity        = '&' alnum ';' ; 
  tagbody       = (bodytext | entity)* ;   
  tag           =  emptytag | fulltag  ; 
  anyspace  = space* ; 
  junk     := comment; 
  main     := prolog? junk* tag junk @{ res = 1; };
}%%

/*
document: prolog element misc*;
prolog: VERSION? ENCODING? misc*;
misc: COMMENT | attribute_decl;
attribute_decl: ATTDEF NAME attribute+ ENDDEF;
element: START attribute* empty_or_content;
empty_or_content: SLASH CLOSE | CLOSE content END NAME? CLOSE;
content: (DATA | misc | element)*;
attribute: NAME (EQ VALUE)?;

nl    (\r\n|\r|\n)
ws    [ \t\r\n]+
open    {nl}?"<"
close   ">"{nl}?
namestart [A-Za-z\200-\377_]
namechar  [A-Za-z\200-\377_0-9.-]
esc   "&#"[0-9]+";"|"&#x"[0-9a-fA-F]+";"
name    {namestart}{namechar}*
data    ([^<\n&]|\n[^<&]|\n{esc}|{esc})+
comment   {open}"!--"([^-]|"-"[^-])*"--"{close}
string    \"([^"&]|{esc})*\"|\'([^'&]|{esc})*\'
version   {open}"?XML-VERSION 1.0?"{close}
encoding  {open}"?XML-ENCODING"{ws}{name}{ws}?"?"{close}
attdef    {open}"?XML-ATT"

%s CONTENT


%%

<INITIAL>{ws}   {  }
<INITIAL>{version}  {return VERSION;}
<INITIAL>{encoding} {yylval.s = word(yytext + 14); return ENCODING;}
<INITIAL>"/"    {return SLASH;}
<INITIAL>"="    {return EQ;}
<INITIAL>{close}  {BEGIN(CONTENT); return CLOSE;}
<INITIAL>{name}   {yylval.s = strdup(yytext); return NAME;}
<INITIAL>{string} {yylval.s = strdup(yytext); return VALUE;}
<INITIAL>"?"{close} {BEGIN(keep); return ENDDEF;}

{attdef}    {keep = YY_START; BEGIN(INITIAL); return ATTDEF;}
{open}{ws}?{name} {BEGIN(INITIAL); yylval.s= word(yytext); return START;}
{open}{ws}?"/"    {BEGIN(INITIAL); return END;}
{comment}   {yylval.s = strdup(yytext); return COMMENT;}

<CONTENT>{data}   {yylval.s = strdup(yytext); return DATA;}

.     {fprintf(stderr, "!ERROR(%c)\n", *yytext);}
{nl}      {/* skip, must be an extra one at EOF */;}

*/

%%write data;

int xmlragel_parse(stuct BXMLRagel_ * parser, char * xmltext, int size) {
  int cs, res = -1;
  if (size < 0) size = strlen(xmltext);
  {
    char * p   = xmltext;
    char * pe  = p + size + 1;
    char * eof = pe;
    /** use %access parser-> later .*/
    %%write init;
    %%write exec;
  }
  printf("Result: %d\n", res);
  return 0;  
}






