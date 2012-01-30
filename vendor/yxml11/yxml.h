/*
    yxml.h

    yoctoXML (yXML), a tiny XML reader/writer.

    Copyright (C) 2009 Alexander G. Karpov a.k.a. mrhx.

    Available under GPL-compatible modified BSD license.
    See LICENSE.txt or <http://www.fsf.org/licensing/licenses/> for details.

    Visit my web-site for updates and more software: http://codege.org/
    E-mail: admin@codege.org

    Date: 2009-08-12
    v1.1: 2009-08-24
*/
#ifndef YOCTO_XML_H
#define YOCTO_XML_H

#define YXML_VERSION        "1.1"
#define YXML_VERSION_INT    11

typedef struct yxml_ yxml_t; /* XML item type */
typedef struct yxml_attr_ yxml_attr_t; /* XML attribute type */

struct yxml_ /* XML item structure */
{
    char *name; /* Tag name */
    int len; /* Length of tag name */
    yxml_attr_t *attrs; /* Attributes */
    yxml_t *details; /* Details (content) */
    yxml_t *next; /* Pointer to next XML item */
};

struct yxml_attr_ /* XML attribute structure */
{
    char *name; /* Attribute name */
    char *value; /* Value of attribute */
    int name_len; /* Length of name */
    int value_len; /* Length of value */
    yxml_attr_t *next; /* Address of next attribute */
};

yxml_t *yxml_read(const char *str, const char **pstr); /* Read xml items from string */
yxml_t *yxml_read_file(const char *filepath, int *plen); /* Read xml items from file */
char *yxml_write(const yxml_t *xml, int *plen); /* Write XML items to new string */
char *yxml_write_buf(const yxml_t *xml, char *str); /* Write XML items to string buffer */
int yxml_length(const yxml_t *xml); /* Get length of string data */
void yxml_free(yxml_t *xml); /* Free xml items */
void yxml_view(const yxml_t *xml); /* View XML info */
yxml_t *yxml_find(yxml_t *xml, const char *tagname); /* Find xml item */
yxml_attr_t *yxml_find_attr(yxml_t *xml, const char *attrname); /* Find attribute */

#endif

