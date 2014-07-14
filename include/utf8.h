#ifndef utf8_H_INCLUDED
#define utf8_H_INCLUDED

int utf8_decode_length(char * str);
int utf8_decode_one(char * str, long * result);
int utf8_next(char * str, char ** next, long * result);



#endif




