/*
	test3.c

	yXML speed test program #2.

	Copyright (C) 2009 Alexander G. Karpov a.k.a. mrhx.

	Visit my web-site for updates and more software: http://codege.org/
	E-mail: admin@codege.org

	Date: 2009-08-13
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "yxml.h"

int main()
{
    const char *pstr;
	/* Initial short string */
    const char *str = "<tag attr1='aaa' attr2=\"bbbb\" attr3='ccccc' attr4='dddddd' attr5=\"eeeeeee\"><detail a='1' b='2' /></tag>";
    yxml_t *xml;
	clock_t t;
	int i, len = strlen(str), num = 100000;
    char *s, *str1 = (char*) malloc(len*num+1); /* Make a long string */
	if (!str1)
	{
		printf("No memory\n");
		return 1;
	}
	*str1 = 0;
	printf("Making a long string...\n");
	s = str1;
	for (i = 0; i < num; ++i)
	{
		strcat(s, str);
		s += len;
	}
	i = strlen(str1);
    printf("IN : string of %i chars (%i KB)\n", i, i/1024);
	t = clock();
	xml = yxml_read(str1, &pstr);
	t = clock() - t;
	printf("Time is %f secs. Speed of yxml_read is %f MB/sec\n", (double) t / CLOCKS_PER_SEC, (double)i/((double)t/CLOCKS_PER_SEC)/1024/1024 );
	if (pstr && !*pstr) printf("Wholly parsed\n"); else printf("Partially parsed\n");
	free(str1);
	t = clock();
	str1 = yxml_write(xml, &len);
	t = clock() - t;
	i = strlen(str1);
    printf("OUT : string of %i (%i) chars (%i KB)\n", len, i, i/1024);
	printf("Time is %f secs. Speed of yxml_write is %f MB/sec\n", (double) t / CLOCKS_PER_SEC, (double)i/((double)t/CLOCKS_PER_SEC)/1024/1024 );
    if (str1) free(str1);
    if (xml) yxml_free(xml);
    return 0;
}

/* End of file */

