/*  Small Xml parser. Yes, I have to reinvent the wheel again! :p */
#include <ctype.h>
#include "dynar.h"
#include "xml.h"
#include "mem.h"
#include "str.h"

/**
* Xml is both an XML node and an xml document.
*/
struct Xml_ {  
  STR * tag; /* Name of the tag or attibute, or #text if a text node */
  struct Xml_ * sibling, * parent, * child, * attribute;
  /* Sibling node (if any), parent node (if any),
  first child node (if any), or first attribute (if any).
  This allows linked list-style structures, to keep adding and removing nodes
  from the document easier and simpler at the expense of some speed for
  accessing the document.
  */
  STR * value; /* For #text nodes, or for attribute values.*/
};


/**
* Xmlparser is an XML parser.
*/
struct Xmlparser_ {
  Xml * result; /* Top level node. */
  Xml * last;   /* Last parsed node */
  STR * tag;      /* Tag name currently parsing */
  STR * value;    /* value currently parsing */
};

/* Functionality for XML */

Xml * xml_initall(Xml * xml, STR * tag, STR * value,
                  Xml * parent, Xml * sibling, Xml * child, Xml * attribute) {
  if(!xml) return NULL;
  xml->tag        = str_dup(tag);
  if(value)       {
    xml->value    = str_dup(value);
  } else          {
   xml->value     = NULL;
  }  
  xml->parent     = parent;
  xml->sibling    = sibling;
  xml->child      = child;
  xml->attribute  = attribute;
  return xml;
}

Xml * xml_init(Xml * xml, STR * tag, STR * value) {
  return xml_initall(xml, tag, value, NULL, NULL, NULL, NULL);
}

/** Allocates a new XML document, attribute or node. */
Xml * xml_new(STR * tag, STR * value) {
  Xml * xml = MEM_ALLOCATE(Xml);
  return xml_init(xml, tag, value);
}

/** Frees an XML document recursively */
Xml * xml_free(Xml * xml) {
  xml_done(xml);
  mem_free(xml);
  return NULL;
}

/** Deallocates an XML document recursively.
   Children, siblings and attributes will be freed.  */
Xml * xml_done(Xml * xml) {
  if(!xml) return xml;
  str_free(xml->tag);
  xml->tag   = NULL;
  if(xml->value) str_free(xml->value);
  xml->value = NULL;
  xml_free(xml->attribute); // free attribute list
  xml_free(xml->sibling); // free sibling list
  xml_free(xml->child); // free chilren list
  // parent is not freed. Clear relation pointers.
  xml->attribute = xml->child = xml->sibling = xml->parent = NULL;
  return xml;
}

/** Adds a sibling to an existing xml node and returns it.
* It will scan through the sibling list and append it to the end. */
Xml * xml_addsibling(Xml * xml, Xml * sibling) {
  Xml * aid      = xml->sibling;
  if(aid) { // Skip though linked list.
    while (aid->sibling) {  aid = aid->sibling;  }
    aid->sibling = sibling; // set sibling at end.         
  } else { // No sibling yet.
    xml->sibling = sibling;
  }
  return sibling;
}

/** Adds a child to an existing xml node. */
Xml * xml_addchild(Xml * xml, Xml * child) {  
  if(xml->child) {
    // Add as a sibling of the existing child if there was one already
    xml_addsibling(xml->child, child);  
  } else { // no children yet, become only child.
    xml->child = child;
  } 
  return child;  
}

/** Adds an attribute attr to an existing xml node. */
Xml * xml_addattribute(Xml * xml, Xml * attr) {
  if(xml->attribute) {
    // Add as a sibling of the existing attribute if there was one already
    xml_addsibling(xml->attribute, attr);
  } else { // no children yet, become only child.
    xml->attribute = attr;
  }
  return attr;
}











