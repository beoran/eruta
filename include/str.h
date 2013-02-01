/** String functions. Use the ALLEGRO_USTR,
but wrap it a bit with macros to have to do less typing. */
#ifndef STR_H_
#define STR_H_
#include <allegro5/allegro.h>

/** String type */
#ifndef USSTR
#define USTR ALLEGRO_USTR
#endif

/** Static String info helper type */
#ifndef USTR_INFO
#define USTR_INFO ALLEGRO_USTR_INFO
#endif

#define ustr_new(STR)                      al_ustr_new(STR)
#define ustr_newcstr(STR)                  al_ustr_new(STR)
#define ustr_newbuffer(B, SZ)              al_ustr_new_from_buffer(B, SZ)
#define ustr_newf(FMT, ...)                al_ustr_newf(FMT, __VA_ARGS__)
#define ustr_free(STR)                     al_ustr_free(STR)
#define ustr_c(STR)                        al_cstr(STR)
#define ustr_cstr(STR)                     al_cstr(STR)
#define ustr_buffer(STR, B, SZ)            al_ustr_to_buffer(STR, B, SZ)
#define ustr_cdup(STR)                     al_cstr_dup(STR)
#define ustr_cfree(CS)                     al_free(CS)
#define ustr_dup(STR)                      al_ustr_dup(STR)
#define ustr_dupsub(STR, FROM, TO)         al_ustr_dup_substr(STR, FROM, TO)
#define ustr_empty                         al_ustr_empty_string()
#define ustr_refcstr(INFO, CS)             al_ref_cstr(INFO, CS)
#define ustr_refbuffer(INFO, BUF, SZ)      al_ref_buffer(INFO, BUF, SZ)
#define ustr_refustr(INFO, STR, FROM, TO)  al_ref_ustr(INFO, STR, FROM, TO)
#define ustr_size(STR)                     al_ustr_size(STR)
#define ustr_length(STR)                   al_ustr_length(STR)
#define ustr_offset(STR, INDEX)            al_ustr_offset(STR, INDEX)
#define ustr_next(STR, POS)                al_ustr_next(STR, POS)
#define ustr_prev(STR, POS)                al_ustr_prev(STR, PREV)
#define ustr_get(STR, INDEX)               al_ustr_get(STR, INDEX)
#define ustr_getnext(STR, POS)             al_ustr_get_next(STR, POS)
#define ustr_getprev(STR, POS)             al_ustr_prev_get(STR, POS)
#define ustr_insert(STR, POS, STR2)        al_ustr_insert(STR, POS, STR2)
#define ustr_insertcstr(STR, POS, CS)      al_ustr_insert_cstr(STR, POS, CS)
#define ustr_insertch(STR, POS, CH)        al_ustr_insert_ch(STR, POS, CH)
#define ustr_append(STR, STR2)             al_ustr_append(STR, STR2)
#define ustr_appendcstr(STR, CS)           al_ustr_append_cstr(STR, CS)
#define ustr_appendch(STR, CH)             al_ustr_append_chr(STR, CH)
#define ustr_appendf(STR, FMT, ...)        al_ustr_appendf(STR, FMT, __VA_ARGS__)
#define ustr_vappendf(STR, FMR, AP)        al_ustr_vappendf(STR, FMT, AP)
#define ustr_removechr(STR, INDEX)         al_ustr_remove_chr(STR, INDEX)
#define ustr_remove_chr(STR, INDEX)        al_ustr_remove_chr(STR, INDEX)
#define ustr_removerange(STR, FROM, TO)    al_ustr_remove_range(STR, FROM, TO)
#define ustr_truncate(STR, INDEX)          al_ustr_truncate(STR, INDEX)
#define ustr_ltrim(STR)                    al_ustr_ltrim_ws(STR, INDEX)
#define ustr_rtrim(STR)                    al_ustr_rtrim_ws(STR, INDEX)
#define ustr_trim(STR)                     al_ustr_trim_ws(STR, INDEX)
#define ustr_set(STR, STR2)                al_ustr_assign(STR, STR2)
#define ustr_setsub(STR, STR2, FROM, TO)  \
        al_ustr_assign_substr(STR, STR2, FROM, TO)
#define ustr_setcstr(STR, CS)              al_ustr_assign_cstr(STR, CS)
#define ustr_setch(STR, INDEX, CH)         al_ustr_set_chr(STR, INDEX, CH)
#define ustr_setrange(STR, FROM, TO, STR2) \
        al_ustr_replace_range(STR, FROM, TO, STR2)
#define ustr_findchr(STR, POS, CH)         al_ustr_find_chr(STR, POS, CH)
#define ustr_rfindchr(STR, POS, CH)        al_ustr_rfind_chr(STR, POS, CH)
#define ustr_findin(STR, POS, STR2)        al_ustr_find_set(STR, POS, STR2)
#define ustr_findincstr(STR, POS, CS)      al_ustr_find_set_cstr(STR, POS, CS)
#define ustr_findnotin(STR, POS, STR2)     al_ustr_find_cset(STR, POS, STR2)
#define ustr_findnotincstr(STR, POS, CS)   al_ustr_find_cset_cstr(STR, POS, CS)
#define ustr_find(STR, POS, STR2)          al_ustr_find_str(STR, POS, STR2)
#define ustr_findcstr(STR, POS, CS)        al_ustr_find_cstr(STR, POS, CS)
#define ustr_rfind(STR, POS, STR2)         al_ustr_rfind_str(STR, POS, STR2)
#define ustr_rfindcstr(STR, POS, CS)       al_ustr_rfind_cstr(STR, POS, CS)
#define ustr_replace(STR, POS, STR2, STR3)\
        al_ustr_find_replace(STR, POS, STR2, STR3)
#define ustr_replacecstr(STR, POS, CS1, CS2)\
        al_ustr_find_replace_cstr(STR, POS, CS1, CS2)
#define ustr_equal(STR, STR2)              al_ustr_equal(STR, STR2)
#define ustr_compare(STR, STR2)            al_ustr_compare(STR, STR2)
#define ustr_ncopare(STR, STR2, SZ)        al_ustr_ncompare(STR, STR2, SZ)
#define ustr_isprefix(STR, STR2)           al_ustr_has_prefix(STR, STR2)
#define ustr_issuffix(STR, STR2)           al_ustr_has_suffix(STR, STR2)
#define ustr_isprefixcstr(STR, CS)         al_ustr_has_prefix_cstr(STR, CS)
#define ustr_issuffixcstr(STR, CS)         al_ustr_has_suffix_cstr(STR, CS)


#define ustr_const(NAME, CSTR)             \
        USTR_INFO info_##NAME; US * NAME = ustr_refcstr(&info_##NAME, CSTR);

#define ustr_isempty(USTR) (ustr_size((USTR)) < 1)

/* This file was generated with:
'cfunctions -c -aoff -n -w str_proto src/str.c' */
#ifndef CFH_STR_PROTO
#define CFH_STR_PROTO

/* From 'src/str.c': */

int ustr_atoi (USTR * str );

double ustr_tod (USTR * ustr , int * ok );

long ustr_tol (USTR * ustr , int * ok , int base );

double ustr_atod (USTR * ustr );

double ustr_atol (USTR * ustr );

USTR * ustr_newdouble (double d );

USTR * ustr_newlong (long l );

int cstr_charis (const char * expression , int ch );

int cstr_simplematch (const char * expression , int ch );

#endif /* CFH_STR_PROTO */


#include "bad.h"

/* An ustrlist is a list of UTF-8 enabled strings.  */
typedef struct USTRListNode_    USTRListNode;
typedef        BadList          USTRList; 


struct USTRListNode_ {
  USTR    * ustr;
  BadListNode   list;  
};

USTR *         ustrlistnode_ustr(USTRListNode * self);
USTRListNode * ustrlistnode_alloc(void);
USTRListNode * ustrlistnode_init(USTRListNode * self, const USTR * ustr);
USTRListNode * ustrlistnode_initcstr(USTRListNode * self, const char * str);
USTRListNode * ustrlistnode_new(const USTR * ustr);
USTRListNode * ustrlistnode_done(USTRListNode * self);
USTRListNode * badlistnode_ustrlistnode(BadListNode * elem);
USTRListNode * ustrlistnode_free(USTRListNode * self);

USTRListNode * ustrlistnode_next(USTRListNode * self);
USTRListNode * ustrlistnode_prev(USTRListNode * self);
USTRListNode * ustrlist_head(USTRList * self);
USTRListNode * ustrlist_tail(USTRList * self);


USTRList * ustrlist_alloc(void);
USTRList * ustrlist_init(USTRList * self);
USTRList * ustrlist_new();
USTRList * ustrlist_done(USTRList * self);
USTRList * ustrlist_free(USTRList * self);
USTRList * ustrlist_addnode(USTRList * self, USTRListNode * node);
USTRList * ustrlist_removenode(USTRList * self, USTRListNode * node);

USTRListNode * ustrlist_addustr(USTRList * self, const USTR * ustr);
USTRListNode * ustrlist_addcstr(USTRList * self, const char * cstr);
USTRListNode * ustrlist_shiftustr(USTRList * self, const USTR * ustr);
USTRListNode * ustrlist_shiftcstr(USTRList * self, const char * cstr);

int ustrlist_size(USTRList * self);

USTRList * ustrlist_droplast(USTRList * self);

USTR * ustrlist_join(USTRList * self);
USTR * ustrlist_joinwithcstr(USTRList * self, const char * sep);
USTR * ustrlist_joinwithch(USTRList * self, const char ch);
USTR * ustrlist_joinwithustr(USTRList * self, USTR * sep);

USTRListNode * ustrlist_skipnode(USTRList * self, int skip);


#endif
