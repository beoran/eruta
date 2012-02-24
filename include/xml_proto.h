#ifndef XML_PROTO_H
#define XML_PROTO_H
/*
This file was autogenerated from src/xml.c
by bin/genproto on 2012-02-24 15:19:02 +0100
Please do not hand edit.
*/

/**
* Xml is both an XML node and an xml document.
*/
struct Xml_;
typedef struct Xml_ Xml;

/**
* Xmlparser is an XML parser.
*/
struct Xmlparser_;
typedef struct Xmlparser_ Xmlparser;

/** Allocates a new XML document, attribute or node. */
Xml * xml_new(STR * tag, STR * value);

/** Allocates a new XML document, attribute or node. */
Xml * xml_newcstr(const char * tag, const char * value);

/** Frees an XML document recursively */
Xml * xml_free(Xml * xml);

/** Deallocates an XML document recursively.
   Children, siblings and attributes will be freed.  */
Xml * xml_done(Xml * xml);

/** Adds a sibling to an existing xml node and returns it.
* It will scan through the sibling list and append it to the end. */
Xml * xml_addsibling(Xml * xml, Xml * sibling);

/** Adds a child to an existing xml node. */
Xml * xml_addchild(Xml * xml, Xml * child);  

/** Adds an attribute attr to an existing xml node. */
Xml * xml_addattribute(Xml * xml, Xml * attr);

/** Creates a new attribute for this node and returns it.
Returns NULL if out of memory. */
Xml * xml_newattribute(Xml * xml, STR * name, STR * value);

/** Creates a new attribute for this node and returns it.
Returns NULL if out of memory. */
Xml * xml_newattributecstr(Xml * xml, const char * name, const char * value);

/** Creates a new child node for this node and returns it.
Returns NULL if out of memory. */
Xml * xml_newchild(Xml * xml, STR * name);

/** Creates a new child node for this node and returns it.
Returns NULL if out of memory. */
Xml * xml_newchildcstr(Xml * xml, const char * name);

/** Creates a new text node child for this node and returns it.
Returns NULL if out of memory. */
Xml * xml_newtext(Xml * xml, STR * text);

/** Creates a new text child node for this node and returns it.
Returns NULL if out of memory. */
Xml * xml_newtextcstr(Xml * xml, const char * text);

#endif // XML_PROTO_H

