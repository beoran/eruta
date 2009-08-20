/** 
 * File:   au_str.h
 * Author: bjmey, 2008
 *
 * Interface to a character buffer that has the property of being pointer 
 * compatible with char * pointers. 
 * It could be used as a simple string, for example, for symbols. For advanced 
 * strings, there will be a higher level String type.
 *
 */

#ifndef _AU_STR_H
#define	_AU_STR_H

#ifdef	__cplusplus
extern "C" {
#endif
    
struct          au_str_struct;    
typedef struct  au_str_struct  * au_str;
    
struct au_str_struct {
    char * cstr;
    size_t size;
};

au_str au_str_new(char * cstr);
au_str au_str_free(au_str self);

const char * au_str_c(au_str self);

au_str  au_str_cpy   (au_str target, const au_str source);
au_str  au_str_ncpy  (au_str target, const au_str source, size_t n);
au_str  au_str_cat   (au_str target, const au_str source);
au_str  au_str_ncat  (au_str target, const au_str source, size_t n);
au_str  au_str_chr   (const au_str haystack, int c);
au_str  au_str_rchr  (const au_str haystack, int c);
size_t  au_str_spn   (const au_str haystack, const au_str needle);
size_t  au_str_cspn  (const au_str haystack, const au_str needle);
au_str  au_str_pbrk  (const au_str haystack, const au_str needle);
au_str  au_str_str   (const au_str haystack, const au_str needle);
size_t  au_str_len   (const au_str self);
int     au_str_cmp   (const au_str self, const au_str other);
size_t  au_str_ncmp  (const au_str self, const au_str other, size_t n);
au_str  au_str_error (int n);
au_str  au_str_toka  (const au_str haystack, const au_str needle, au_str *results, int * nresults);



#ifdef	__cplusplus
}
#endif

#endif	/* _AU_STR_H */

