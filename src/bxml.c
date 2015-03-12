
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include "bxml.h"
#include "ses.h"

#define BXML_WS " \t\n\r"

/* Strdup isn't ANSI C, just posix... :p so we need our own local version.*/
static char * bxml_strdup(const char *str) {
    char * res = malloc(strlen(str) + 1);
    if(res) { strcpy(res, str); }
    return res;
}

/* Strdup isn't ANSI C, just posix... :p so we need our own local version.*/
static char * bxml_strdup_size(const char *str, int size) {
    char * res = malloc(size + 1);
    if(res) { 
      if (size < 0) {
        size = strlen(str);
      }
      strncpy(res, str, (size_t) size); 
      res[size] = '\0';
    }
    return res;
}


/* Initializes a bxml attributes, sets no relationships! */
BxmlAttribute * 
bxmlattribute_init_size(BxmlAttribute * me, char * name, int namesize, char * value, int valuesize) {
  if (!me) return NULL;
  me->name  = bxml_strdup_size(name, namesize);
  if (!me->name) return NULL;
  me->value = bxml_strdup_size(value, valuesize);
  if (!me->value) {
    free(me->name);
    me->name = NULL;
    return NULL;
  }
  me->next  = NULL;
  return me;
}

/* Initializes a bxml attributes, sets no relationships! */
BxmlAttribute * 
bxmlattribute_init(BxmlAttribute * me, char * name, char * value) {
  return bxmlattribute_init_size(me, name, -1, value, -1);
}

/* Sets the value of an attribute. */
BxmlAttribute * bxmlattribute_set_value_size(BxmlAttribute * me, char * value, int size) {
  if (!me) return NULL;
  free(me->value);
  me->value = bxml_strdup_size(value, size);
  if (!me->value) return NULL;
  return me;
}

BxmlAttribute * bxmlattribute_set_value(BxmlAttribute * me, char * value) {
  return bxmlattribute_set_value_size(me, value, -1);
}

/* Cleans up an attribute. Does nothing with linked attributes! */
void bxmlattribute_done(BxmlAttribute * me) {
  if (!me) return;
  free(me->value);
  free(me->name);
  me->value = NULL;
  me->name  = NULL;
  me->next  = NULL;
}

/* Frees a bxmlattribute without doing anything to apotential next one. */
void bxmlattribute_free_one(BxmlAttribute * me) {
  bxmlattribute_done(me);
  free(me);
}

/* Frees a linked list of bxml attributes */
void bxmlattribute_free_list(BxmlAttribute * me) {
  BxmlAttribute * now, * aid;
  now = me; 
  while (now) {
    aid = now->next;
    bxmlattribute_free_one(now);
    now = aid;
  }
}

/* Allocates a new bxmlattibute */
BxmlAttribute * bxmlattribute_alloc(void) {
  return calloc(sizeof(BxmlAttribute), 1);
}


BxmlAttribute * bxmlattribute_new_size(char * name, int namesize, char * value, int valuesize) {
  return bxmlattribute_init_size(bxmlattribute_alloc(), name, namesize, value, valuesize);
}


BxmlAttribute * bxmlattribute_new(char * name, char * value) {
  return bxmlattribute_init(bxmlattribute_alloc(), name, value);
}

/* Appends or insterts an existing attribute to the list at me.
 * returns you on sucess or NULL on failure.  */
BxmlAttribute * bxmlattribute_insert(BxmlAttribute * me, BxmlAttribute * you) {
  BxmlAttribute * aid;
  if (!me) return NULL;
  if (!you) return NULL;
  aid       = me->next;
  me->next  = you;
  you->next = aid;
  return you;
}

/* Inserts a new attribute to the list after me. */
BxmlAttribute * 
bxmlattribute_insert_new(BxmlAttribute * me, char * name, char * value) {
  return bxmlattribute_insert(me, bxmlattribute_new(name, value));
}

/* Inserts a new attribute to the list after me. */
BxmlAttribute * 
bxmlattribute_insert_new_size(BxmlAttribute * me, char * name, int namesize, char * value, int valuesize) {
  return bxmlattribute_insert(me, bxmlattribute_new_size(name, namesize, value, valuesize));
}


/* Initializes a Bxml me. Does not set up any relationships nor 
 * call strdup!  */
Bxml * bxml_init_all(Bxml * me, int kind, char * name, char * text) {
  if(!me) return NULL;
  me->parent      = NULL;  
  me->sibling     = NULL;
  me->child       = NULL;
  me->attributes  = NULL;
  me->kind        = kind;
  me->name        = name;
  me->text        = text;
  return me;
}

/* Initializes a bxml me based on it's kind. */
Bxml * bxml_init_size_va(Bxml * me, int kind, int size, va_list args) {
  char * name = NULL;
  char * text = NULL;
  switch(kind) {
    case BXML_ROOT:
      return bxml_init_all(me, kind, NULL, NULL);
    case BXML_TAG: 
      name = bxml_strdup_size(va_arg(args, char *), size);
      if (!name) return NULL;
      return bxml_init_all(me, kind, name, NULL);
    case BXML_TEXT:
      text = bxml_strdup_size(va_arg(args, char *), size);
      if (!text) return NULL;
      return bxml_init_all(me, kind, NULL, text);
    case BXML_COMMENT:
      text = bxml_strdup_size(va_arg(args, char *), size);
      if (!text) return NULL;
      return bxml_init_all(me, kind, NULL, text);      
    default:
      return NULL;
  }
}

/* Initializes a bxml me based on it's kind. */
Bxml * bxml_init_va(Bxml * me, int kind, va_list args) {
  return bxml_init_size_va(me, kind, -1, args);
}
  
 
/* Allocates a new Bxml node */
Bxml * bxml_alloc() {
  return calloc(sizeof(Bxml), 1);
}

/* Allocates and initialzes a new Bxml node. */
Bxml * bxml_new_size_va(int kind, int size, va_list args) {
  return bxml_init_size_va(bxml_alloc(), kind, size, args);
}

/* Allocates and initialzes a new Bxml node. */
Bxml * bxml_new_va(int kind, va_list args) {
  return bxml_init_size_va(bxml_alloc(), kind, -1, args);
}

/** Appends text to the text of this tag */
Bxml * bxml_append_buf(Bxml * me, char * buf, int bufsize) {
  if (!me->text) {
    me->text = bxml_strdup_size(buf, bufsize);
    if (!me->text) return NULL;
  } else {
    int oldsize = strlen(me->text);
    me->text = realloc(me->text, oldsize + bufsize + 1);
    strncpy(me->text + oldsize, buf, bufsize);
    me->text[oldsize+bufsize] = '\0';
  }
  return me;
}

/* Removes a child from parent. Does NOT free it, but returns it on success, 
 * or NULL if no such child. Comparison is by address. */
Bxml * bxml_remove_child(Bxml * parent, Bxml * child) {
  Bxml * index, * previous;
  /* First child as special case. */
  if (parent->child == child) {
    parent->child = parent->child->sibling;
    return child;
  }
  
  for (index = parent->child; index; index = index->sibling) {
    /* next up is the child... */
    if (index->sibling == child) {
      /* cut it from the linked list */
      index->sibling = index->sibling->sibling;
      return child;
    }
  }
  return NULL;
}
 


/* Merges two text tags and returns a newly allocated 
 * test tag on success, or NULL on failure. 
 */
Bxml * bxml_merge_text(Bxml * text1, Bxml * text2) {
   Swis buffer;
   Bxml * result;

   if (!text1) return NULL;
   if (!text2) return NULL;
   if (text1->kind != BXML_TEXT) return NULL;
   if (text2->kind != BXML_TEXT) return NULL;
   
   if (!swis_init_cstr(&buffer, text1->text)) return NULL;
   
   if (!swis_append_cstr(&buffer, text2->text)) {
      swis_free(&buffer);
      return NULL;
   }
   
   result = bxml_new_size(BXML_TEXT, buffer.size, buffer.text);
   swis_free(&buffer);
   return result;
} 

/* Merges two adjacent text tag, frees them and replaces them by the 
 * merged tag. 
 */
Bxml * bxml_merge_adjacent_text(Bxml * text1, Bxml * text2) {
  Bxml * parent;
  Bxml * merged = bxml_merge_text(text1, text2);
  if (!merged) return NULL;
  parent = text1->parent;
  bxml_remove_child(parent, text1);
  bxml_free(text1);
  bxml_remove_child(parent, text2);
  bxml_free(text2);
  bxml_add_child(parent, merged);
  return merged;
}
 

/* Allocates and initialzes a new Bxml node. */
Bxml * bxml_new(int kind, ...) {
  Bxml * result;
  va_list args;
  va_start(args, kind);
  result = bxml_new_va(kind, args);
  va_end(args);
  return result;
}


/* Allocates and initialzes a new Bxml node. */
Bxml * bxml_new_size(int kind, int size, ...) {
  Bxml * result;
  va_list args;
  va_start(args, size);
  result = bxml_new_size_va(kind, size, args);
  va_end(args);
  return result;
}


/* Cleans up a Bxml  node, freeing the children recursively. */
Bxml * bxml_done(Bxml * me) {
  Bxml * aid  = me->child;
  Bxml * next = NULL;
  while(aid) { 
    next = aid->sibling; // get next, since aid will be destroyed.
    bxml_free(aid);   // free first child
    aid  = next;      // move to next child. 
  }
  bxmlattribute_free_list(me->attributes);
  free(me->name);
  free(me->text);
  return me;
}

/* Frees this node and cleans up its children recursively. returns NULL. */
Bxml * bxml_free(Bxml * me) {
  if(!me) return NULL;
  bxml_done(me);
  free(me);
  return NULL;
}



/* Sets node siling to be the sibling node of this node, possibly inserting 
* between an existing sibling.
* returns you if OK, NULL on error.
**/
Bxml * bxml_add_sibling(Bxml * me, Bxml * you) {
  Bxml * old_sibling;
  if(!me)  return NULL;
  if(!you) return NULL;
  old_sibling = me->sibling;
  /* Ensure proper insertion. */
  you->sibling   = old_sibling;
  me->sibling    = you;
  return you;
}

/* Gets the last sibing of me, or NULL if no sibling. */
Bxml * bxml_get_last_sibling(Bxml * me) {
  Bxml * aid;
  if (!me) return NULL;
  for (aid = me; aid->sibling ; aid = aid->sibling);
  return aid;
}

/* Adds a sibling to me, at the end of the sibling list. */
Bxml * bxml_append_sibling(Bxml * me, Bxml * you) {
  Bxml * index;
  if (!me) return NULL;
  if (!you) return NULL;
  return bxml_add_sibling(bxml_get_last_sibling(me), you);
}

/* Returns the pos-th sibling of me */
Bxml * bxml_get_sibling_at(Bxml * me, int pos) {
  int    index = 0;
  Bxml * aid;
  if (!me) return NULL;  
  for (aid = me; aid; aid = aid->sibling) {
    if (index == pos) return aid;
    index++;
  }
  return NULL;
}

/* Returns the pos-th child of me */
Bxml * bxml_get_child_at(Bxml * me, int pos) {
  if (!me) return NULL;
  return bxml_get_sibling_at(me->child, pos);
}

/* Gets the last sibing of me, or NULL if no sibling. */
Bxml * bxml_get_last_child(Bxml * me) {
  Bxml * aid;
  if (!me) return NULL;
  if (!me->child) return NULL;
  return bxml_get_last_sibling(me->child);
}


/* Adds attr as an attribute to the Bxml tag me. 
 * Returns you if ok, NULL on error.
*/
BxmlAttribute * bxml_add_attribute(Bxml * me, BxmlAttribute * you) {
  Bxml * oldnext;
  if (!me)   return NULL;
  if (!you)  return NULL;
  
  if (!me->last_attr) {
    me->attributes = you;
    me->last_attr  = you;
  } else {
    bxmlattribute_insert(me->last_attr, you);
  }
  return you;
}

/** Makes a new attribute and adds it to me. */
BxmlAttribute * bxml_new_attribute_size(Bxml * me, char * name, int namesize, char * value, int valuesize) {
  return bxml_add_attribute(me, bxmlattribute_new_size(name, namesize, value, valuesize));
} 


/** Makes a new attribute and adds it to me. */
BxmlAttribute * bxml_new_attribute(Bxml * me, char * name, char * value) {
  return bxml_add_attribute(me, bxmlattribute_new(name, value));
} 

/** Gets the attribute of me with the given name, or NULL if no such attribute. */
BxmlAttribute * bxml_get_attribute_pointer(Bxml * me, char * name) {
  BxmlAttribute * index;
  for(index = me->attributes; index ; index = index->next) {
    if (strcmp(name, index->name) == 0) {
      return index;
    }
  }
  return NULL;
}

/** Gets the value of the attribute of me with the given name, or NULL if no 
 * such attribute. */
char * bxml_get_attribute(Bxml * me, char * name) {
  BxmlAttribute * result;
  result = bxml_get_attribute_pointer(me, name);
  return result ? result->value : NULL;
}

/** Sets the attribute value of the given name. If the attribte already existed,
 * it will be overwritten, otherwise it will be made new. */
BxmlAttribute * bxml_set_attribute(Bxml * me, char * name, char * value) {
  BxmlAttribute * attribute;
  attribute = bxml_get_attribute_pointer(me, name);
  if (attribute) {
    return bxmlattribute_set_value(attribute, value);
  }
  return bxml_new_attribute(me, name, value);
} 

/* Adds child as a child node of the Bxml tag me. 
* Ensures that any siblings are connected correctly as well.
* Returns child if OK, NULL on error.
*/
Bxml * bxml_add_child(Bxml * me, Bxml * child) {
  if (!me)  return NULL;
  if (!child) return NULL;
  
  if (!me->child) {
    me->child = child;
    return child;
  } else {
    return bxml_append_sibling(me->child, child);
  }
}

Bxml * bxml_new_child_size_va(Bxml * me, int kind, int size, va_list args) {
  Bxml * child;
  child = bxml_new_size_va(kind, size, args);
  if (!bxml_add_child(me, child)) {
    bxml_free(child);
    return NULL;
  }
  return child;
}


Bxml * bxml_new_child_va(Bxml * me, int kind, va_list args) {
  return bxml_new_child_size_va(me, kind, -1, args);
}

/* Adds a new child initialized depending on kind. 
 * Text nodes should pass the text of the node, 
 * tags should pass the name of the tag. 
 */
Bxml * bxml_new_child_size(Bxml * me, int kind, int size, ...) {
  Bxml * result;
  va_list args;
  va_start(args, size);
  result = bxml_new_child_size_va(me, kind, size, args);
  va_end(args);
  return result;
}


/* Adds a new child initialized depending on kind. 
 * Text nodes should pass the text of the node, 
 * tags should pass the name of the tag. 
 */
Bxml * bxml_new_child(Bxml * me, int kind, ...) {
  Bxml * result;
  va_list args;
  va_start(args, kind);
  result = bxml_new_child_va(me, kind, args);
  va_end(args);
  return result;
}


#ifdef COMMENT_


/* Pushes a state on the parser's state stack. Returns state if 
OK, negative on error. */
BxmlState Bxmlparser_push(BxmlParser * me, BxmlState state) {
  me->sp++;
  // stack overflow
  if( me->sp >= BxmlPARSER_STACKSIZE) {
    return bxml_STATE_STACKERROR;
  }
  me->stack[me->sp] = state;
  return state;
}

/* Returns the state of the top of the stack. 
Assumes the stack pointer is safe.
*/
BxmlState Bxmlparser_peek(BxmlParser * me) {
  return me->stack[me->sp];
}


/* Pops a state from the parser's state stack. Returns negative if
an error (e.g. underflow) occurs. Returns the new stack top youwise. */
BxmlState Bxmlparser_pop(BxmlParser * me) {
  me->sp--;
  // Stack underflow.
  if(me->sp < 0) {
    return bxml_STATE_STACKERROR;
  }
  return me->stack[me->sp];
}

/* Sets the top of the state stack to a state and returns the NEW top */
BxmlState Bxmlparser_put(BxmlParser * me, BxmlState state) {
  BxmlState old = me->stack[me->sp];
  return me->stack[me->sp] = state;
}


/* Stack and parsing helper macros. */
#define PUSH(P, S) Bxmlparser_push(P, S)
#define POP(P) Bxmlparser_pop(P)
#define PEEK(P) Bxmlparser_peek(P)
#define PUT(P, S) Bxmlparser_put(P, S)

/* Store me->tag in R, and return S */
#define PRET(me, R, S) do { (*(R)) = (me)->tag ; return (S); } while(0)   


/* Initializes an Bxml parser.
BxmlParser * 
Bxmlparser_init(BxmlParser * me, USTR * data) {
  if(!me) return NULL;
  me->line      = 1;
  me->col       = 1;
  me->index     = 0;
  me->now       = 0;
  me->sp        = -1;
  // Push a done state on the stack. 
  // If the parser reaches this, parsing is done. 
  PUSH(me, bxml_STATE_DONE);
  // Then push a start state.
  PUSH(me, bxml_STATE_START);
  me->tag       = NULL;
  me->root      = NULL;
  me->buffer    = data;
  return me;
}
*/


/* Parses when in the start state.  */
BxmlState
Bxmlparser_parse_start(BxmlParser * me, int ch) {
  /* We should get a < first  */
  if (al_ustr_has_prefix_cstr()  == '<') { 
    /* Go on to parse the tag, preparing it for use. */
    
    return PUSH(me, bxml_STATE_TAGSTART); 
  } else if (isblank(ch)) { 
  /* Normally we should get a < character right away,
  but be lenient and skip spaces. So do nothing. */
    return PEEK(me);
  } 
  /* youwise it's a parse error. */
  return PUSH(me, bxml_STATE_ERROR);
}

/* Parses when in the tag start state. This is the beginning of the   
tag name, to deal with comments, <![CDATA[ and <? declarations. */
BxmlState
Bxmlparser_parse_tagstart(BxmlParser * me, int ch) {
  /*    
  * If there's a ? at the beginning, it's a declaration. 
  * If there is a ! at the start it should a comment
    ( may not be so if it's a <!ENTITY>)  
  * If it's alphabetical, it's OK, the tag name begins.
  * / means it's an end tag
  * Anything else is an error.
  */
  if (ch == '?')       {
      return PUSH(me, bxml_STATE_DECLARE);
  } else if(ch == '!') {
      return PUSH(me, bxml_STATE_ENTITY);
  } else if(ch == '/') {
      return PUT(me, bxml_STATE_ENDTAG);
  } else if(isalpha(ch)) {
      // here we really have a new tag. Create it. 
      Bxml * newtag = bxml_new(bxml_TAG, me->tag);
      if(!newtag) { 
        return PUSH(me, bxml_STATE_MEMERROR);
      }
      /* Initialize root tag if not set yet. */
      if(!me->root) {
        me->root = newtag; 
      }
      me->tag = newtag;
      ustr_appendch(me->tag->name, ch);
      return PUT(me, bxml_STATE_TAGNAME);
  } else {
      /* Anything else is a parse error at the start of the tag name.  */
      return PUSH(me, bxml_STATE_ERROR);
  }
  /* If we get here it's inside of the tag */
}

/* Parses when in the tag name state. This is the rest of the the tag name. */
BxmlState
Bxmlparser_parse_tagname(BxmlParser * me, int ch) {
/*
  Possible cases:
  * If there is a > the tag is done, and we go on to text parsing mode
    for the possible contents of the tag.
  * If there is a / the tag SHOULD be ending,
  * If there are alphanumerical characters, continue with the tag name.
  * If there is whitespace, attribute list begins. 
*/
  if(isalnum(ch)) {
    ustr_appendch(me->tag->name, ch);
    return PEEK(state);
  } else if (isspace(ch)) {
    return SWAP(me, bxml_STATE_ATTRLIST);
  } else if (ch == '/') {
    return SWAP(me, bxml_STATE_TAGEND);
  } else if (ch == '>') {
    /* a > after the tag name (without having seen a /, 
      means that a text node wil follow as the child of the current node. 
      Prepare that text node.
    */
    Bxml * newtag = bxml_new(bxml_TEXT, me->tag);
    if(!newtag) { 
      return PUSH(me, bxml_STATE_MEMERROR);
    }
    me->tag = newtag;
    return SWAP(me, bxml_STATE_TEXT);
  } else {
    /* Anything else is a parse error  */
    return PUSH(me, bxml_STATE_ERROR);
  }

}


/* Parses an attribute list */
BxmlState
Bxmlparser_parse_attrlist(BxmlParser * me, int ch) {
  return PUSH(me, bxml_STATE_ERROR);
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
    Bxml * attr = bxml_new(bxml_ATTR, me->tag);
    if(!bxml_addattribute(me->tag, attr)) {
      return return PUSH(me, bxml_STATE_MEMERROR);
    }
    ustr_appendch(me->attr->name, ch);
    return PUSH(state, bxml_STATE_ATTRNAME);
  } else if (isspace(ch)) {
    /* skip spaces. */
    return PEEK(me);
  } else if (ch == '/') {
    return SWAP(me, bxml_STATE_TAGEND);
  } else if (ch == '>') {
    /* a > after the tag name (without having seen a /, 
      means that a text node wil follow as the child of the current node. 
      Prepare that text node.
    */
    Bxml * newtag = bxml_new(bxml_TEXT, me->tag);
    if(!newtag) { 
      return PUSH(me, bxml_STATE_MEMERROR);
    }
    me->tag = newtag;
    return SWAP(me, bxml_STATE_TEXT);
  } else {
    /* Anything else is a parse error  */
    return PUSH(me, bxml_STATE_ERROR);
  }
}

BxmlState
Bxmlparser_parse_attrname(BxmlParser * me, int ch) {
  /* an attibute name continues until we find a isspace or an = character
  *  anything else isn't allowed.
  */
  if(isalnum(ch)) {
    ustr_appendch(me->attr->name, ch);
    return PEEK(state);
  } else if(isspace(ch)) { 
    /* attribute name is done, but we still need the = */
    return SWAP(state, bxml_STATE_NAMEEND);
  } else if(ch == '=')
    /* value will start from here */
    return SWAP(state, bxml_STATE_VALSTART);
  } else {
    return PUSH(me, bxml_STATE_ERROR);
  }
}


BxmlState
Bxmlparser_parse_nameend(BxmlParser * me, int ch) {
  /* attribute has ended, but we still need to skip spaces and look for the =
  */
  if(isspace(ch)) { 
    /* attribute name is done, but we still need the = */
    return PEEK(state);
  } else if(ch == '=')
    /* value will start from here */
    return SWAP(state, bxml_STATE_VALSTART);
  } else {
    return PUSH(me, bxml_STATE_ERROR);
  }
}

BxmlState
Bxmlparser_parse_valstart(BxmlParser * me) {
  /* we have had the = , now skip spaces until we get the " or '
  */
  if(isspace(ch)) { 
    /* We saw the =, but we still need the " */
    return PEEK(state);
  } else if((ch == '"') {
    /* Double quoted value will start from here. Store the quote too,  */
    ustr_appendch(me->attr->value, ch);
    return SWAP(state, bxml_STATE_DQVALUE);
  } else if((ch == '\'') {
    /* Single quoted value will start from here. Store the quote too,  */
    ustr_appendch(me->attr->value, ch);
    return SWAP(state, bxml_STATE_DQVALUE);
  } else {
    return PUSH(me, bxml_STATE_ERROR);
  }
}

/* parses a single or double quoted value as per quote. */
BxmlState
Bxmlparser_parse_value(BxmlParser * me, int ch, int quote) {
  /* copy data until we get the quote, but beware entities */
  if(ch == '&') { 
    /* Entity! */
    ustr_empty(state->buffer);
    return PUSH(state, bxml_STATE_ENTITY);
  } else if (ch == quote) {
    /* attribute is all done. POP! */    
    return POP(state);
  } else {
    /* Just store the value. */
    ustr_appendch(me->attr->value, ch);
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
BxmlState
Bxmlparser_parse_entity(BxmlParser * me, int ch, USTR * target) {
  /* copy data until we get the ending ;  */
  if(ch == ';') { 
    /* Entity is done */
    Silut match = silut_lsearchcstr(bxml_entity_lut, ustr_c(state->buffer));
    if(match) {
      ustr_appendch(target, match.integer);
    } else  { 
      int base = 10;
      int res  = 0;
      char * aid = ustr_c(me->buffer);
      if(aid[0] != '#') { 
        /* must be a # entity if not known as a named one */
        return PUSH(state, bxml_STATE_ERROR);
      }
      aid++;
      if(aid[0] == 'x') { aid++; base = 16; }
      res = strtol(aid, NULL, base);
      /* XXX: wildly ignore strtol failing :p */
      ustr_appendch(target, res);
    }
    ustr_empty(state->buffer);
    /* Return to previous state. */
    return POP(state);
  } else {
    /* Just store the value in the sschratch buffer . */
    ustr_appendch(me->buffer, ch);
  }
}



BxmlState
Bxmlparser_parse_text(BxmlParser * me, int ch, Bxml ** result) {
  // when parsing text we can either encounter anyou tag, an entity, or
  // anything else ,which goes into the text tag's value
  if(ch == '<') { 
    // New tag starting, sibling of this text node.
    Bxml * newtag = bxml_new(bxml_TAG, me->tag);
    if(!bxml_addsibling(me->tag, newtag)) {
      return PUSH(me, bxml_STATE_MEMERROR);
    }
    me->tag = newtag;
    return (me, bxml_STATE_TAGSTART);
  } else if(ch = '&') {
    return PUSH(state, bxml_STATE_ENTITY);
  }
  
  return PUSH(me, bxml_STATE_ERROR);
}

BxmlResult 
Bxmlparser_parse_entity (BxmlParser * me, int ch, Bxml ** result) {
  return PUSH(me, bxml_STATE_ERROR);
}

BxmlResult 
Bxmlparser_parse_comment (BxmlParser * me, int ch, Bxml ** result) {
  return PUSH(me, bxml_STATE_ERROR);
}


BxmlResult 
Bxmlparser_parse_declare (BxmlParser * me, int ch, Bxml ** result) {
  return PUSH(me, bxml_STATE_ERROR);
}

BxmlResult 
Bxmlparser_parse_processing(BxmlParser * me, int ch, Bxml ** result) {   
  return PUSH(me, bxml_STATE_ERROR);
}


BxmlResult 
Bxmlparser_parse_cdata (BxmlParser * me, int ch, Bxml ** result) {
  return PUSH(me, bxml_STATE_ERROR);
}


BxmlResult Bxmlparser_parse_dispatch(BxmlParser * me, int ch) { 
  // Handle the state.
  state          = PEEK(me);
  switch(state) {
    case bxml_STATE_START:
      return Bxmlparser_parse_start(me, ch, result);
    case bxml_STATE_TAGSTART:
      return Bxmlparser_parse_tagstart(me, ch, result);
    case bxml_STATE_TAGNAME:
      return Bxmlparser_parse_tagname(me, ch, result);
    case bxml_STATE_ATTRLIST:
      return Bxmlparser_parse_attrlist(me, ch, result);
    case bxml_STATE_ATTRSTART:
      return Bxmlparser_parse_attrstart(me, ch, result);
    case bxml_STATE_ATTRNAME:
      return Bxmlparser_parse_attrname(me, ch, result);
    case bxml_STATE_VALSTART:
      return Bxmlparser_parse_valstart(me, ch, result);
    case bxml_STATE_VALUE:
      return Bxmlparser_parse_value(me, ch, result);
    case bxml_STATE_TAGEND:
      return Bxmlparser_parse_tagend(me, ch, result);
    case bxml_STATE_TEXT:
      return Bxmlparser_parse_text(me, ch, result);
    case bxml_STATE_ENTITY:
      return Bxmlparser_parse_entity(me, ch, result);
    case bxml_STATE_COMMENT:
      return Bxmlparser_parse_comment(me, ch, result);
    case bxml_STATE_DECLARE:
      return Bxmlparser_parse_declare(me, ch, result);
    default: 
      return SET(me, bxml_ERROR);
  }
}

/* Makes the parser accept a single character. 
The tag or attribute that is currently being parsed, if available, 
is stored in result. youwise NULL is stored. Negative values indicate a 
parse or parser error. */
BxmlResult 
Bxmlparser_parse_core(BxmlParser * me, int ch, 
                      Bxml ** result) {
  BxmlState newstate;
  if(!me) return bxml_ERROR;
  me->lastchar = me->nowchar;
  me->nowchar  = ch;
  // Advance line and column of the parser as needed. 
  me->col++;
  if (ch == '\n') {
    me->line++; me->col = 1;
  }
  newstate = Bxmlparser_parse_dispatch(me, ch);
  if((newstate >= bxml_PARSE_OK) && (result)) {
    (*result) = me->tag;
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

/* Parses an XML string into a Bxml stree struct. 
Return NULL on failure. Uses a goto-based state switcher. */
Bxml * bxml_parse_strn(char * str, size_t size) {
  Bxml * root; /* root node  */
  Bxml * node; /* active node */
  int  index = 0; /* index of active character character */
  char now   = str[0];   /* active character */
  int aid;    /* aid count for various uses. */
  char * start;/* aid string for begin of string uses */
  char * stop; /* aid string for end of string uses */

  root = bxml_new(bxml_TAG, NULL);
  node = root;
  
  goto state_start;
  /* begin of states */
  state_start :
    /* skip whitespace, and then we should get a < to open the first tag or 
     <?xml declaration */
    STATE_SKIP_SET(str, now, index, size, bxml_SPACE_STR);
    /* Must get < now. */
    if(now != '<') return NULL;
    /* on to tag parsing state */
    STATE_NEXT(state_tag, str, now, index, size);
    
  state_tag: /* tag parsing state, must already have pased beginning < */
    node = bxml_new(bxml_TAG, NULL);  /* new node for the current tag. */
    
    STATE_SKIP_SET(str, now, index, size, bxml_SPACE_STR);
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


Bxml * bxml_parse_str(char * str) {
  return bxml_parse_strn(str, strlen(str));
}


Bxml * bxml_readfile(char * filename) {
  return NULL;
}


#endif
