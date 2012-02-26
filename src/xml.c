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

/** Allocates a new XML document, attribute or node. */
Xml * xml_newcstr(const char * tag, const char * value) {
  STR_INFO tagi, valuei; STR * tags = NULL,  * values = NULL;
  if (tag)   tags   = str_refcstr(&tagi  , tag);
  if (value) values = str_refcstr(&valuei, value);  
  return xml_new(tags, values);
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

/** Creates a new attribute for this node and returns it.
Returns NULL if out of memory. */
Xml * xml_newattribute(Xml * xml, STR * name, STR * value) {
  Xml * attr = xml_new(name, value);
  if (!attr) return NULL;
  return xml_addattribute(xml, attr);
}

/** Creates a new attribute for this node and returns it.
Returns NULL if out of memory. */
Xml * xml_newattributecstr(Xml * xml, const char * name, const char * value) {
  Xml * attr = xml_newcstr(name, value);
  if (!attr) return NULL;
  return xml_addattribute(xml, attr);
}


/** Creates a new child node for this node and returns it.
Returns NULL if out of memory. */
Xml * xml_newchild(Xml * xml, STR * name) {
  Xml * attr = xml_new(name, NULL);
  if (!attr) return NULL;
  return xml_addchild(xml, attr);
}

/** Creates a new child node for this node and returns it.
Returns NULL if out of memory. */
Xml * xml_newchildcstr(Xml * xml, const char * name) {
  Xml * attr = xml_newcstr(name, NULL);
  if (!attr) return NULL;
  return xml_addchild(xml, attr);
}

/** Creates a new text node child for this node and returns it.
Returns NULL if out of memory. */
Xml * xml_newtext(Xml * xml, STR * text) {
  str_const(name, "#text"); // string constant for the #text header
  Xml * attr = xml_new(name, text);
  if (!attr) return NULL;
  return xml_addchild(xml, attr);
}

/** Creates a new text child node for this node and returns it.
Returns NULL if out of memory. */
Xml * xml_newtextcstr(Xml * xml, const char * text) {
  Xml * attr = xml_newcstr("#text", text);
  if (!attr) return NULL;
  return xml_addchild(xml, attr);
}

/** Iterates over each sibling node of this xml node using the EachDo walker
interface */
Xml * xml_eachsibling(Xml * xml, EachDo * todo, void * data) {
  Xml * res = NULL;
  Xml * aid = xml;
  Each each;
  if (!xml) return NULL;
  // Initialize the each Each struct that holds iteration info.
  each_init(&each, (void *) aid, data);
  while(aid) {
    each_next(&each, (void *) aid); // aid is the next element 
    res = todo(&each);
    if (res) return res; // break if something was "found".
    aid = aid->sibling;
  }
  return NULL;
}

/** Iterates over each direct child node of this xml node using the EachDo
walker interface, breadth-only. */
Xml * xml_eachchild(Xml * xml, EachDo * todo, void * data) {
  return xml_eachsibling(xml->child, todo, data);
  // Children are in the siblings of the children.
}

/** Iterates over each attibute node of this xml node using the EachDo
walker interface, breadth-only. */
Xml * xml_eachattribute(Xml * xml, EachDo * todo, void * data) {
  return xml_eachsibling(xml->attribute, todo, data);
  // Children are in the siblings of the children.
}

/* helper for finding functions, finds a tag or atribute by name */
static void * findtag_each(Each * each) {
  Xml * xml  = (Xml *)each_now(each);
  STR * name = (STR *)each_extra(each);
  if(!name) return NULL;
  if(str_equal(name, xml->tag)) return xml;
  return NULL;
}

/** Find an attribute with the given name and returns it's STR * value,
or NULL if not found. */
STR * xml_findattribute_strstr(Xml * xml, STR * name) {  
  Xml * attr = xml_eachattribute(xml, findtag_each, name);
  if(attr) return attr->value;
  return NULL;
}

/** Find a attribute with given name returns it's STR * value, 
or NULL if not found. */
STR * xml_findattribute_cstrstr(Xml * xml, const char * cname) {
  str_const(name, cname); // string constant for the nam header
  return xml_findattribute_strstr(xml, name);
}

/** Find a given attribute and returns it's const char * value, 
or NULL if not found. */
const char * xml_findattribute_cstrcstr(Xml * xml, const char * cname) {  
  STR * res = xml_findattribute_cstrstr(xml, cname);
  if(res) return str_cstr(res);
  return NULL;
}

/** Finds the first child with the given tag name. */
Xml * xml_findchild_str(Xml * xml, STR * name) {
  return xml_eachchild(xml, findtag_each, name);
}

/** Finds the first child with the given tag name. */
Xml * xml_findchild_cstr(Xml * xml, const char * cname) {
  str_const(name, cname); // string constant for the name header
  return xml_findchild_str(xml, name);
}


/** Finds the first sibling with the given tag name. */
Xml * xml_findsibling_str(Xml * xml, STR * name) {
  if(!xml) return NULL;
  return xml_eachsibling(xml->sibling, findtag_each, name);
}

/** Finds the first sibling with the given tag name. */
Xml * xml_findsibling_cstr(Xml * xml, const char * cname) {
  str_const(name, cname); // string constant for the name header
  return xml_findsibling_str(xml, name);
}


/** Parser states */
enum Xmlstate_ {
  XMLSTATE_START,
  XMLSTATE_ENTITY,
  XMLSTATE_COMMENT,
  XMLSTATE_COMMENTSTART1,
  XMLSTATE_COMMENTSTART2,
  XMLSTATE_COMMENTEND1,
  XMLSTATE_COMMENTEND2,
  XMLSTATE_HEADER,
  XMLSTATE_HEADEREND,
  XMLSTATE_TAGOPEN,
  XMLSTATE_TAGBETWEEN,
  XMLSTATE_TAGCLOSE,
  XMLSTATE_ATTRNAME,
  XMLSTATE_ATTRVALUE,
  XMLSTATE_TEXT,
  XMLSTATE_ERROR,
  XMLSTATE_DONE,
  XMLSTATE_KEEP
};

/** Parser actions */
enum Xmlstate_ {
  XMLACT_NONE,
  XMLACT_NEWTAG,
  XMLACT_TAGDONE,
  XMLACT_KEEPTAG,
  XMLACT_KEEPVALUE,
  XMLACT_KEEPENTITY,
  XMLACT_ATTRDONE,
  XMLACT_ATTRDONE,
  XMLACT_ERROR
};


/**
* Xmlparser is an XML parser.
*/
struct Xmlparser_ {
  Xml         * result; /* Top level node. */
  Xml         * last;   /* Last parsed node */
  STR         * tag;    /* Tag name currently parsing */
  STR         * value;  /* Value currently parsing */
  STR         * entity; /* & entity currently parsing if any */
  int           state;  /* Current parser state */
};

/* Parsing table entry */
struct Xmlentry_ { 
  int     state;  /* State the parser must be in to trigger this entry. */
  char *  str;    /* Characters that trigger this entry. NULL Is catch-all. */
  int     action; /* Action to be taken. */
  int     result; /* Resulting state. Set to XML_STATE_KEEP to kep unchanged. */
};

typedef struct Xmlentry_ Xmlentry;

Xmlentry xmlparser_table[] = {
  { XMLSTATE_START    , "<"       , XMLACT_NONE   , XMLSTATE_TAGOPEN        } ,
  { XMLSTATE_START    , " \n\t\r" , XMLACT_NONE   , XMLSTATE_START          } ,
  { XMLSTATE_START    , NULL      , XMLACT_NONE   , XMLSTATE_ERROR          } ,

  { XMLSTATE_TAGOPEN  , "!"       , XMLACT_NONE   , XMLSTATE_COMMENTSTART1  } ,
  { XMLSTATE_TAGOPEN  , "?"       , XMLACT_NONE   , XMLSTATE_HEADER         } ,
  
  { XMLSTATE_HEADER   , "?"       , XMLACT_NONE   , XMLSTATE_HEADEREND      } ,
  { XMLSTATE_HEADER   , NULL      , XMLACT_NONE   , XMLSTATE_ERROR          } ,
  { XMLSTATE_HEADEREND, "?"       , XMLACT_NONE   , XMLSTATE_HEADEREND      } ,
  { XMLSTATE_HEADEREND, ">"       , XMLACT_NONE   , XMLSTATE_ERROR          } ,
  
  { XMLSTATE_COMMENTSTART1, "-"   , XMLACT_NONE   , XMLSTATE_COMMENTSTART2  } ,
  { XMLSTATE_COMMENTSTART1, NULL  , XMLACT_NONE   , XMLSTATE_ERROR          } ,
  
  { XMLSTATE_COMMENTSTART2, "-"   , XMLACT_NONE   , XMLSTATE_COMMENT        } ,
  { XMLSTATE_COMMENTSTART2, NULL  , XMLACT_NONE   , XMLSTATE_ERROR          } ,
  

}





Xmlparser * xmlparser_init(Xmlparser * self) {
  if(!self) return NULL;
  self->result = NULL;
  self->last   = NULL;
  self->tag    = str_empty;
  self->value  = str_empty;
  self->entity = NULL;
  self->state  = XML_STATE_START;
  return self;
}

Xmlparser * xmlparser_new() {
  Xmlparser * self = MEM_ALLOCATE(Xmlparser);
  return xmlparser_init(self);
}

#ifdef COMMENT_

// Callback called with the new input character, 
// returns new state to switch to.
typedef int (XmlStateAct)(Xmlparser * self, int ch);

int xmlstate_start(Xmlparser * self, int ch) {
  switch(ch) {
    case '<': return XML_STATE_TAGOPEN;
    default: // nothing;
  }
  return self->state;
} 

int xmlstate_entity(Xmlparser * self, int ch) {
  return self->state;
} 

int xmlstate_comment(Xmlparser * self, int ch) {
  return self->state;
} 

int xmlstate_heading(Xmlparser * self, int ch) {
  return self->state;
} 


int xmlstate_tagopen(Xmlparser * self, int ch) {
  switch(ch) {
    case '?': return XML_STATE_HEADING;
    case '!': return XML_STATE_COMMENT;
    default: // nothing;
  }
  return self->state;
} 

int xmlstate_tagbetween(Xmlparser * self, int ch) {
  return self->state;
} 

int xmlstate_tagclose(Xmlparser * self, int ch) {
  return self->state;
} 

int xmlstate_attrname(Xmlparser * self, int ch) {
  return self->state;
} 

int xmlstate_attrvalue(Xmlparser * self, int ch) {
  return self->state;
} 

int xmlstate_text(Xmlparser * self, int ch) {
  return self->state;
} 


/* Parse table. */
XmlStateAct * xmlparser_table[XML_STATE_DONE+1] = {
  xmlstate_start, 
  xmlstate_entity,
  xmlstate_comment,
  xmlstate_heading,
  xmlstate_tagopen,
  xmlstate_tagbetween,
  xmlstate_tagclose,
  xmlstate_attrname,
  xmlstate_attrvalue,
  NULL,
  NULL,
  NULL,
};
*/

/** Parses a single character, changing the state of the parser and it's results 
as it goes. */
Xmlparser * xmlparser_parsech(Xmlparser * self, int ch) {
  int res;
  XmlStateAct * act = xmlparser_table[self->state];
  if(!act) return NULL;
  res = act(self, ch);
  if((res < 0) || (res > XML_STATE_DONE)) return NULL;
  self->state = res;
  return self;
}

#endif

