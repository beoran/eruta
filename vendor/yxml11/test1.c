/*
    test1.c

    yXML simple test program.

    Copyright (C) 2009 Alexander G. Karpov a.k.a. mrhx.

    Visit my web-site for updates and more software: http://codege.org/
    E-mail: admin@codege.org

    Date: 2009-08-12
	v1.1: 2009-08-24
*/
#include <stdio.h>
#include <stdlib.h>
#include "yxml.h"

int main()
{
    const char *pstr;
    const char *str = "   <tag attr='value' attr2=\"value2 'x'\"><detail>simple text value</detail><detail /> <detail index=\"3\" /></tag><tag></tag><!-- comments --><b/>  ";
    yxml_t *xml = yxml_read(str, &pstr);
	int len;
    char *str1 = yxml_write(xml, &len);
    printf("IN : \"%s\"\n", str);
    printf("XML:\n");
    yxml_view(xml);
    printf("OUT (%i): \"%s\"\n", len, str1?str1:"NULL");
    if (pstr && !*pstr) printf("Wholly parsed\n"); else printf("Partially parsed\n");
    if (str1) free(str1);
    if (xml) yxml_free(xml);
    return 0;
}

/* End of file */

