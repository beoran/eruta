
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
  child->parent = me;
  
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



/**
 * xml_print_element_names:
 * @a_node: the initial xml node to consider.
 *
 * Prints the names of the all the xml elements
 * that are siblings or children of a given xml node.
 */
void bxml_print_element_names(Bxml * node) {
  Bxml * now = NULL;
  static int level = 0;
  for (now = node; now; now = now->sibling) {
    if (now->kind == BXML_TAG) {
      printf("node type: Element, %d, name: %s\n", level, now->name);
    } else {
      printf("node type: %d, %d, name: %s\n", node->kind, level, now->name);
    }
    level++;
    bxml_print_element_names(now->child);
    level--;
  }
}

/**
* bxml_print_all_attributes: a debugging function to print all xml attributes
*/
void bxml_print_all_attributes(Bxml * node) {
  BxmlAttribute * attr;
  printf("Node a%s attributes:", node->name);
  for(attr = node->attributes ; attr ; attr = attr->next) {
    printf(" %s=%s", attr->name, attr->value);
  }
  puts("");
}


int bxml_kind_is(Bxml * me, int kind) {
  if (!me) return 0;
  if (kind < 1) return !0;
  return (me->kind == kind);
}

int bxml_name_is(Bxml * me, char * name) {
  if (!name) return !0;
  if (!me) return 0;
  if (!me->name) return 0;
  return (strcmp(me->name, name) == 0);
}

int bxml_match_name_and_kind(Bxml * me, char * name, int kind) {
  return (bxml_kind_is(me, kind) && bxml_name_is(me, name));
}


/** Finds a sibling node. If name is not null
or type is strictly positive, return matching nodes.
Also searches node itself, so pass node->next if you don't want that.*/
Bxml * bxml_find_next_kind(Bxml * node, const char * name, int kind) {
  Bxml * now;
  for (now = node; now; now = now->sibling) {
    if (bxml_match_name_and_kind(now, (char *)name, kind)) {
      return now;
    }
  }
  return NULL;
}

/** Shorthand for bxml_find_next_type(node, name, BXML_TAG */
Bxml * bxml_find_next(Bxml * node, const char * name) {
  return bxml_find_next_kind(node, name, BXML_TAG);
}

/** Nonrecursively finds a BXML child node with the given name, 
 *  and kind including self. */
Bxml *  bxml_find_child_kind(Bxml * node, const char * name, int kind) {
  if (!node) return NULL;
  return bxml_find_next_kind(node->child, name, kind);
}

/** Nonrecursively finds a BXML child node with the given name, including self. */
Bxml *  bxml_find_child(Bxml * node, const char * name) {
  return bxml_find_child_kind(node, name, BXML_TAG);
}


/** Finds a child node with the given paths, pass null as last one  */
Bxml * bxml_find_child_deep_va(Bxml * node, va_list args) {
  const char * name = NULL;
  Bxml * res     = NULL;
  Bxml * aid     = node;
  for(name = va_arg(args, const char *); name && aid;
      name = va_arg(args, const char *)) {
    aid = bxml_find_child(aid, name);
  }
  res = aid;
  return res;
}

/** Finds a child node with the given paths, pass null as last one  */
Bxml * bxml_find_child_deep(Bxml * node, ...) {
  Bxml * result;
  va_list args;
  va_start(args, node);
  result = bxml_find_child_deep_va(node, args);
  va_end(args);
  return result;
}


/** Gets an integer property from the node and stores it into result,
 * returns result if ok, NULL if not a correct integer */
long * bxml_get_attribute_long(Bxml * node, char * name, long * result) {
  long aid;
  char * prop =  bxml_get_attribute(node, name);
  if (!prop) return false;
  errno = 0;
  aid   = strtol(prop, NULL, 10);
  if (errno) { return NULL; } 
  (*result) = aid;
  return result;
}

/** Gets an double property from the node and stores it into result,
 * returns result if ok, NULL if not a correct integer */
double * bxml_get_attribute_double(Bxml * node, char * name, double * result) {
  double aid;
  char * prop =  bxml_get_attribute(node, name);
  if (!prop) return false;
  errno = 0;
  aid   = strtod(prop, NULL);
  if (errno) { return NULL; } 
  (*result) = aid;
  return result;
}


/* Returns a reference to the first text node child of node */
Bxml * bxml_get_text_tag_under(Bxml * node) {
  return bxml_find_child_kind(node, NULL, BXML_TEXT);
}


/** Returns a reference to the (first) text contents of an xml node.
* this looks in the child node for the text.
*/
char * bxml_get_text_under(Bxml * node) {
  Bxml * text_tag = bxml_get_text_tag_under(node);
  return text_tag->text;
}

/** Recursively outputs a debug rep of xml to file. */
int bxml_show_to(Bxml * xml, FILE * out, int depth) {
  int index;
  for (index = 0; index < depth; index++) {
    fprintf(out, "  ");
  }
  if (xml->kind == BXML_TAG) { 
    fprintf(out, ">%s", xml->name);
  } else if (xml->kind == BXML_TEXT) { 
    fprintf(out, ">#text: %s", xml->text);
  } else {
    fprintf(out, ">%s: %s", xml->name, xml->text);
  }
   
  if (xml->attributes) {
    BxmlAttribute * nattr = xml->attributes;
    fprintf(out, "(");

    while (nattr) {
      fprintf(out, " %s->%s", nattr->name, nattr->value);
      nattr = nattr->next;
    }
    fprintf(out, " )");
  }
  fprintf(out, "\n");
  
  if (xml->child) {
    bxml_show_to(xml->child, out, depth + 1);
  }
  if (xml->sibling) {
    bxml_show_to(xml->sibling, out, depth);
  }
  return depth;
}

