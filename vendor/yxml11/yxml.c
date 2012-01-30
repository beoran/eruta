/*
    yxml.c

    yoctoXML (yXML), a tiny XML reader/writer.

    Copyright (C) 2009 Alexander G. Karpov a.k.a. mrhx.

    Available under GPL-compatible modified BSD license.
    See LICENSE.txt or <http://www.fsf.org/licensing/licenses/> for details.

    Visit my web-site for updates and more software: http://codege.org/
    E-mail: admin@codege.org

    Date: 2009-08-12
    v1.1: 2009-08-24
*/
#include "yxml.h"
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

void yxml_free(yxml_t *xml) /* Free xml items */
{
    yxml_attr_t *a, *b;
    yxml_t *r, *p = xml;
    for (; p; p = r)
    {
        if (p->name) free(p->name); /* Free tag name, if any */
        if (p->details) yxml_free(p->details); /* Free details, if any */
        for (a = p->attrs; a; a = b) /* Free attributes, if any */
        {
            if (a->name) free(a->name); /* Free attribute name, if any */
            if (a->value) free(a->value); /* Free attribute value, if any */
            b = a->next;
            free(a); /* Free attribute */
        }
        r = p->next;
        free(p); /* Free xml item */
    }
}

yxml_t *yxml_read(const char *str, const char **pstr) /* Read xml items from string */
{
    const char *s, *p = str;
    yxml_t *x, *xml = NULL, *e = NULL;
    yxml_attr_t *a;
    int n, m;
    while (*p)
    {
        if (*p != '<') /* Text content */
        {
            for (s = p; *p && *p != '<'; ++p); /* Read text content */
            x = (yxml_t*) malloc(sizeof(yxml_t)); /* Malloc xml item */
            if (!x) { yxml_free(xml); *pstr = p; return NULL; } /* Memory error */
            x->details = x->next = NULL;
            x->attrs = NULL;
            if (e) e->next = x; else xml = x; /* Append xml item */
            e = x;
            x->name = (char*) malloc((n = p-s)+1); /* Malloc tag name */
            if (!x->name) { yxml_free(xml); *pstr = p; return NULL; } /* Memory error */
            memcpy(x->name, s, n);
            x->name[n] = 0;
            x->len = -n; /* Negative length means it's a text value instead of tag name */
            continue; /* Go to begin */
        }
        if (p[1] == '/') { *pstr = p; return xml; } /* End tag */
        else if (p[1] == '!' || p[1] == '?') /* Skip <! .. > and <? .. > */
        {
            for (p += 2; *p && *p != '>'; ++p);
            if (!*p) { yxml_free(xml); *pstr = p; return NULL; } /* Syntax error */
            ++p;
            continue; /* Go to begin */
        }
        for (s = ++p; isalnum(*p) || *p == '_'; ++p); /* Skip < and read tag name */
        x = (yxml_t*) malloc(sizeof(yxml_t)); /* Malloc xml item */
        if (!x) { yxml_free(xml); *pstr = p; return NULL; } /* Memory error */
        x->details = x->next = NULL;
        x->attrs = NULL;
        if (e) e->next = x; else xml = x; /* Append xml item */
        e = x;
        x->name = NULL;
        if (!(n = p-s)) { yxml_free(xml); *pstr = p; return NULL; } /* Syntax error */
        x->name = (char*) malloc(n+1); /* Malloc tag name */
        if (!x->name) { yxml_free(xml); *pstr = p; return NULL; } /* Memory error */
        memcpy(x->name, s, n);
        x->name[n] = 0;
        x->len = n;
        while (isspace(*p)) ++p; /* Skip spaces */
        while (*p != '>' && !(*p == '/' && p[1] == '>')) /* Read attributes */
        {
            for (s = p; isalnum(*p) || *p == '_' || *p == ':' || *p == '-'; ++p); /* Read attribute name */
            a = (yxml_attr_t*) malloc(sizeof(yxml_attr_t)); /* Malloc attribute */
            if (!a) { yxml_free(xml); *pstr = p; return NULL; } /* Memory error */
            a->value = a->name = NULL;
            a->next = x->attrs; /* Insert attribute */
            x->attrs = a;
            if (!(m = p-s)) { yxml_free(xml); *pstr = p; return NULL; } /* Syntax error */
            a->name = (char*) malloc(m+1); /* Malloc attribute name */
            if (!a->name) { yxml_free(xml); *pstr = p; return NULL; } /* Memory error */
            memcpy(a->name, s, m);
            a->name[m] = 0;
            a->name_len = m;
            while (isspace(*p)) ++p; /* Skip spaces */
            if (*p != '=') { yxml_free(xml); *pstr = p; return NULL; } /* Syntax error */
            for (++p; isspace(*p); ++p); /* Skip = and spaces */
            if (*p != '"' && *p != '\'') { yxml_free(xml); *pstr = p; return NULL; } /* Syntax error */
            for (s = p++; *p && *p != *s; ++p); /* Read quoted value */
            if (!*p) { yxml_free(xml); *pstr = p; return NULL; } /* Syntax error */
            m = p++ - ++s; /* Skip first quote and end quote */
            a->value = (char*) malloc(m+1); /* Malloc attribute value */
            if (!a->value) { yxml_free(xml); *pstr = p; return NULL; } /* Memory error */
            memcpy(a->value, s, m);
            a->value[m] = 0;
            a->value_len = m;
            while (isspace(*p)) ++p; /* Skip spaces */
        }
        if (*p == '>') /* Read details */
        {
            ++p; /* Skip > */
            x->details = yxml_read(p, &p);
            if (*p == '<' && p[1] == '/' && !strncmp(p+2, x->name, n) && p[n+2] == '>')
            {
                p += n+3; /* Skip end tag </tag> */
            }
            else { yxml_free(xml); *pstr = p; return NULL; } /* Syntax error */
        }
        else p += 2; /* Skip /> */
    }
    *pstr = p;
    return xml;
}

int yxml_length(const yxml_t *xml) /* Get length of string data */
{
    int len = 0;
    const yxml_t *p = xml;
    const yxml_attr_t *a;
    for (; p; p = p->next)
        if (p->len < 0) len -= p->len;
        else
        {
            len += 4 + p->len;
            for (a = p->attrs; a; a = a->next) len += 4 + a->name_len + a->value_len;
            if (p->details) len += 1 + p->len + yxml_length(p->details);
        }
    return len;
}

char *yxml_write_buf(const yxml_t *xml, char *str) /* Write XML items to string buffer */
{
    char *s = str;
    const yxml_t *p = xml;
    const yxml_attr_t *a;
    for (; p; p = p->next)
        if (p->len < 0)
        {
            memcpy(s, p->name, -p->len);
            s -= p->len;
        }
        else
        {
            *s++ = '<';
            memcpy(s, p->name, p->len); /* Write tag name */
            s += p->len;
            for (a = p->attrs; a; a = a->next) /* Write attributes */
            {
                *s++ = ' ';
                memcpy(s, a->name, a->name_len); /* Write attribute name */
                s += a->name_len;
                *s++ = '=';
                *s++ = '"';
                memcpy(s, a->value, a->value_len); /* Write value of attribute */
                s += a->value_len;
                *s++ = '"';
            }
            if (p->details) /* Write details, if any */
            {
                *s++ = '>';
                s = yxml_write_buf(p->details, s);
                *s++ = '<';
                *s++ = '/';
                memcpy(s, p->name, p->len); /* Write end tag */
                s += p->len;
                *s++ = '>';
            }
            else
            {
                *s++ = ' ';
                *s++ = '/';
                *s++ = '>';
            }
        }
    return s; /* Return current pointer */
}

char *yxml_write(const yxml_t *xml, int *plen) /* Write XML items to new string */
{
    char *str = (char*) malloc((*plen = yxml_length(xml)) + 1); /* Malloc all needed memory */
    if (!str) return NULL; /* Memory error */
    *yxml_write_buf(xml, str) = 0; /* Append zero character */
    return str; /* Return new string with XML */
}

void yxml_view(const yxml_t *xml) /* View XML info */
{
    const yxml_t *p = xml;
    const yxml_attr_t *a;
    if (p)
        for (; p; p = p->next)
            if (p->len < 0) printf("%s(%i)\n", p->name?p->name:"NULL", -p->len);
            else
            {
                printf("<%s(%i)", p->name?p->name:"NULL", p->len);
                for (a = p->attrs; a; a = a->next)
                    printf(" %s(%i)=\"%s\"(%i)", a->name?a->name:"NULL", a->name_len, a->value?a->value:"NULL", a->value_len);
                if (p->details)
                {
                    printf(">\n");
                    yxml_view(p->details);
                    printf("</%s>\n", p->name?p->name:"NULL");
                }
                else printf(" />\n");
            }
    else printf("NULL\n");
}

yxml_t *yxml_find(yxml_t *xml, const char *tagname) /* Find xml item */
{
    int len = strlen(tagname);
    yxml_t *p = xml;
    for (; p; p = p->next)
        if (len == p->len && !strcmp(tagname, p->name)) return p;
    return NULL;
}

yxml_attr_t *yxml_find_attr(yxml_t *xml, const char *attrname) /* Find attribute */
{
    int len = strlen(attrname);
    yxml_attr_t *p = xml->attrs;
    for (; p; p = p->next)
        if (len == p->name_len && !strcmp(attrname, p->name)) return p;
    return NULL;
}

yxml_t *yxml_read_file(const char *filepath, int *plen) /* Read xml items from file */
{
    yxml_t *xml = NULL;
    char *buf;
    const char *pstr;
    long len;
    FILE *f = fopen(filepath, "rb");
    if (f)
    {
        if (!fseek(f, 0, SEEK_END))
        {
            if (-1 != (len = ftell(f)))
            {
                if (!fseek(f, 0, SEEK_SET))
                {
                    if (buf = (char*) malloc(len+1))
                    {
                        if (1 == fread(buf, len, 1, f))
                        {
                            buf[len] = 0;
                            xml = yxml_read(buf, &pstr);
                            if (!pstr || *pstr) { yxml_free(xml); xml = NULL; }
                        }
                        free(buf);
                    }
                }
            }
        }
        fclose(f);
    }
    if (plen) *plen = len;
    return xml;
}

/* End of file */

