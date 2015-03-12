/* My own XML parser. For what good it is :p ... */

#include <stdlib.h>
#include <string.h>
#include "ses.h"
#include "bxml.h"
#include "bxmlparser.h"


#define BXML_WS " \t\n\r"
#define BXML_LOWER "abcdefghijklmnopqrstuvwxyz"
#define BXML_UPPER "ABCDEFGHIJKLMNOP"
#define BXML_ALPHA BXML_LOWER BXML_UPPER
#define BXML_NUM   "0123456789"
#define BXML_ALNUM BXML_ALPHA BXML_NUM


/* According to the XML standard, these are spaces */
static const char BXML_SPACE_STR[] = { 0x20,  0x09, 0x0D, 0x0A, 0x00 };


/* State of the parser. Negative states indicate errors. 
* State zero means the parser is done. States greater than to 
* bxml_FOUND indicate that the parser has found the corresponding tag.
*/
enum BxmlState_ {
  bxml_STATE_MEMERROR   = -3,
  bxml_STATE_STACKERROR = -2,
  bxml_STATE_ERROR      = -1,
  bxml_STATE_DONE       = 0,
  bxml_STATE_START      = 1,
  bxml_STATE_TAGSTART   = 2,
  bxml_STATE_TAGNAME    = 3,
  bxml_STATE_ATTRLIST   = 4,
  bxml_STATE_ATTRSTART  = 5,
  bxml_STATE_ATTRNAME   = 6,
  bxml_STATE_VALSTART   = 7,
  bxml_STATE_SQVALUE    = 8,
  bxml_STATE_DQVALUE    = 9,
  bxml_STATE_TAGEND     = 10,
  bxml_STATE_TEXT       = 11,
  bxml_STATE_VALENTITY  = 12,
  bxml_STATE_TEXTENTITY = 13,
  bxml_STATE_COMMENT    = 14,
  bxml_STATE_DECLARE    = 15,
  bxml_STATE_PROCESSING = 16,
  bxml_STATE_CDATA      = 17
};

typedef enum BxmlState_ BxmlState; 

#define BXML_PARSER_STACKSIZE 1024

/*
* BxmlParse is the parser object. For simplicity, the parser works on a string 
* with the whole xml document in memory. Not very efficient, but easier to parse.
*/
struct BxmlParser_ {
  Swis        buffer;
  char      * index;
  int         line;
  int         col;
  int         now;
  int         stack[BXML_PARSER_STACKSIZE];
  int         sp;
  Bxml      * tag;
  Bxml      * root;
  const char* error;
};

BxmlParser * bxmlparser_alloc(void) {
  return calloc(1, sizeof(BxmlParser));
} 


/** Initializes a parser. */
BxmlParser * bxmlparser_init(BxmlParser * me) {
  if (!me) return NULL;
  if (!swis_new_empty(&me->buffer)) return NULL;
  me->index = me->buffer.text;
  me->line  = 0;
  me->col   = 0;
  me->now   = 0;
  me->sp    = 0;
  me->tag   = NULL;
  me->root  = NULL; 
  me->error = NULL; 
  return me;
}

BxmlParser * bxmlparser_new() {
  return bxmlparser_init(bxmlparser_alloc());
}

/** Cleans up a parser after use. */
BxmlParser * bxmlparser_done(BxmlParser * me) {
  if (!me) return NULL;
  swis_free(&me->buffer);
  me->index = NULL;
  me->line  = 0;
  me->col   = 0;
  me->now   = 0;
  me->sp    = 0;
  me->tag   = NULL;
  me->root  = NULL;  
  return me;
}

BxmlParser * bxmlparser_free(BxmlParser * me) {
  bxmlparser_done(me);
  free(me);
  return NULL;
}

/* Adds the tag, either as root if no root is set, or as a child of the 
 * current tag. */
Bxml * bxmlparser_add_tag_size(BxmlParser * me, int kind, int size, ...) {
  Bxml * newtag;
  va_list args;
  va_start(args, size);
  newtag = bxml_new_size_va(kind, size, args);
  va_end(args);
  if (!newtag) return NULL;
  
  if (me->tag) {
    bxml_add_child(me->tag, newtag);
  } else if (me->root) {
    bxml_add_child(me->root, newtag);
  } else {
    me->root = newtag;
  }
  return newtag;
}

/* Returns nonzero if the parser is at the end, false if not */
int bxmlparser_is_end(BxmlParser * me) {
  if ((me->buffer.text + me->buffer.size) == (me->index)) return TRUE;
  if ('\0' == (*(me->index))) return TRUE;
  return FALSE; 
}

/* Gets curent byte, or '\0' if at end. */
int bxmlparser_now(BxmlParser * me) {
  if (bxmlparser_is_end(me)) return '\0';
  return *(me->index);
}

/* Gets curent index pointer location. */
char * bxmlparser_index(BxmlParser * me) {
  return me->index;
}


/* Advances the parser by one byte (and returns the  
 * the advance byte). Returns \0 if at end.*/
int bxmlparser_next(BxmlParser * me) {
  int now  = bxmlparser_now(me);
  if (bxmlparser_is_end(me)) return '\0';
  
  if (now != '\0') { 
    me->index++;
    if (now == '\n') {
      me->line++;
      me->col = 1;
    } else {
      me->col++;
    }
  }
  return bxmlparser_now(me);
}

/* Skips n characters or until end of string . */
int bxmlparser_skip(BxmlParser * me, int amount) {
  int now;
  int skipped = 0;
  if (bxmlparser_is_end(me)) return -1;
  
  for (now = bxmlparser_now(me); now != '\0' ; now = bxmlparser_next(me)) {
    if (skipped == amount) return skipped;
    skipped++;
  }
  return skipped;
}

/* Skips all characters in the set. */
int bxmlparser_skip_in(BxmlParser * me, char * set) {
  int now;
  int skipped = 0;
  if (bxmlparser_is_end(me)) return -1;

  
  for (now = bxmlparser_now(me); now != '\0' ; now = bxmlparser_next(me)) {
    if (!strchr(set, now)) return skipped;
    skipped++;
  }
  return skipped;
}

int bxmlparser_skip_not_in(BxmlParser * me, char * set) {
  int now;
  int skipped = 0;
  if (bxmlparser_is_end(me)) return -1;
  
  for (now = bxmlparser_now(me); now != '\0' ; now = bxmlparser_next(me)) {
    if (strchr(set, now)) return skipped;
    skipped++;
  }
  return skipped;  
}

/* Skips xml whitespace */
int bxmlparser_skip_ws(BxmlParser * me) {
  return bxmlparser_skip_in(me, BXML_WS);
}


/* Checks if the current character is inb the given set . */
int bxmlparser_now_in(BxmlParser * me, char * set) {
  if (bxmlparser_is_end(me)) return 0;
  return (!!strchr(set, bxmlparser_now(me)));
}

/* Checks if the current character is inb the given set . */
int bxmlparser_now_not_in(BxmlParser * me, char * set) {
  if (bxmlparser_is_end(me)) return 0;
  return (!strchr(set, bxmlparser_now(me)));
}

/* Checks if the current character is whitespace . */
int bxmlparser_now_ws(BxmlParser * me) {
  return bxmlparser_now_in(me, BXML_WS);
}


/* Checks if the current location starts with the given prefix */
int bxmlparser_have_prefix(BxmlParser * me, char * prefix) {
  if (bxmlparser_is_end(me)) return FALSE;
  return (strncmp(me->index, prefix, strlen(prefix)) == 0);
}

/* Tries to find the next "suffix" at or after the current parser position. 
 * Returns the relative position index of that suffix, or negative if not found.
 */
int bxmlparser_find_suffix(BxmlParser * me, char * suffix) {
  char * aid;
  aid = strstr(me->index, suffix);
  if (aid) {
    return aid - me->index;
  } else {
    return -1;
  }
}

/* Skips until a given suffix is skipped. Returns true if he suffix was found,
 * ro false if not. */
int bxmlparser_skip_suffix(BxmlParser * me, char * suffix) {
  int aid;
  aid = bxmlparser_find_suffix(me, suffix);
  if (aid < 0) return 0;
  /* Aslo skip suffix itself! */
  aid += strlen(suffix);
  return (bxmlparser_skip(me, aid) == aid);
}

/* Skips until a given prefix is skipped. Returns true if the prefix was found,
 * or false if not. */
int bxmlparser_skip_prefix(BxmlParser * me, char * prefix) {
  int aid;
  if (!bxmlparser_have_prefix(me, prefix)) return 0;
  aid = strlen(prefix);
  if (aid < 0) return 0;
  return (bxmlparser_skip(me, aid) == aid);
}


BxmlParser * bxmlparser_set_error(BxmlParser * me, const char * error) {
  me->error = error;
  return NULL;
}


/*
Here's is an overview of the syntax of full XML.
My parser will only support a subset of that. 

[1]     document           ::=          prolog element Misc*
[2]     Char       ::=          #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF]  
( any Unicode character, excluding the surrogate blocks, FFFE, and FFFF. )
[3]     S          ::=          (#x20 | #x9 | #xD | #xA)+
(
  The presence of #xD in the above production is maintained purely for backward compatibility with the First Edition. As explained in 2.11 End-of-Line Handling, all #xD characters literally present in an XML document are either removed or replaced by #xA characters before any other processing is done. The only way to get a #xD character to match this production is to use a character reference in an entity value literal.
)
[4]     NameStartChar      ::=          ":" | [A-Z] | "_" | [a-z] | [#xC0-#xD6] | [#xD8-#xF6] | [#xF8-#x2FF] | [#x370-#x37D] | [#x37F-#x1FFF] | [#x200C-#x200D] | [#x2070-#x218F] | [#x2C00-#x2FEF] | [#x3001-#xD7FF] | [#xF900-#xFDCF] | [#xFDF0-#xFFFD] | [#x10000-#xEFFFF]
[4a]    NameChar           ::=          NameStartChar | "-" | "." | [0-9] | #xB7 | [#x0300-#x036F] | [#x203F-#x2040]
[5]     Name       ::=          NameStartChar (NameChar)*
[6]     Names      ::=          Name (#x20 Name)*
[7]     Nmtoken    ::=          (NameChar)+
[8]     Nmtokens           ::=          Nmtoken (#x20 Nmtoken)*

Literals
[9]     EntityValue        ::=          '"' ([^%&"] | PEReference | Reference)* '"'
                        |  "'" ([^%&'] | PEReference | Reference)* "'"
[10]    AttValue           ::=          '"' ([^<&"] | Reference)* '"'
                        |  "'" ([^<&'] | Reference)* "'"
[11]    SystemLiteral      ::=          ('"' [^"]* '"') | ("'" [^']* "'")
[12]    PubidLiteral       ::=          '"' PubidChar* '"' | "'" (PubidChar - "'")* "'"
[13]    PubidChar          ::=          #x20 | #xD | #xA | [a-zA-Z0-9] | [-'()+,./:=?;!*#@$_%]

Character Data
[14]    CharData           ::=          [^<&]* - ([^<&]* ']]>' [^<&]*)
Comments
[15]    Comment    ::=          '<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'

[16]    PI         ::=          '<?' PITarget (S (Char* - (Char* '?>' Char*)))? '?>'
[17]    PITarget           ::=          Name - (('X' | 'x') ('M' | 'm') ('L' | 'l'))

[18]    CDSect     ::=          CDStart CData CDEnd
[19]    CDStart    ::=          '<![CDATA['
[20]    CData      ::=          (Char* - (Char* ']]>' Char*))
[21]    CDEnd      ::=          ']]>'

Prolog
[22]    prolog     ::=          XMLDecl? Misc* (doctypedecl Misc*)?
[23]    XMLDecl    ::=          '<?xml' VersionInfo EncodingDecl? SDDecl? S? '?>'
[24]    VersionInfo        ::=          S 'version' Eq ("'" VersionNum "'" | '"' VersionNum '"')
[25]    Eq         ::=          S? '=' S?
[26]    VersionNum         ::=          '1.' [0-9]+
[27]    Misc       ::=          Comment | PI | S

[28]    doctypedecl        ::=          '<!DOCTYPE' S Name (S ExternalID)? S? ('[' intSubset ']' S?)? '>'       [VC: Root Element Type]
                                [WFC: External Subset]
[28a]           DeclSep    ::=          PEReference | S         [WFC: PE Between Declarations]
[28b]           intSubset          ::=          (markupdecl | DeclSep)*
[29]    markupdecl         ::=          elementdecl | AttlistDecl | EntityDecl | NotationDecl | PI | Comment    [VC: Proper Declaration/PE Nesting]
                                [WFC: PEs in Internal Subset]

External Subset
[30]    extSubset          ::=          TextDecl? extSubsetDecl
[31]    extSubsetDecl      ::=          ( markupdecl | conditionalSect | DeclSep)*

Standalone Document Declaration
[32]    SDDecl     ::=          S 'standalone' Eq (("'" ('yes' | 'no') "'") | ('"' ('yes' | 'no') '"'))         [VC: Standalone Document Declaration]

NOTE:
To simplify the tasks of applications, the XML processor MUST behave as if it normalized all line breaks in external parsed entities (including the document entity) on input, before parsing, by translating both the two-character sequence #xD #xA and any #xD that is not followed by #xA to a single #xA character.

Element
[39]    element    ::=          EmptyElemTag
                        | STag content ETag     [WFC: Element Type Match]
                                [VC: Element Valid]
Start-tag
[40]    STag       ::=          '<' Name (S Attribute)* S? '>'  [WFC: Unique Att Spec]
[41]    Attribute          ::=          Name Eq AttValue        [VC: Attribute Value Type]
                                [WFC: No External Entity References]
                                [WFC: No < in Attribute Values]
End-tag
[42]    ETag       ::=          '</' Name S? '>'

Content of Elements
[43]    content    ::=          CharData? ((element | Reference | CDSect | PI | Comment) CharData?)*

Tags for Empty Elements
[44]    EmptyElemTag       ::=          '<' Name (S Attribute)* S? '/>' [WFC: Unique Att Spec]

Element Type Declaration
[45]    elementdecl        ::=          '<!ELEMENT' S Name S contentspec S? '>' [VC: Unique Element Type Declaration]
[46]    contentspec        ::=          'EMPTY' | 'ANY' | Mixed | children

Element-content Models
[47]    children           ::=          (choice | seq) ('?' | '*' | '+')?
[48]    cp         ::=          (Name | choice | seq) ('?' | '*' | '+')?
[49]    choice     ::=          '(' S? cp ( S? '|' S? cp )+ S? ')'      [VC: Proper Group/PE Nesting]
[50]    seq        ::=          '(' S? cp ( S? ',' S? cp )* S? ')'      [VC: Proper Group/PE Nesting]

Mixed-content Declaration
[51]    Mixed      ::=          '(' S? '#PCDATA' (S? '|' S? Name)* S? ')*'
                        | '(' S? '#PCDATA' S? ')'       [VC: Proper Group/PE Nesting]
                                [VC: No Duplicate Types]

Attribute-list Declaration
[52]    AttlistDecl        ::=          '<!ATTLIST' S Name AttDef* S? '>'
[53]    AttDef     ::=          S Name S AttType S DefaultDecl

[54]    AttType    ::=          StringType | TokenizedType | EnumeratedType
[55]    StringType         ::=          'CDATA'
[56]    TokenizedType      ::=          'ID'    [VC: ID]
                                [VC: One ID per Element Type]
                                [VC: ID Attribute Default]
                        | 'IDREF'       [VC: IDREF]
                        | 'IDREFS'      [VC: IDREF]
                        | 'ENTITY'      [VC: Entity Name]
                        | 'ENTITIES'    [VC: Entity Name]
           [22]    prolog     ::=          XMLDecl? Misc* (doctypedecl Misc*)?             | 'NMTOKEN'     [VC: Name Token]
                        | 'EmptyElemTag
                        | STag content ETag     [WFC: Element Type Match]NMTOKENS'    [VC: Name Token]

Enumerated Attribute Types
[57]    EnumeratedType     ::=          NotationType | Enumeration
[58]    NotationType       ::=          'NOTATION' S '(' S? Name (S? '|' S? Name)* S? ')'       [VC: Notation Attributes]
                                [VC: One Notation Per Element Type]
                                [VC: No Notation on Empty Element]
                                [VC: No Duplicate Tokens]
[59]    Enumeration        ::=          '(' S? Nmtoken (S? '|' S? Nmtoken)* S? ')'      [VC: Enumeration]
                                [VC: No Duplicate Tokens]

[60]    DefaultDecl        ::=          '#REQUIRED' | '#IMPLIED'
                        | (('#FIXED' S)? AttValue)      [VC: Required Attribute]
                                [VC: Attribute Default Value Syntactically Correct]
                                [WFC: No < in Attribute Values]
                                [VC: Fixed Attribute Default]
                                [WFC: No External Entity References]

Conditional Section
[61]    conditionalSect    ::=          includeSect | ignoreSect
[62]    includeSect        ::=          '<![' S? 'INCLUDE' S? '[' extSubsetDecl ']]>'   [VC: Proper Conditional Section/PE Nesting]
[63]    ignoreSect         ::=          '<![' S? 'IGNORE' S? '[' ignoreSectContents* ']]>'      [VC: Proper Conditional Section/PE Nesting]
[64]    ignoreSectContents         ::=          Ignore ('<![' ignoreSectContents ']]>' Ignore)*
[65]    Ignore     ::=          Char* - (Char* ('<![' | ']]>') Char*)

4.1 Character and Entity References

[66]    CharRef    ::=          '&#' [0-9]+ ';'
                        | '&#x' [0-9a-fA-F]+ ';'        [WFC: Legal Character]

Entity Reference
[67]    Reference          ::=          EntityRef | CharRef
[68]    EntityRef          ::=          '&' Name ';'    [WFC: Entity Declared]
                                [VC: Entity Declared]
                                [WFC: Parsed Entity]
                                [WFC: No Recursion]
[69]    PEReference        ::=          '%' Name ';'    [VC: Entity Declared]
                                [WFC: No Recursion]
                                [WFC: In DTD]

Entity Declaration
[70]    EntityDecl         ::=          GEDecl | PEDecl
[71]    GEDecl     ::=          '<!ENTITY' S Name S EntityDef S? '>'
[72]    PEDecl     ::=          '<!ENTITY' S '%' S Name S PEDef S? '>'
[73]    EntityDef          ::=          EntityValue | (ExternalID NDataDecl?)
[74]    PEDef      ::=          EntityValue | ExternalID

External Entity Declaration
[75]    ExternalID         ::=          'SYSTEM' S SystemLiteral
                        | 'PUBLIC' S PubidLiteral S SystemLiteral
[76]    NDataDecl          ::=          S 'NDATA' S Name        [VC: Notation Declared]

Text Declaration
[77]    TextDecl           ::=          '<?xml' VersionInfo? EncodingDecl S? '?>'

Well-Formed External Parsed Entity
[78]    extParsedEnt       ::=          TextDecl? content

[80]    EncodingDecl       ::=          S 'encoding' Eq ('"' EncName '"' | "'" EncName "'" )
[81]    EncName    ::=          [A-Za-z] ([A-Za-z0-9._] | '-')* ( Encoding name contains only Latin characters )

4.6 Predefined Entities
<!ENTITY lt     "&#38;#60;">
<!ENTITY gt     "&#62;">
<!ENTITY amp    "&#38;#38;">
<!ENTITY apos   "&#39;">
<!ENTITY quot   "&#34;">

Notation Declarations
[82]    NotationDecl       ::=          '<!NOTATION' S Name S (ExternalID | PublicID) S? '>'    [VC: Unique Notation Name]
[83]    PublicID           ::=          'PUBLIC' S PubidLiteral 

*/



BxmlParser * bxmlparser_parse_comment(BxmlParser * me) {
  bxmlparser_skip_prefix(me, "<!--");
  if (!bxmlparser_skip_suffix(me, "-->")) { 
    return bxmlparser_set_error(me, "cannot find closing -->");
  } 
  return me;
}

BxmlParser * bxmlparser_parse_cdata(BxmlParser * me) {
  Bxml * last;
  char * start;
  int size;
  bxmlparser_skip_prefix(me, "<![CDATA[");
  start = bxmlparser_index(me);
  size  = bxmlparser_find_suffix(me, "]]>");
  if (size < 0) { 
    return bxmlparser_set_error(me, "cannot find end of CDATA ]]");
  } 
  
  last = bxml_get_last_child(me->tag); 
  /* compact text if possible */
  if (last && (last->kind == BXML_TEXT)) {
    bxml_append_buf(last, start, size);
  } else {  
    bxmlparser_add_tag_size(me, BXML_TEXT, size, start);  
  }
  bxmlparser_skip(me, size + 3);
  
  return me;
}

BxmlParser * bxmlparser_parse_xmldecl(BxmlParser * me) {
  if (!bxmlparser_skip_suffix(me, "?>")) { 
    return bxmlparser_set_error(me, "cannot find closing ?> of xml declaration tag");
  } 
  /* TODO: implement this. */
  /* Skip whitespace to make sure following tags are found. */
  bxmlparser_skip_ws(me);
  return me;
}


BxmlParser * bxmlparser_parse_processing(BxmlParser * me) {
  if (!bxmlparser_skip_suffix(me, "?>")) { 
    return bxmlparser_set_error(me,"cannot find closing ?>");
  } 
  /* TODO: implement this. */
  return me;
}

/* Parses a doctype declaration. 
* [28]    doctypedecl        ::=          '<!DOCTYPE' S Name (S ExternalID)? S? ('[' intSubset ']' S?)? '>'
*/
BxmlParser * bxmlparser_parse_doctype(BxmlParser * me) {
  if(!bxmlparser_skip_prefix(me, "<!DOCTYPE")) return NULL;
  if (!bxmlparser_skip_suffix(me, ">")) { 
    return bxmlparser_set_error(me, "cannot find closing > for doctype");
  } 
  /* TODO: implement this. */
  return me;
}

BxmlParser * bxmlparser_parse_entity(BxmlParser * me) {
  if(!bxmlparser_skip_prefix(me, "<!ENTITY")) return NULL;
  if (!bxmlparser_skip_suffix(me, ">")) { 
    return bxmlparser_set_error(me,"cannot find closing > for entity");
  } 
  /* TODO: implement this. */
  return me;
}


BxmlParser * bxmlparser_parse_attlist_tag(BxmlParser * me) {
  if(!bxmlparser_skip_prefix(me, "<!ATTLIST")) return NULL;
  if (!bxmlparser_skip_suffix(me, ">")) { 
    return bxmlparser_set_error(me, "cannot find closing > for attlist");
  } 
  /* TODO: implement this. */
  return me;
}


int bxmlparser_have_comment(BxmlParser * me) {  
  return (bxmlparser_have_prefix(me, "<!--"));
}

int bxmlparser_have_end_tag(BxmlParser * me) {  
  return (bxmlparser_have_prefix(me, "</"));
}

int bxmlparser_have_processing(BxmlParser * me) {  
  return (bxmlparser_have_prefix(me, "<?"));
}

int bxmlparser_have_reference(BxmlParser * me) {  
  return (bxmlparser_have_prefix(me, "&"));
}

int bxmlparser_have_bang(BxmlParser * me) {  
  return (bxmlparser_have_prefix(me, "<!"));
}

int bxmlparser_have_cdata(BxmlParser * me) {  
  return (bxmlparser_have_prefix(me, "<![CDATA["));
}


int bxmlparser_have_element(BxmlParser * me) {  
  if (bxmlparser_have_processing(me)) return 0;
  if (bxmlparser_have_bang(me))       return 0;
  if (bxmlparser_have_end_tag(me))    return 0;
  return bxmlparser_have_prefix(me, "<");
}

int bxmlparser_have_chardata(BxmlParser * me) {  
  if (bxmlparser_have_prefix(me, "<")) return 0;
  if (bxmlparser_have_prefix(me, "&")) return 0;
  return !bxmlparser_is_end(me);
}


/*
 * [4]     NameStartChar      ::=          ":" | [A-Z] | "_" | [a-z] | [#xC0-#xD6] | [#xD8-#xF6] | [#xF8-#x2FF] | [#x370-#x37D] | [#x37F-#x1FFF] | [#x200C-#x200D] | [#x2070-#x218F] | [#x2C00-#x2FEF] | [#x3001-#xD7FF] | [#xF900-#xFDCF] | [#xFDF0-#xFFFD] | [#x10000-#xEFFFF]
 * [4a]    NameChar           ::=          NameStartChar | "-" | "." | [0-9] | #xB7 | [#x0300-#x036F] | [#x203F-#x2040]
 * [5]     Name       ::=          NameStartChar (NameChar)*
 * 
 * However I won't support non ASCII names of now
 * If ok Name is set to point into the parer's buffer and should not be freed.
 */
BxmlParser * bxmlparser_parse_name(BxmlParser * me, char ** name, int * size) {
  int size1, size2;
  char * text;
  text = bxmlparser_index(me);
  size1 = bxmlparser_skip_in(me, BXML_ALPHA ":_");
  if (size1 < 0) return bxmlparser_set_error(me, "unexpected character in name");
  if (size1 < 1) return bxmlparser_set_error(me, "name too short");
  size2 = bxmlparser_skip_in(me, BXML_ALNUM ":_-.");
  if (size2 < 0) return bxmlparser_set_error(me, "unexpected character in rest of name");
  (*name) = text;
  (*size) = size1 + size2;
  return me;
}

int bxmlparser_encode_utf8(long lchar, char * buf) {
  int bits;
  int bytes;
  int result;
  
  if (lchar < 0x80) { /* ASCII */
    buf[0] = (char) lchar;
    result = 1; 
  } else {
      /* Bit and byte count */
      for (bits = 0; lchar; lchar /=2) bits++;
      bytes  = (bits - 2) / 5;
      result = bytes;
      /* First header byte. */
      (*buf) = (0xFF << (7 - bytes)) | (lchar >> (6 * bytes));
      buf++;
      /* Data bytes. */
      while (bytes) {
        (*buf) = 0x80 | ( lchar >> (6 * bytes)); 
        buf++;
        bytes--;
      }
   }   
   return result;
}

/* Use as table to look up the character of the given entities. */
static char * bxmlparser_entity_lut[6][2] = {
  { "&"  , "amp"    },
  { "'"  , "apos"   },
  { ">"  , "gt"     },
  { "<"  , "lt"     },
  { "\"" , "quot"   },
  { NULL , NULL     }
};


char * bxmlparser_lookup_entity(BxmlParser * me, char * name, size_t size) {
  int index; 
  for (index = 0; bxmlparser_entity_lut[index][0]; index ++) {
    if (0 == strncmp(bxmlparser_entity_lut[index][1], name, size)) {
      return bxmlparser_entity_lut[index][0];
    }
  } 
  (void) me;
  return NULL;
}


static char bxmlparser_reference_buffer[12];


/* Parses an entity reference into a string. Returns a reference to a const or 
 * static string buffer and a size. */
BxmlParser * bxmlparser_parse_reference_to_string(BxmlParser * me, char ** str_out, int * size_out) {
  int    numerical;
  int    size;
  long   lchar;
  char * text;
  if (!bxmlparser_skip_prefix(me, "&")) return bxmlparser_set_error(me, "starting & of reference not found");
  numerical = bxmlparser_skip_prefix(me, "#");
  text      = bxmlparser_index(me);
  size      = bxmlparser_find_suffix(me, ";"); 
  if (size < 1) return bxmlparser_set_error(me, "empty reference");
  bxmlparser_skip_suffix(me, ";"); 
    
  if (numerical) {
    int base = 10;
    int bytes;
    if ((text[0] == 'x') || (text[0] == 'X')) {
      base = 16;
      text++;
      size--;
    }
    errno = 0;
    lchar = strtol(text, NULL, base);
    if (errno)
    if (size >  (int)(sizeof(bxmlparser_reference_buffer) - 1)) {
      size = (sizeof(bxmlparser_reference_buffer) - 1);
    }
    bytes = bxmlparser_encode_utf8(lchar, bxmlparser_reference_buffer);
    (*str_out)  = bxmlparser_reference_buffer;
    (*size_out) = bytes;
  } else {
    char * replace = bxmlparser_lookup_entity(me, text, size);
    if (!replace) return bxmlparser_set_error(me, "Unknown entity reference!");
    (*str_out)  = replace;
    (*size_out) = strlen(replace);
  }
  return me;
}


/* Parses an entity reference. */
BxmlParser * bxmlparser_parse_reference(BxmlParser * me) {
  char * str;
  int    size;
  Bxml * last;
  if (!bxmlparser_parse_reference_to_string(me, &str, &size)) {
    return NULL;
  }
  last = bxml_get_last_child(me->tag); 
  /* compact text if possible */
  if (last && (last->kind == BXML_TEXT)) {
    bxml_append_buf(last, str, size);
  } else {  
    bxmlparser_add_tag_size(me, BXML_TEXT, size, str);
  }
  return me;
}


/* [10]    AttValue           ::=          '"' ([^<&"] | Reference)* '"'
                        |  "'" ([^<&'] | Reference)* "'"
 */
BxmlParser * bxmlparser_parse_attribute_value(BxmlParser * me, Swis * buffer) {
  char * start;
  char * ref;
  int size;
  int ch;
  if (!bxmlparser_skip_prefix(me, "\"")) 
    return bxmlparser_set_error(me, "cannot find start of attribute value");
    
  ch = bxmlparser_now(me);  
  while (ch != '"') {
    if (ch == '&') {
      if(!bxmlparser_parse_reference_to_string(me, &ref, &size))
        return NULL;
      swis_append_buf(buffer, ref, size);
      ch = bxmlparser_now(me);
    } else {
      swis_append_char(buffer, ch);
      ch = bxmlparser_next(me);
    }
  }
  if (!bxmlparser_skip_prefix(me, "\"")) 
    return bxmlparser_set_error(me, "cannot find end of attribute value");
  return me;  
}

/* [41]    Attribute          ::=          Name Eq AttValue  */      

BxmlParser * bxmlparser_parse_attribute(BxmlParser * me, Bxml * tag) {
  char * name = NULL;
  int namesize;
  Swis value;
  
  bxmlparser_skip_ws(me);
  
  if (!bxmlparser_parse_name(me, &name, &namesize)) {
    return bxmlparser_set_error(me, "Cannot parse name in attribute.");
  } 
  
  bxmlparser_skip_ws(me);
  
  if (!bxmlparser_skip_prefix(me, "=")) {
    return bxmlparser_set_error(me, "Cannot find = in attribute.");
  }
  
  bxmlparser_skip_ws(me);
  if (!swis_new_empty(&value)) 
    return bxmlparser_set_error(me, "out of memory in attribute value parsing");
    
  if (!bxmlparser_parse_attribute_value(me, &value)) {
    swis_free(&value);
    return bxmlparser_set_error(me, "Cannot parse attribute value");
  }
  
  bxml_new_attribute_size(tag, name, namesize, value.text, value.size);
  swis_free(&value); 
  return me;
}


BxmlParser * bxmlparser_parse_attribute_list(BxmlParser * me, Bxml * tag) {
  bxmlparser_skip_ws(me);
  while (!bxmlparser_now_in(me, "/>")) {
    if (!bxmlparser_parse_attribute(me, tag)) return NULL;
    bxmlparser_skip_ws(me);
  }
  return me;
}



/* Parses in-between character data that is not in a CDATA section. */
BxmlParser * bxmlparser_parse_chardata(BxmlParser * me)  {
  Bxml * last;
  int    size;
  char * text;
  text = bxmlparser_index(me);
  size = bxmlparser_skip_not_in(me, "<&");
  if (size < 0) { 
    return bxmlparser_set_error(me, "trouble in character data");    
  }
  
  last = bxml_get_last_child(me->tag); 
  /* compact text if possible */
  if (last && (last->kind == BXML_TEXT)) {
    bxml_append_buf(last, text, size);
  } else {  
    if (!bxmlparser_add_tag_size(me, BXML_TEXT, size, text)) {
      return bxmlparser_set_error(me, "could not allocate character tag");    
    }
  }
  return me;
}



/* Forward declaration. */
BxmlParser * bxmlparser_parse_element(BxmlParser * me);

/* Parses the contents of a non-empty tag. 
 * [43]    content    ::=          CharData? ((element | Reference | CDSect | PI | Comment) CharData?)*
 */
BxmlParser * bxmlparser_parse_contents(BxmlParser * me) {  
  Bxml * tagi = NULL;
  int done = 0;
  if (bxmlparser_have_chardata(me)) {
    if (!bxmlparser_parse_chardata(me)) return NULL;
  }
  
  while (!done) {
    Bxml * now = me->tag;
    
    if (bxmlparser_have_reference(me)) {
      if (!bxmlparser_parse_reference(me)) return NULL;
    } else if (bxmlparser_have_cdata(me)) {
      if (!bxmlparser_parse_cdata(me)) return NULL;
    } else if (bxmlparser_have_processing(me)) {
      if (!bxmlparser_parse_processing(me)) return NULL;
    } else if (bxmlparser_have_comment(me)) {
      if (!bxmlparser_parse_comment(me)) return NULL;
    } if (bxmlparser_have_element(me)) {
      if (!bxmlparser_parse_element(me)) return NULL;
    } else {
      done = 1;
    }
    /* Restore current tag from the ravages of recursion. */
    me->tag = now; 
    /* Try to parse any other chardata if needed. */
    if (bxmlparser_have_chardata(me)) {
      if (!bxmlparser_parse_chardata(me)) return NULL;
    }
  }
  
  return me;
}


/* Parses the opening <, the name and the attributes of a tag. */
BxmlParser * bxmlparser_parse_tag_head(BxmlParser * me) {
  char * name;
  int    size; 
  Bxml * tag;
  
  if (!bxmlparser_skip_prefix(me, "<")) { 
    return bxmlparser_set_error(me, "Cannot find opening < of tag.");
  }
  
  if (!bxmlparser_parse_name(me, &name, &size)) { 
    return bxmlparser_set_error(me, "Cannot parse name of tag.");
  }

  tag     = bxmlparser_add_tag_size(me, BXML_TAG, size, name);  
  me->tag = tag;  
  
  if (!bxmlparser_parse_attribute_list(me, tag)) { 
    return bxmlparser_set_error(me, "Cannot parse attribute list of tag.");
  }  
  
  bxmlparser_skip_ws(me);
    
  return me;
}



/* STag content ETag */
BxmlParser * bxmlparser_parse_open_close_tag(BxmlParser * me) { 
  char * name;
  int  size; 
  
  /* start tag */
  if (!bxmlparser_skip_prefix(me, ">")) { 
    return bxmlparser_set_error(me, "Cannot find closing > of tag.");
  }
  
  /* contents of the tag */
  if (!bxmlparser_parse_contents(me)) return NULL;
  
  /* end tag */
  if (!bxmlparser_skip_prefix(me, "</")) return NULL;
  
  if (!bxmlparser_parse_name(me, &name, &size)) { 
    return bxmlparser_set_error(me, "Cannot parse name of end of tag.");
  }
  
  bxmlparser_skip_ws(me);
    
  if (!bxmlparser_skip_prefix(me, ">")) { 
    return bxmlparser_set_error(me, "Cannot find closing > of end tag");
  }
  
  if (strncmp(me->tag->name, name, (size_t)size) != 0) {
    return bxmlparser_set_error(me, "Name of end tag does not match begin tag.");
  } 
  
  return me; 
}


/*
[44]    EmptyTag       ::=          '<' Name (S Attribute)* S? '/>' [WFC: Unique Att Spec]
*/
BxmlParser * bxmlparser_parse_empty_tag(BxmlParser * me) { 
  if (!bxmlparser_skip_prefix(me, "/>")) return NULL;
  return me; 
}


/* element := EmptyElemTag | STag content ETag */
BxmlParser * bxmlparser_parse_element(BxmlParser * me) {  
  int now;
  now = bxmlparser_now(me);
  if (now != '<') {
    return bxmlparser_set_error(me, "cannot find beginning of tag");
  }
  
  /* Parse the head of the tag first. */
  if (!bxmlparser_parse_tag_head(me)) return NULL;
  /* now check for a close tag. */
  bxmlparser_skip_ws(me);
  /* If found, it's an empty tag, otherwise try a normal tag. */
  if (bxmlparser_have_prefix(me, "/>")) {
    return bxmlparser_parse_empty_tag(me);
  } else {
    return bxmlparser_parse_open_close_tag(me);
  }
}

/** 
 * Checks if a misc tag seems to be ready.
 */
int bxmlparser_have_misc(BxmlParser * me) {  
  if (bxmlparser_have_prefix(me, "<!--")) return 1;
  if (bxmlparser_have_prefix(me, "<?")) return 2;
  if (bxmlparser_now_ws(me)) return 3;    
  return 0;
}

/*
 * Parses a Misc part of XML as per the rule:
[27]    Misc       ::=          Comment | PI | S
*/
BxmlParser * bxmlparser_parse_misc(BxmlParser * me) {  
  if (bxmlparser_have_comment(me)) return bxmlparser_parse_comment(me);
  if (bxmlparser_have_processing(me)) return bxmlparser_parse_processing(me);
  if (bxmlparser_now_ws(me)) { 
    bxmlparser_skip_ws(me);    
    return me;
  }
  return NULL;  
}

/* Parses Misc*  lists */
BxmlParser * bxmlparser_parse_misc_list(BxmlParser * me) {  
  BxmlParser * ok;
  while (bxmlparser_have_misc(me)) {
    ok = bxmlparser_parse_misc(me);
    if (!ok) return NULL;
  }
  return me;
}
 
/*
* Parses a prolog as per rule 22:
*  [22]    prolog     ::=          XMLDecl? Misc* (doctypedecl Misc*)?
*/
BxmlParser * bxmlparser_parse_prolog(BxmlParser * me) {  
  BxmlParser * ok;
  if (bxmlparser_have_prefix(me, "<?xml")) {
    ok = bxmlparser_parse_xmldecl(me);
    if (!ok) return NULL;
  }
  
  ok = bxmlparser_parse_misc_list(me);
  if (!ok) return NULL;  
  if (bxmlparser_have_prefix(me, "<!DOCTYPE")) {
    ok = bxmlparser_parse_doctype(me);
    if (!ok) return NULL;
    ok = bxmlparser_parse_misc_list(me);
    if (!ok) return NULL;
  }
  return ok;
}


/* 
 * Parses a document as per rule [1]. 
 * document           ::=          prolog element Misc*
 */
BxmlParser * bxmlparser_parse_document(BxmlParser * me) {  
  BxmlParser * ok = bxmlparser_parse_prolog(me);
  if (!ok) return NULL;
  ok = bxmlparser_parse_element(me);
  if (!ok) return NULL;
  ok = bxmlparser_parse_misc_list(me);
  return ok;
}


/* Initiates the parse. */
BxmlParser * bxmlparser_parse(BxmlParser * me) {
  bxmlparser_skip_ws(me);  
  return bxmlparser_parse_document(me);
}

Bxml * bxmlparser_parse_buf(BxmlParser * me, char * buf, int size) {
  Bxml * result = NULL;
  if ((!me) || (!buf)) { 
    return NULL;
  }
  
  swis_init_buf(&(me->buffer), buf, size);
  bxmlparser_parse(me);
  result = me->root;
  return result;
}


Bxml * bxmlparser_parse_str(BxmlParser * me, char * buf) {
  return bxmlparser_parse_buf(me, buf, strlen(buf));
}


/** Reads in a file into the parser's buffer. */
BxmlParser * bxmlparser_read_file(BxmlParser * me, FILE * file) {
  if (!swis_read_file(&me->buffer, file)) return NULL;
  me->index = me->buffer.text;
  return me;
}

Bxml * bxmlparser_parse_file(BxmlParser * me, FILE * file) {
  Bxml * result = NULL;
  if (!file) { 
    return NULL;
  }
  
  if (bxmlparser_read_file(me, file)) {
    bxmlparser_parse(me);
    result = me->root;
  }
  return result;
}

Bxml * bxmlparser_parse_filename(BxmlParser * me, char * filename) {
  Bxml * result;
  FILE * file;
  file = fopen(filename, "r");
  result = bxmlparser_parse_file(me, file);
  fclose(file);
  return result;
}


const char * bxmlparser_get_error(BxmlParser * me) {
  return me->error;
}

int bxmlparser_get_line(BxmlParser * me) {
  return me->line;
}

int bxmlparser_get_column(BxmlParser * me) {
  return me->col;
}

