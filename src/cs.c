
#include "cs.h"

#include <stdlib.h>
#include <string.h>



/* CS contains functions that intend to replace string.h 
with functions that a) work with C strings unmodified, 
but B) are safe and easy to use. Something's gotta give, though, 
and to ensure that the operations go along safely, cs treats all strings as 
/immutable/. That is, for every operation, a new 
string will be alloated to store the result in. So you will need to use csfree
(which normally is just free) a lot. Also, due to the fact that 
and that's  the fact that C strings are \0 terminated, which means that many operations will be O(n) or O(n+m), since the string length will have to be 
checked for many operations, hence this library will be slow. 
Use bstrlib or allegro's strings for better performance. This library exists
only for simplicty reasons.
*/


/** Allocates a string with the given size and null-terminates it. 
* Note that csnew(0) will return a pointer to a string that is sipmly '\0';
*/
char * csnew(size_t size) {
  char * result = calloc(size + 1, 1);
  if(!result) return NULL;
  result[size]  = '\0';
  return result;
}

/** Frees an allocated string and returns NULL. */
char * csfree(char * str) {
  free(str);
  return NULL;
}

/** Returns the length of the string without the ending \0 character. 
Also returns 0 if str is NULL. */
size_t cslen(const char * str) {
  if(!str) return 0;
  return strlen(str);
}

/** Duplicates a null terminated string. 
Allocates memory. Returns NULL if allocation failed, 
or if scr is NULL too. */
char * csdup(const char * src) {
  size_t src_size;
  char * result, * dindex;
  if(!src) return NULL;
  src_size    = cslen(src);
  result      = csnew(src_size);
  if(!result) return NULL;
  dindex      = result;
  for(dindex  = result ; (*src) != '\0'; src++, dindex++) {
    (*dindex) = (*src);
  }
  return result;
};

/** Copies up to amount characters of a string if possible, 
and returns that as a new string.
Returns NULL if it couldn't copy or if src was NULL. */
char * csncpy(const char * src, size_t amount) {
  size_t src_size;
  char * result;
  int    index;
  if(!src) return NULL;
  src_size    = cslen(src);
  if(amount < src_size) src_size = amount;
  result      = csnew(src_size);
  if(!result) return NULL;
  for(index  = 0; index < src_size ; index++) {
    result[index] = src[index];
  }
  return result;
}

/** Concnatenates amount strings together, the sizes are known before and 
available in the size_list, which MUST also have amount sizes in it. 
size_total must contain the total size of the string. Thos is mostly a helper function for cscat and cscatn.
*/
char * cscatnsize(const char ** str_list, size_t * size_list, 
                  size_t amount, size_t size_total) {
  char   * result;
  int      index, listdex;
  if(!str_list)   return NULL;
  if(!size_list)  return NULL;
  result = csnew(size_total);
  if(!result)     return NULL;
  index  = 0;
  for(listdex = 0; listdex < amount; listdex++) {
    memcpy(result + index, str_list[listdex], size_list[listdex]);
    index += size_list[listdex];
  }
  return result;
}

/** Concantenates two strings together. Retuns the result as a newly allocated string. */
char * cscat(const char * one, const char * two) {
  size_t size_list[2], size_total = 0;
  const char * str_list[2] = { one, two };
  if(!one) return NULL;
  if(!two) return NULL;
  size_list[0]  = cslen(one);
  size_list[1]  = cslen(two);
  size_total    = size_list[0] + size_list[1];
  return cscatnsize(str_list, size_list, 2, size_total);
}

/** Concnatenates amount strings together. */
char * cscatn(const char ** str_list, size_t amount) {
  char   * result;
  size_t   size_total = 0;
  size_t * size_list;
  int      listdex;
  size_list = calloc(sizeof((*size_list)), amount);
  if(!size_list) return NULL;
  /* fill in size list */
  for(listdex = 0; listdex < amount; listdex++) {
    size_list[listdex] = cslen(str_list[listdex]);
    size_total        += size_list[listdex];
  }
  result = cscatnsize(str_list, size_list, amount, size_total);
  free(size_list); 
  return result;
}


/** Joins amount strings together,using str_join as a separator between them. 
The sizes are known before and available in the size_list, which MUST also have amount sizes in it. size_total must contain the total size of the string. This is mostly a helper function for csjoin and csjoinn.
*/
/*
char * csjoinnsize(const char ** str_list, size_t * size_list, 
                  size_t amount, size_t size_total, char * str_join) {
  char   * result;
  int      index, listdex;
  size_t   size_join = cslen(join);
  if(!str_list)   return NULL;
  if(!size_list)  return NULL;
  result = csnew(size_total + );
  if(!result)     return NULL;
  index  = 0;
  for(listdex = 0; listdex < amount; listdex++) {
    memcpy(result + index, str_list[listdex], size_list[listdex]);
    index += size_list[listdex];
  }
  return result;
}
*/


/*
String
manipulation  strcpy  wcscpy  copies one string to another
strncpy   wcsncpy   writes exactly n bytes/wchar_t, copying from source or adding nulls
strcat  wcscat  appends one string to another
strncat   wcsncat   appends no more than n bytes/wchar_t from one string to another
strxfrm   wcsxfrm   transforms a string according to the current locale
String
examination   strlen  wcslen  returns the length of the string
strcmp  wcscmp  compares two strings
strncmp   wcsncmp   compares a specific number of bytes/wchar_t in two strings
strcoll   wcscoll   compares two strings according to the current locale
strchr  wcschr  finds the first occurrence of a byte/wchar_t in a string
strrchr   wcsrchr   finds the last occurrence of a byte/wchar_t in a string
strspn  wcsspn  finds in a string the first occurrence of a byte/wchar_t not in a set
strcspn   wcscspn   finds in a string the last occurrence of a byte/wchar_t not in a set
strpbrk   wcspbrk   finds in a string the first occurrence of a byte/wchar_t in a set
strstr  wcsstr  finds the first occurrence of a substring in a string
strtok  wcstok  splits string into tokens

*/



