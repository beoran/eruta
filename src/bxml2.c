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
                        | 'NMTOKEN'     [VC: Name Token]
                        | 'NMTOKENS'    [VC: Name Token]

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


typedef struct BXML2Parser_ BXML2Parser;

struct BXML2Parser_ {
  char * text;
  char * now;
  int    size;
  int    index;
  int    line;
  int    col;
  int    error;
  char * errtext;
}; 


bxml2parser_init(BXML2Parser * parser, char * text, int size) {
  parser->text  = text;
  parser->now   = text;
  parser->size  = size;
  parser->index = 0;
  parser->line  = 1;
  parser->col   = 1;  
}


static int is_xml_space(int ch) { 
  return (ch == 0x20) ||  (ch == 0x09) || (ch == 0x0D) || ( ch == 0x0A);
}


/* [1]     document           ::=          prolog element Misc* */
static int parse_prolog(BXML2Parser * self) {
  
}




/* [1]     document           ::=          prolog element Misc* */
static int parse_document(BXML2Parser * self) {
  int res ;
  res = parse_prolog(self);
  if (res) return res;
  res = parse_element(self);  
  if (res) return res;
  res = 0; 
  while (!res) { 
    parse_misc(); 
  } 
  return 0;
}





