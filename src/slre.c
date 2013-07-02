// Copyright (c) 2004-2012 Sergey Lyubka <valenok@gmail.com>
// All rights reserved
//
// Modifications and enhancements by <beoran@gmail.com>, 2013.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "slre.h"

#ifdef _WIN32
#define snprintf _snprintf
#endif

// Compiled regular expression
struct slre {
  unsigned char code[256];
  unsigned char data[256];
  int code_size;
  int data_size;
  int num_caps;   // Number of bracket pairs
  int anchored;   // Must match from string start
  enum slre_option options;
  int error;      // Error code
};


/* BDM: extended to support more isXXX functionality. */

enum {
  END, BRANCH, ANY, EXACT, ANYOF, ANYBUT, OPEN, CLOSE, BOL, EOL, STAR, PLUS,
  STARQ, PLUSQ, QUEST, SPACE, NONSPACE, DIGIT, 
  NONDIGIT, ALPHA, NONALPHA, ALNUM, NONALNUM, BLANK, NONBLANK, XDIGIT, NONXDIGIT
};

// Commands and operands are all unsigned char (1 byte long). All code offsets
// are relative to current address, and positive (always point forward). Data
// offsets are absolute. Commands with operands:
//
// BRANCH offset1 offset2
//  Try to match the code block that follows the BRANCH instruction
//  (code block ends with END). If no match, try to match code block that
//  starts at offset1. If either of these match, jump to offset2.
//
// EXACT data_offset data_length
//  Try to match exact string. String is recorded in data section from
//  data_offset, and has length data_length.
//
// OPEN capture_number
// CLOSE capture_number
//  If the user have passed 'struct slre_captured' array for captures, OPEN
//  records the beginning of the matched substring (cap->ptr), CLOSE
//  sets the length (cap->len) for respective capture_number.
//
// STAR code_offset
// PLUS code_offset
// QUEST code_offset
//  *, +, ?, respectively. Try to gobble as much as possible from the
//  matched buffer while code block that follows these instructions
//  matches. When the longest possible string is matched,
//  jump to code_offset
//
// STARQ, PLUSQ are non-greedy versions of STAR and PLUS.

static const char *meta_characters      = "|.^$*+?()[\\";
static const char *message_match        = "Match";
static const char *error_no_match       = "No match";
static const char *error_jump_offset    = "Jump offset is too big" ;
static const char *error_code_too_long  = "RE is too long (code overflow)";
static const char *error_data_too_long  = "RE is too long (data overflow)";
static const char *error_text_too_long  = "RE is too long (text overflow)";
static const char *error_no_paren       = "No closing parenthesis";
static const char *error_bad_paren      = "Unbalanced parenthesis";
static const char *error_no_bracket     = "No closing ']' bracket";
static const char *error_too_many_paren = "Too many parenthesis";
static const char *error_int_failed     = "SLRE_INT: capture failed";
static const char *error_int_size       = "SLRE_INT: unsupported size";
static const char *error_float_size     = "SLRE_FLOAT: unsupported size";
static const char *error_float_failed   = "SLRE_FLOAT: capture failed";
static const char *error_string_size    = "SLRE_STRING: buffer size too small";
static const char *error_unknown_type   =  
      "Unknown type, expected SLRE_(INT|FLOAT|STRING|CALLBACK|CAPTURED)";
static const char *error_null_captured  =  "SLRE_CAPTURED: null captured struct";


/* Converts an SLRE error code to a string. Returns NULL if unknown error is used. */
const char * slre_error(int code) { 
  switch(code) { 
    case SLRE_OK                     : return message_match;
    case SLRE_ERROR_NO_MATCH            : return error_no_match;
    case SLRE_ERROR_BAD_PAREN           : return error_bad_paren;
    case SLRE_ERROR_CODE_TOO_LONG       : return error_code_too_long;
    case SLRE_ERROR_DATA_TOO_LONG       : return error_data_too_long;
    case SLRE_ERROR_TEXT_TOO_LONG       : return error_text_too_long;
    case SLRE_ERROR_FLOAT_FAILED        : return error_float_failed;
    case SLRE_ERROR_FLOAT_SIZE          : return error_float_size;
    case SLRE_ERROR_INT_FAILED          : return error_int_failed;
    case SLRE_ERROR_INT_SIZE            : return error_int_size;
    case SLRE_ERROR_JUMP_OFFSET         : return error_jump_offset;
    case SLRE_ERROR_NO_BRACKET          : return error_no_bracket;
    case SLRE_ERROR_NO_PAREN            : return error_no_paren;
    case SLRE_ERROR_STRING_SIZE         : return error_string_size;
    case SLRE_ERROR_TOO_MANY_PAREN      : return error_too_many_paren;
    case SLRE_ERROR_UNKNOWN_TYPE        : return error_unknown_type;
    case SLRE_ERROR_NULL_CAPTURED       : return error_null_captured;
    default                             : return NULL;
  }
}


/*     

*/

static void set_jump_offset(struct slre *r, int pc, int offset) {
  assert(offset < r->code_size);
  if (r->code_size - offset > 0xff) {
    r->error = SLRE_ERROR_JUMP_OFFSET;
  } else {
    r->code[pc] = (unsigned char) (r->code_size - offset);
  }
}

static void emit(struct slre *r, int code) {
  if (r->code_size >= (int) (sizeof(r->code) / sizeof(r->code[0]))) {
    r->error = SLRE_ERROR_CODE_TOO_LONG;
  } else {
    r->code[r->code_size++] = (unsigned char) code;
  }
}

static void store_char_in_data(struct slre *r, int ch) {
  if (r->data_size >= (int) sizeof(r->data)) {
    r->error = SLRE_ERROR_DATA_TOO_LONG;
  } else {
    r->data[r->data_size++] = ch;
  }
}

static void exact(struct slre *r, const char **re) {
  int  old_data_size = r->data_size;

  while (**re != '\0' && (strchr(meta_characters, **re)) == NULL) {
    store_char_in_data(r, *(*re)++);
  }

  emit(r, EXACT);
  emit(r, old_data_size);
  emit(r, r->data_size - old_data_size);
}

static int get_escape_char(const char **re) {
  int  res;

  switch (*(*re)++) {
    case 'n':  res = '\n';    break;
    case 'r':  res = '\r';    break;
    case 't':  res = '\t';    break;
    case '0':  res = 0;    break;
    case 'S':  res = NONSPACE   << 8;  break;
    case 's':  res = SPACE      << 8;  break;
    case 'D':  res = NONDIGIT   << 8;  break;
    case 'd':  res = DIGIT      << 8;  break;    
    case 'x':  res = XDIGIT     << 8;  break;
    case 'X':  res = NONXDIGIT  << 8;  break;
    case 'a':  res = ALPHA      << 8;  break;
    case 'A':  res = NONALPHA   << 8;  break;
    case 'w':  res = ALNUM      << 8;  break;
    case 'W':  res = NONALNUM   << 8;  break;
    case 'b':  res = BLANK      << 8;  break;
    case 'B':  res = NONBLANK   << 8;  break;
    default:  res = (*re)[-1];  break;
  }

  return res;
}

static void anyof(struct slre *r, const char **re) {
  int  esc, old_data_size = r->data_size, op = ANYOF;

  if (**re == '^') {
    op = ANYBUT;
    (*re)++;
  }

  while (**re != '\0')

    switch (*(*re)++) {
      case ']':
        emit(r, op);
        emit(r, old_data_size);
        emit(r, r->data_size - old_data_size);
        return;
        // NOTREACHED
        break;
      case '\\':
        esc = get_escape_char(re);
        if ((esc & 0xff) == 0) {
          store_char_in_data(r, 0);
          store_char_in_data(r, esc >> 8);
        } else {
          store_char_in_data(r, esc);
        }
        break;
      default:
        store_char_in_data(r, (*re)[-1]);
        break;
    }

  r->error = SLRE_ERROR_NO_BRACKET; 
}

static void relocate(struct slre *r, int begin, int shift) {
  emit(r, END);
  memmove(r->code + begin + shift, r->code + begin, r->code_size - begin);
  r->code_size += shift;
}

static void quantifier(struct slre *r, int prev, int op) {
  if (r->code[prev] == EXACT && r->code[prev + 2] > 1) {
    r->code[prev + 2]--;
    emit(r, EXACT);
    emit(r, r->code[prev + 1] + r->code[prev + 2]);
    emit(r, 1);
    prev = r->code_size - 3;
  }
  relocate(r, prev, 2);
  r->code[prev] = op;
  set_jump_offset(r, prev + 1, prev);
}

static void exact_one_char(struct slre *r, int ch) {
  emit(r, EXACT);
  emit(r, r->data_size);
  emit(r, 1);
  store_char_in_data(r, ch);
}

static void fixup_branch(struct slre *r, int fixup) {
  if (fixup > 0) {
    emit(r, END);
    set_jump_offset(r, fixup, fixup - 2);
  }
}

static void compile(struct slre *r, const char **re) {
  int  op, esc, branch_start, last_op, fixup, cap_no, level;

  fixup = 0;
  level = r->num_caps;
  branch_start = last_op = r->code_size;

  for (;;)
    switch (*(*re)++) {

      case '\0':
        (*re)--;
        return;
        // NOTREACHED
        break;

      case '^':
        emit(r, BOL);
        break;

      case '$':
        emit(r, EOL);
        break;

      case '.':
        last_op = r->code_size;
        emit(r, ANY);
        break;

      case '[':
        last_op = r->code_size;
        anyof(r, re);
        break;

      case '\\':
        last_op = r->code_size;
        esc = get_escape_char(re);
        if (esc & 0xff00) {
          emit(r, esc >> 8);
        } else {
          exact_one_char(r, esc);
        }
        break;

      case '(':
        last_op = r->code_size;
        cap_no = ++r->num_caps;
        emit(r, OPEN);
        emit(r, cap_no);

        compile(r, re);
        if (*(*re)++ != ')') {
          r->error = SLRE_ERROR_NO_PAREN;
          return;
        }

        emit(r, CLOSE);
        emit(r, cap_no);
        break;

      case ')':
        (*re)--;
        fixup_branch(r, fixup);
        if (level == 0) {
          r->error = SLRE_ERROR_BAD_PAREN;
          return;
        }
        return;
        // NOTREACHED
        break;

      case '+':
      case '*':
        op = (*re)[-1] == '*' ? STAR: PLUS;
        if (**re == '?') {
          (*re)++;
          op = op == STAR ? STARQ : PLUSQ;
        }
        quantifier(r, last_op, op);
        break;

      case '?':
        quantifier(r, last_op, QUEST);
        break;

      case '|':
        fixup_branch(r, fixup);
        relocate(r, branch_start, 3);
        r->code[branch_start] = BRANCH;
        set_jump_offset(r, branch_start + 1, branch_start);
        fixup = branch_start + 2;
        r->code[fixup] = 0xff;
        break;

      default:
        (*re)--;
        last_op = r->code_size;
        exact(r, re);
        break;
    }
}

// Compile regular expression. If success, 1 is returned.
// If error, 0 is returned and slre.error_string points to the error message.
static int compile2(struct slre *r, const char *re) {
  r->error = 0;
  r->code_size = r->data_size = r->num_caps = r->anchored = 0;

  if (*re == '^') {
    r->anchored++;
  }

  emit(r, OPEN);  // This will capture what matches full RE
  emit(r, 0);

  while (*re != '\0') {
    compile(r, &re);
  }

  if (r->code[2] == BRANCH) {
    fixup_branch(r, 4);
  }

  emit(r, CLOSE);
  emit(r, 0);
  emit(r, END);

#if 0
  static void dump(const struct slre *, FILE *);
  dump(r, stdout);
#endif

  return r->error;
}

static int match(const struct slre *, int, const char *, int, int *,
                         struct slre_captured *, int caps_size);

static void loop_greedy(const struct slre *r, int pc, const char *s, int len,
                        int *ofs) {
  int  saved_offset, matched_offset;

  saved_offset = matched_offset = *ofs;

  while (!match(r, pc + 2, s, len, ofs, NULL, 0)) {
    saved_offset = *ofs;
    if (!match(r, pc + r->code[pc + 1], s, len, ofs, NULL, 0)) {
      matched_offset = saved_offset;
    }
    *ofs = saved_offset;
  }

  *ofs = matched_offset;
}

static void loop_non_greedy(const struct slre *r, int pc, const char *s,
                            int len, int *ofs) {
  int  saved_offset = *ofs;

  while (!match(r, pc + 2, s, len, ofs, NULL, 0)) {
    saved_offset = *ofs;
    if (!match(r, pc + r->code[pc + 1], s, len, ofs, NULL, 0))
      break;
  }

  *ofs = saved_offset;
}

static int is_any_of(const unsigned char *p, int len, const char *s, int *ofs) {
  int  i, ch;

  ch = s[*ofs];

  for (i = 0; i < len; i++)
    if (p[i] == ch) {
      (*ofs)++;
      return 1;
    }

  return 0;
}

static int is_any_but(const unsigned char *p, int len, const char *s,
                      int *ofs) {
  int  i, ch;

  ch = s[*ofs];

  for (i = 0; i < len; i++)
    if (p[i] == ch) {
      return 0;
    }

  (*ofs)++;
  return 1;
}

static int lowercase(const char *s) {
  return tolower(* (const unsigned char *) s);
}

static int casecmp(const void *p1, const void *p2, size_t len) {
  const char *s1 = p1, *s2 = p2;
  int diff = 0;

  if (len > 0)
    do {
      diff = lowercase(s1++) - lowercase(s2++);
    } while (diff == 0 && s1[-1] != '\0' && --len > 0);

  return diff;
}


/* Macro to easily implement match cases */
#define MATCH_WITH_FUNCTION(FUNC)                                       \
        error = SLRE_ERROR_NO_MATCH;                                    \
        if (*ofs < len && FUNC(((unsigned char *)s)[*ofs])) {           \
          (*ofs)++;                                                     \
          error = SLRE_OK;                                              \
        }                                                               \
        pc++;                                                           \

/* Macro to easily implement negative match cases */
#define MATCH_WITH_NEGATE_FUNCTION(FUNC)                                \
        error = SLRE_ERROR_NO_MATCH;                                    \
        if (*ofs < len && (!(FUNC(((unsigned char *)s)[*ofs])))) {      \
          (*ofs)++;                                                     \
          error = SLRE_OK;                                              \
        }                                                               \
        pc++;                                                           \



static int match(const struct slre *r, int pc, const char *s, int len,
                         int *ofs, struct slre_captured *caps, int caps_size) {
  int n, saved_offset;
  int error = SLRE_OK;
  int (*cmp)(const void *string1, const void *string2, size_t len);

  while (error == SLRE_OK && r->code[pc] != END) {

    assert(pc < r->code_size);
    assert(pc < (int) (sizeof(r->code) / sizeof(r->code[0])));

    switch (r->code[pc]) {
      case BRANCH:
        saved_offset = *ofs;
        error = match(r, pc + 3, s, len, ofs, caps, caps_size);
        if (error != SLRE_OK) {
          *ofs = saved_offset;
          error = match(r, pc + r->code[pc + 1], s, len, ofs, caps,
                               caps_size);
        }
        pc += r->code[pc + 2];
        break;

      case EXACT:
        error = SLRE_ERROR_NO_MATCH;
        n = r->code[pc + 2];  // String length
        cmp = r->options & SLRE_CASE_INSENSITIVE ? casecmp : memcmp;
        if (n <= len - *ofs && !cmp(s + *ofs, r->data + r->code[pc + 1], n)) {
          (*ofs) += n;
          error = SLRE_OK;
        }
        pc += 3;
        break;

      case QUEST:        
        error = SLRE_OK;
        saved_offset = *ofs;
        if (match(r, pc + 2, s, len, ofs, caps, caps_size) != SLRE_OK) {
          *ofs = saved_offset;
        }
        pc += r->code[pc + 1];
        break;

      case STAR:
        error = SLRE_OK;
        loop_greedy(r, pc, s, len, ofs);
        pc += r->code[pc + 1];
        break;

      case STARQ:
        error = SLRE_OK;
        loop_non_greedy(r, pc, s, len, ofs);
        pc += r->code[pc + 1];
        break;

      case PLUS:
        if ((error = match(r, pc + 2, s, len, ofs,
                                  caps, caps_size)) != SLRE_OK) {
          break;
        }
        loop_greedy(r, pc, s, len, ofs);
        pc += r->code[pc + 1];
        break;

      case PLUSQ:
        if ((error = match(r, pc + 2, s, len, ofs,
                                  caps, caps_size)) != SLRE_OK) {
          break;
        }
        loop_non_greedy(r, pc, s, len, ofs);
        pc += r->code[pc + 1];
        break;
        
      case SPACE:
        MATCH_WITH_FUNCTION(isspace)
        break; 

      case NONSPACE:
        MATCH_WITH_NEGATE_FUNCTION(isspace)
        break; 
        
      case DIGIT:
        MATCH_WITH_FUNCTION(isdigit)
        break; 
        
      case NONDIGIT:
        MATCH_WITH_NEGATE_FUNCTION(isdigit)
        break;
        
      case ALPHA:
        MATCH_WITH_FUNCTION(isalpha)
        break;
        
      case ALNUM:
        MATCH_WITH_FUNCTION(isalnum)
        break;
        
      case BLANK:
        MATCH_WITH_FUNCTION(isblank)
        break;
        
      case XDIGIT:
        MATCH_WITH_FUNCTION(isxdigit)
        break; 
        
      case NONALPHA:
        MATCH_WITH_NEGATE_FUNCTION(isalpha)
        break;
        
      case NONALNUM:
        MATCH_WITH_NEGATE_FUNCTION(isalnum)
        break;
        
      case NONBLANK:
        MATCH_WITH_NEGATE_FUNCTION(isblank)
        break;
        
      case NONXDIGIT:
        MATCH_WITH_NEGATE_FUNCTION(isxdigit)
        break;

      case ANY:
        error = SLRE_ERROR_NO_MATCH;
        if (*ofs < len) {
          (*ofs)++;
          error = SLRE_OK;
        }
        pc++;
        break;

      case ANYOF:
        error = SLRE_ERROR_NO_MATCH;
        if (*ofs < len)
          error = is_any_of(r->data + r->code[pc + 1], r->code[pc + 2],
                                   s, ofs) ? SLRE_OK : SLRE_ERROR_NO_MATCH ;
        pc += 3;
        break;

      case ANYBUT:
        error = SLRE_ERROR_NO_MATCH;
        if (*ofs < len)
          error = is_any_but(r->data + r->code[pc + 1], r->code[pc + 2],
                                    s, ofs) ? SLRE_OK : SLRE_ERROR_NO_MATCH ;
        pc += 3;
        break;

      case BOL:
        error = *ofs == 0 ? SLRE_OK : SLRE_ERROR_NO_MATCH; 
        pc++;
        break;

      case EOL:
        error = *ofs == len ? SLRE_OK : SLRE_ERROR_NO_MATCH;
        pc++;
        break;

      case OPEN:
        if (caps != NULL) {
          if (caps_size - 2 < r->code[pc + 1]) {
            error = SLRE_ERROR_TOO_MANY_PAREN;
          } else {
            caps[r->code[pc + 1]].ptr = s + *ofs;
          }
        }
        pc += 2;
        break;

      case CLOSE:
        if (caps != NULL) {
          assert(r->code[pc + 1] >= 0);
          assert(r->code[pc + 1] < caps_size);
          caps[r->code[pc + 1]].len = (s + *ofs) -
            caps[r->code[pc + 1]].ptr;
        }
        pc += 2;
        break;

      case END:
        pc++;
        break;

      default:
        printf("unknown cmd (%d) at %d\n", r->code[pc], pc);
        assert(0);
        break;
    }
  }

  return error;
}

// Return 1 if match, 0 if no match.
// If `captured_substrings' array is not NULL, then it is filled with the
// values of captured substrings. captured_substrings[0] element is always
// a full matched substring. The round bracket captures start from
// captured_substrings[1].
// It is assumed that the size of captured_substrings array is enough to
// hold all captures. The caller function must make sure it is! So, the
// array_size = number_of_round_bracket_pairs + 1
static int match2(const struct slre *r, const char *buf, int len,
                          struct slre_captured *caps, int caps_size) {
  int  i, ofs = 0;
  int error   = SLRE_ERROR_NO_MATCH;

  if (caps != NULL) {
    memset(caps, 0, caps_size * sizeof(caps[0]));
  }

  if (r->anchored) {
    error = match(r, 0, buf, len, &ofs, caps, caps_size);
  } else {
    for (i = 0; i < len && error != SLRE_OK; i++) {
      ofs = i;
      error = match(r, 0, buf, len, &ofs, caps, caps_size);
    }
  }

  return error;
}

static int capture_float(const struct slre_captured *cap, void *p, size_t len) {
  const char *fmt;
  char buf[20];

  switch (len) {
    case sizeof(float): fmt = "f"; break;
    case sizeof(double): fmt = "lf"; break;
    default: return SLRE_ERROR_FLOAT_SIZE;
  }

  snprintf(buf, sizeof(buf), "%%%d%s", cap->len, fmt);
  return sscanf(cap->ptr, buf, p) == 1 ? SLRE_OK : SLRE_ERROR_FLOAT_FAILED;
}

static int capture_string(const struct slre_captured *cap, void *p, size_t len) {
  if ((int) len <= cap->len) {
    return SLRE_ERROR_STRING_SIZE;
  }
  memcpy(p, cap->ptr, cap->len);
  ((char *) p)[cap->len] = '\0';
  return SLRE_OK;
}

static int capture_int(const struct slre_captured *cap, void *p, size_t len) {
  const char *fmt;
  char buf[20];

  switch (len) {
    case sizeof(char): fmt = "hh"; break;
    case sizeof(short): fmt = "h"; break;
    case sizeof(int): fmt = "d"; break;
    case sizeof(long long int): fmt = "lld"; break;
    default: return SLRE_ERROR_INT_SIZE;
  }

  snprintf(buf, sizeof(buf), "%%%d%s", cap->len, fmt);
  return sscanf(cap->ptr, buf, p) == 1 ?  SLRE_OK : SLRE_ERROR_INT_FAILED;
}

static int capture_callback(const struct slre_captured *cap, int i, slre_callback *fp, void * extra) {
  return fp(i, cap->ptr, cap->len, extra);
}

static int capture_captured(const struct slre_captured *cap, struct slre_captured *target) {
  if(!target) return SLRE_ERROR_NULL_CAPTURED;
  (*target) = (*cap);
  return 0;
}


static int capture(const struct slre_captured *caps, int num_caps, va_list ap) {
  int i, type;
  size_t size                 = 0;
  void             *p         = NULL;
  int             err         = SLRE_OK;
  int             callback_ok = 0;  
  slre_callback * fp          = NULL;

  for (i = 0; i < num_caps; i++) {
    /* Callback needs slightly different arguments, only taken once. */
    if (!callback_ok) { 
      type = va_arg(ap, int); 
      // stop processing captures if the type SLRE_IGNORE is seen. 
      if (type == SLRE_IGNORE) {
        return err;
      }
      
      if (type == SLRE_CALLBACK) { 
        callback_ok = !callback_ok;
        fp   = va_arg(ap, slre_callback*);
        p    = va_arg(ap, void *);
      } else if (type == SLRE_CAPTURED) {  
        p    = va_arg(ap, void *);
      } else {
        size = va_arg(ap, size_t);
        p    = va_arg(ap, void *);
      }
    } 
    
    switch (type) {
      case SLRE_INT     : err = capture_int(&caps[i], p, size); break;
      case SLRE_FLOAT   : err = capture_float(&caps[i], p, size); break;
      case SLRE_STRING  : err = capture_string(&caps[i], p, size); break;
      case SLRE_CALLBACK: err = capture_callback(&caps[i], i, fp, p); break;
      case SLRE_CAPTURED: err = capture_captured(&caps[i], p); break;
      default: err = SLRE_ERROR_UNKNOWN_TYPE; break;
    }
  }
  return err;
}

int slre_match(enum slre_option options, const char *re,
                       const char *buf, int buf_len, ...) {
  struct slre slre;
  struct slre_captured caps[SLRE_CAPURES_MAX];
  va_list ap;
  int error = SLRE_OK;

  slre.options = options;
  error = compile2(&slre, re);
  if(error != SLRE_OK)           { return error; } 
  error = match2(&slre, buf, buf_len, caps, sizeof(caps) / sizeof(caps[0]));
  if(error != SLRE_OK)           { return error; } 
  /* Don't capture if not requested. */
  if (options & SLRE_NO_CAPTURE) { return error; }
  
  va_start(ap, buf_len);
  error = capture(caps + 1, slre.num_caps, ap);
  va_end(ap);
  
  return error;
}

#if defined(SLRE_UNIT_TEST)
static struct {
  const char *name;
  int narg;
  const char *flags;
} opcodes[] = {
  {"END",      0, ""  },  // End of code block or program
  {"BRANCH",   2, "oo"},  // Alternative operator, "|"
  {"ANY",      0, ""  },  // Match any character, "."
  {"EXACT",    2, "d" },  // Match exact string
  {"ANYOF",    2, "D" },  // Match any from set, "[]"
  {"ANYBUT",   2, "D" },  // Match any but from set, "[^]"
  {"OPEN ",    1, "i" },  // Capture start, "("
  {"CLOSE",    1, "i" },  // Capture end, ")"
  {"BOL",      0, ""  },  // Beginning of string, "^"
  {"EOL",      0, ""  },  // End of string, "$"
  {"STAR",     1, "o" },  // Match zero or more times "*"
  {"PLUS",     1, "o" },  // Match one or more times, "+"
  {"STARQ",    1, "o" },  // Non-greedy STAR,  "*?"
  {"PLUSQ",    1, "o" },  // Non-greedy PLUS, "+?"
  {"QUEST",    1, "o" },  // Match zero or one time, "?"
  {"SPACE",    0, ""  },  // Match whitespace, "\s"
  {"NONSPACE", 0, ""  },  // Match non-space, "\S"
  {"DIGIT",    0, ""  }   // Match digit, "\d"
};

static void print_character_set(FILE *fp, const unsigned char *p, int len) {
  int  i;

  for (i = 0; i < len; i++) {
    if (i > 0)
      (void) fputc(',', fp);
    if (p[i] == 0) {
      i++;
      if (p[i] == 0)
        (void) fprintf(fp, "\\x%02x", p[i]);
      else
        (void) fprintf(fp, "%s", opcodes[p[i]].name);
    } else if (isprint(p[i])) {
      (void) fputc(p[i], fp);
    } else {
      (void) fprintf(fp,"\\x%02x", p[i]);
    }
  }
}

static void dump(const struct slre *r, FILE *fp) {
  int  i, j, ch, op, pc;

  for (pc = 0; pc < r->code_size; pc++) {

    op = r->code[pc];
    (void) fprintf(fp, "%3d %s ", pc, opcodes[op].name);

    for (i = 0; opcodes[op].flags[i] != '\0'; i++)
      switch (opcodes[op].flags[i]) {
        case 'i':
          fprintf(fp, "%d ", r->code[pc + 1]);
          pc++;
          break;
        case 'o':
          fprintf(fp, "%d ", pc + r->code[pc + 1] - i);
          pc++;
          break;
        case 'D':
          print_character_set(fp, r->data +
                              r->code[pc + 1], r->code[pc + 2]);
          pc += 2;
          break;
        case 'd':
          (void) fputc('"', fp);
          for (j = 0; j < r->code[pc + 2]; j++) {
            ch = r->data[r->code[pc + 1] + j];
            if (isprint(ch))
              fputc(ch, fp);
            else
              fprintf(fp,"\\x%02x",ch);
          }
          (void) fputc('"', fp);
          pc += 2;
          break;
      }

    fputc('\n', fp);
  }
}


int main(void) {
  static const struct { const char *str, *regex, *msg; } tests[] = {
    {"aa", ".+", NULL},
    {"aa", ".", NULL},
    {"", ".", "No match"},
    {" cc 1234", "c.\\s\\d+", NULL},
  };
  char buf[20];
  int int_value;
  const char *msg, *str, *re;
  size_t i;

  char method[10], uri[100];
  int http_version_minor, http_version_major;
  const char *error;
  const char *request = " \tGET /index.html HTTP/1.0\r\n\r\n";

  error = slre_match(0, "^\\s*(GET|POST)\\s+(\\S+)\\s+HTTP/(\\d)\\.(\\d)",
                     request, strlen(request),
                     SLRE_STRING,  sizeof(method), method,
                     SLRE_STRING, sizeof(uri), uri,
                     SLRE_INT, sizeof(http_version_major), &http_version_major,
                     SLRE_INT, sizeof(http_version_minor), &http_version_minor);

  if (error != NULL) {
    printf("Error parsing HTTP request: %s\n", error);
  } else {
    printf("Requested URI: %s\n", uri);
  }

  for (i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
    if ((msg = slre_match(0, tests[i].regex, tests[i].str,
                          strlen(tests[i].str), NULL)) != tests[i].msg) {
      printf("Test %zd failed: [%s] [%s] -> [%s]\n", i, tests[i].str,
             tests[i].regex, msg ? msg : "(null)");
      return EXIT_FAILURE;
    }
  }

  assert(slre_match(0, "a (\\d+)4\\s*(\\S+)", "aa 1234 xy\nz", 12,
                    SLRE_INT, sizeof(int_value), &int_value,
                    SLRE_STRING, sizeof(buf), buf) == NULL);
  assert(int_value == 123);
  assert(!strcmp(buf, "xy"));

  str = "Hello превед!";
  re = "^hello (\\S+)";
  assert(strcmp(error_no_match, slre_match(0, re, str, strlen(str), SLRE_STRING,
                                           sizeof(buf), buf)) == 0);
  assert(slre_match(SLRE_CASE_INSENSITIVE, re, str, strlen(str),
                    SLRE_STRING, sizeof(buf), buf) == NULL);
  assert(!strcmp(buf, "превед!"));

  assert(strcmp(error_no_match, slre_match(0, "bC", "aBc", 3)) == 0);
  assert(slre_match(SLRE_CASE_INSENSITIVE, "bC", "aBc", 3) == NULL);
  assert(slre_match(0, "3?9", "9", 1) == NULL);

  // TODO: fix this!
  //assert(slre_match(0, "9?9", "9", 1) == NULL);

  {
    struct slre slre;
    struct slre_captured caps[10];
    char a[10], b[10];

    memset(caps, 'x', sizeof(caps));
    slre.options = 0;
    assert(compile2(&slre, "(\\d(\\d)?)") == NULL);
    assert(!strcmp(match2(&slre, "1", 1, caps, 2), "Too many brackets"));
    assert(match2(&slre, "1", 1, caps, 3) == NULL);
    assert(slre.num_caps == 2);
    assert(caps[1].len == 1);
    assert(caps[2].len == 0);
    assert(caps[1].ptr[0] == '1');

    a[0] = b[0] = 'x';
    assert(slre_match(0, "(\\d(\\d)?)", "1", 1,
                      SLRE_STRING, sizeof(a), a,
                      SLRE_STRING, sizeof(b), b) == NULL);
    assert(!strcmp(a, "1"));
    assert(b[0] == '\0');
  }

  printf("%s\n", "All tests passed");
  return EXIT_SUCCESS;
}
#endif // SLRE_UNIT_TEST
