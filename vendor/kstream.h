/* The MIT License

   Copyright (c) 2008, by Attractive Chaos <attractivechaos@aol.co.uk>

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#ifndef AC_KSTREAM_H
#define AC_KSTREAM_H

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define __KS_TYPE(type_t)						\
	typedef struct __kstream_t {				\
		char *buf;								\
		int begin, end, is_eof;					\
		type_t f;								\
	} kstream_t;

#define ks_eof(ks) ((ks)->is_eof && (ks)->begin >= (ks)->end)

#define __KS_BASIC(type_t, __bufsize)								\
	static inline kstream_t *ks_init(type_t f)						\
	{																\
		kstream_t *ks = (kstream_t*)calloc(1, sizeof(kstream_t));	\
		ks->f = f;													\
		ks->buf = (char*)malloc(__bufsize);							\
		return ks;													\
	}																\
	static inline void ks_destroy(kstream_t *ks)					\
	{																\
		if (ks) {													\
			free(ks->buf);											\
			free(ks);												\
		}															\
	}

#define __KS_GETC(__read, __bufsize)						\
	static inline int ks_getc(kstream_t *ks)				\
	{														\
		if (ks->is_eof && ks->begin >= ks->end) return -1;	\
		if (ks->begin >= ks->end) {							\
			ks->begin = 0;									\
			ks->end = __read(ks->f, ks->buf, __bufsize);	\
			if (ks->end < __bufsize) ks->is_eof = 1;		\
			if (ks->end == 0) return -1;					\
		}													\
		return (int)ks->buf[ks->begin++];					\
	}

typedef struct __kstring_t {
	size_t l, m;
	unsigned char *s;
} kstring_t;

#ifndef kroundup32
#define kroundup32(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))
#endif

#define __KS_GETUNTIL(__read, __bufsize)								\
	static int ks_getuntil(kstream_t *ks, int delimiter, kstring_t *str, int *dret) \
	{																	\
		if (dret) *dret = 0;											\
		str->l = 0;														\
		if (ks->begin >= ks->end && ks->is_eof) return -1;				\
		for (;;) {														\
			int i;														\
			if (ks->begin >= ks->end) {									\
				if (!ks->is_eof) {										\
					ks->begin = 0;										\
					ks->end = __read(ks->f, ks->buf, __bufsize);		\
					if (ks->end < __bufsize) ks->is_eof = 1;			\
					if (ks->end == 0) break;							\
				} else break;											\
			}															\
			if (delimiter) {											\
				for (i = ks->begin; i < ks->end; ++i)					\
					if (ks->buf[i] == delimiter) break;					\
			} else {													\
				for (i = ks->begin; i < ks->end; ++i)					\
					if (isspace(ks->buf[i])) break;						\
			}															\
			if (str->m - str->l < i - ks->begin + 1) {					\
				str->m = str->l + (i - ks->begin) + 1;					\
				kroundup32(str->m);										\
				str->s = (unsigned char*)realloc(str->s, str->m);		\
			}															\
			memcpy(str->s + str->l, ks->buf + ks->begin, i - ks->begin); \
			str->l = str->l + (i - ks->begin);							\
			ks->begin = i + 1;											\
			if (i < ks->end) {											\
				if (dret) *dret = ks->buf[i];							\
				break;													\
			}															\
		}																\
		str->s[str->l] = '\0';											\
		return str->l;													\
	}

#define KSTREAM_INIT(type_t, __read, __bufsize) \
	__KS_TYPE(type_t)							\
	__KS_BASIC(type_t, __bufsize)				\
	__KS_GETC(__read, __bufsize)				\
	__KS_GETUNTIL(__read, __bufsize)

#endif
