/* Xml helper functions that extend the functionality of libxml2.  */
#include "xml.h"
#include <string.h>

/* Exrra XML functions that exxtend libxml2's capabilities or
make it easier to use. */

/**
 * print_element_names:
 * @a_node: the initial xml node to consider.
 *
 * Prints the names of the all the xml elements
 * that are siblings or children of a given xml node.
 */
void print_element_names(xmlNode * node) {
  xmlNode *now = NULL;
  static int level = 0;
  for(now = node; now; now = now->next) {
    if (now->type == XML_ELEMENT_NODE) {
      printf("node type: Element, %d, name: %s\n", level, now->name);
    } else {
      printf("node type: %d, %d, name: %s\n", node->type, level, now->name);
    }
    level++;
    print_element_names(now->children);
    level--;
  }
}

/**
* print_all_attributes: a debugging function to print all xml attributes
*/
void print_all_attributes(xmlNode * node) {
 xmlAttrPtr attr;
  printf("Node a%s attributes:", node->name);
  for(attr = node->properties; attr; attr = attr->next) {
    printf(" %s=%s", attr->name, XML_GET_PROP(node, attr->name));
  }
  puts("");
}


/** Finds a sibling node. If name is not null
or type is strictly positive, return matching nodes.
Also searches node itself, so pass node->next if you don't want that.*/
xmlNode * xmlFindNextType(xmlNode * node, const char * name, int type) {
  xmlNode * now;
  for(now = node; now; now = now->next) {
    if ((type < 1) || (now->type == type)) {
      if ((!name) || (!strcmp((char *)now->name, name))) {
        return now;
      }
    }
  }
  return NULL;
}

/** shorthand for xmlFindNextType(node, name, XML_ELEMENT_NODE */
xmlNode * xmlFindNext(xmlNode * node, const char * name) {
  return xmlFindNextType(node, name, XML_ELEMENT_NODE);
}

/** nonrecursively finds a child node with the given name, including self. */
xmlNode * xmlFindChild(xmlNode * node, const char * name) {
  xmlNode * aid = xmlFindNext(node->children, name);
  if(aid) return aid;
  return NULL;
}

/** Finds a child node with the given paths, pass null as last one  */
xmlNode * xmlFindChildDeep(xmlNode * node, ...) {
  va_list args;
  const char * name = NULL;
  xmlNode * res     = NULL;
  xmlNode * aid     = node;
  va_start(args, node);
  for(name = va_arg(args, const char *); name && aid;
      name = va_arg(args, const char *)) {
    aid = xmlFindChild(aid, name);
  }
  res = aid;
  va_end(args);
  return res;
}

/** Gets an integer property from the libxml2 node,
or XML_BAD_VALUE if not found. */
int xmlGetPropInt(xmlNode * node, const char * name) {
  char * prop = XML_GET_PROP(node, name);
  if(!prop) return XML_BAD_VALUE;
  return atoi(prop);
}


/** Helper to load property values for a given property name. Pass in the
first propery tag.  */
char * xmlPropertyValue(xmlNode * firstprop, char * name) {
  xmlNode * xprop;
  for(xprop = firstprop ; xprop; xprop = xmlFindNext(xprop->next, "property")) {
    char * propname = XML_GET_PROP(xprop, "name");
    if(!propname) continue;
    if(!strcmp(propname, name)) return  XML_GET_PROP(xprop, "value");
  }
  return NULL;
}

/** Helper to load integer property values. */
int * xmlPropertyValueInt(xmlNode * firstprop, char * name, int * result) {
  char * aid = xmlPropertyValue(firstprop, name);
  if(!aid) return NULL;
  (*result) = atoi(aid);
  return result;
}  




