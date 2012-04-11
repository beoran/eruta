#ifndef XML_PROTO_H
#define XML_PROTO_H
/*
This file was autogenerated from src/xml.c
by bin/genproto
Please do not hand edit.
*/

/**
 * print_element_names:
 * @a_node: the initial xml node to consider.
 *
 * Prints the names of the all the xml elements
 * that are siblings or children of a given xml node.
 */
void print_element_names(xmlNode * node);

/**
* print_all_attributes: a debugging function to print all xml attributes
*/
void print_all_attributes(xmlNode * node);

/** Finds a sibling node. If name is not null
or type is strictly positive, return matching nodes.
Also searches node itself, so pass node->next if you don't want that.*/
xmlNode * xmlFindNextType(xmlNode * node, const char * name, int type);

/** shorthand for xmlFindNextType(node, name, XML_ELEMENT_NODE */
xmlNode * xmlFindNext(xmlNode * node, const char * name);

/** nonrecursively finds a child node with the given name, including self. */
xmlNode * xmlFindChild(xmlNode * node, const char * name);

/** Finds a child node with the given paths, pass null as last one  */
xmlNode * xmlFindChildDeep(xmlNode * node, ...);

#endif // XML_PROTO_H
