/** String functions. Use the ALLEGRO_USTR,
but wrap it a bit with macros to have to do less typing. */
#ifndef STR_H_
#define STR_H_

#include <allegro5/allegro.h>

/** String type */
#ifndef STR
#define STR ALLEGRO_USTR
#endif

/** Static String type */
#ifndef STR
#define STRINFO ALLEGRO_USTR_INFO
#endif

#define str_new(STR)                      al_ustr_new(STR)
#define str_newcstr(STR)                  al_ustr_new(STR)
#define str_newbuffer(B, SZ)              al_ustr_new_from_buffer(B, SZ)
#define str_newf(STR, FMT, ...)           al_ustr_newf(STR, FMT, __VA_ARGS__)
#define str_free(STR)                     al_ustr_free(STR)
#define str_c(STR)                        al_cstr(STR)
#define str_cstr(STR)                     al_cstr(STR)
#define str_buffer(STR, B, SZ)            al_ustr_to_buffer(STR, B, SZ)
#define str_cdup(STR)                     al_cstr_dup(STR)
#define str_cfree(CS)                     al_free(CS)
#define str_dup(STR)                      al_ustr_dup(STR)
#define str_dupsub(STR, FROM, TO)         al_ustr_dup_substr(STR, FROM, TO)
#define str_empty                         al_ustr_empty_string()
#define str_refcstr(INFO, CS)             al_ref_cstr(INFO, CS)
#define str_refbuffer(INFO, BUF, SZ)      al_ref_buffer(INFO, BUF, SZ)
#define str_refstr(INFO, STR, FROM, TO)   al_ref_ustr(INFO, STR, FROM, TO)
#define str_size(STR)                     al_ustr_size(STR)
#define str_length(STR)                   al_ustr_length(STR)
#define str_offset(STR, INDEX)            al_ustr_offset(STR, INDEX)
#define str_next(STR, POS)                al_ustr_next(STR, POS)
#define str_prev(STR, POS)                al_ustr_prev(STR, PREV)
#define str_get(STR, INDEX)               al_ustr_get(STR, INDEX)
#define str_getnext(STR, POS)             al_ustr_get_next(STR, POS)
#define str_getprev(STR, POS)             al_ustr_prev_get(STR, POS)
#define str_insert(STR, POS, STR2)        al_ustr_insert(STR, POS, STR2)
#define str_insertcstr(STR, POS, CS)      al_ustr_insert_cstr(STR, POS, CS)
#define str_insertch(STR, POS, CH)        al_ustr_insert_ch(STR, POS, CH)
#define str_append(STR, STR2)             al_ustr_append(STR, STR2)
#define str_appendcstr(STR, CS)           al_ustr_append_cstr(STR, CS)
#define str_appendch(STR, CH)             al_ustr_append_ch(STR, CH)
#define str_appendf(STR, FMT, ...)        al_ustr_appendf(STR, FMT, __VA_ARGS__)
#define str_vappendf(STR, FMR, AP)        al_ustr_vappendf(STR, FMT, AP)
#define str_removechr(STR, INDEX)         al_ustr_remove_chr(STR, INDEX)
#define str_removerange(STR, FROM, TO)    al_ustr_remove_range(STR, FROM, TO)
#define str_trunbcate(STR, INDEX)         al_ustr_truncate(STR, INDEX)
#define str_ltrim(STR)                    al_ustr_ltrim_ws(STR, INDEX)
#define str_rtrim(STR)                    al_ustr_rtrim_ws(STR, INDEX)
#define str_trim(STR)                     al_ustr_trim_ws(STR, INDEX)
#define str_set(STR, STR2)                al_ustr_assign(STR, STR2)
#define str_setsub(STR, STR2, FROM, TO)  \
        al_ustr_assign_substr(STR, STR2, FROM, TO)
#define str_setcstr(STR, CS)              al_ustr_assign_cstr(STR, CS)
#define str_setch(STR, INDEX, CH)         al_ustr_set_chr(STR, INDEX, CH)
#define str_setrange(STR, FROM, TO, STR2) \
        al_ustr_replace_range(STR, FROM, TO, STR2)
#define str_findchr(STR, POS, CH)         al_ustr_find_chr(STR, POS, CH)
#define str_rfindchr(STR, POS, CH)        al_ustr_rfind_chr(STR, POS, CH)
#define str_findin(STR, POS, STR2)        al_ustr_find_set(STR, POS, STR2)
#define str_findincstr(STR, POS, CS)      al_ustr_find_set_cstr(STR, POS, CS)
#define str_findnotin(STR, POS, STR2)     al_ustr_find_cset(STR, POS, STR2)
#define str_findnotincstr(STR, POS, CS)   al_ustr_find_cset_cstr(STR, POS, CS)
#define str_find(STR, POS, STR2)          al_ustr_find_str(STR, POS, STR2)
#define str_findcstr(STR, POS, CS)        al_ustr_find_cstr(STR, POS, CS)
#define str_rfind(STR, POS, STR2)         al_ustr_rfind_str(STR, POS, STR2)
#define str_rfindcstr(STR, POS, CS)       al_ustr_rfind_cstr(STR, POS, CS)
#define str_replace(STR, POS, STR2, STR3)\
        al_ustr_find_replace(STR, POS, STR2, STR3)
#define str_replacecstr(STR, POS, CS1, CS2)\
        al_ustr_find_replace_cstr(STR, POS, CS1, CS2)
#define str_equal(STR, STR2)              al_ustr_equal(STR, STR2)
#define str_compare(STR, STR2)            al_ustr_compare(STR, STR2)
#define str_ncopare(STR, STR2, SZ)        al_ustr_ncompare(STR, STR2, SZ)
#define str_prefix(STR, STR2)             al_ustr_has_prefix(STR, STR2)
#define str_suffix(STR, STR2)             al_ustr_has_suffix(STR, STR2)
#define str_prefixcstr(STR, CS)           al_ustr_has_prefix(STR, CS)
#define str_suffixcstr(STR, CS)           al_ustr_has_suffix(STR, CS)


#include "str_proto.h"


#endif
