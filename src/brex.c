/* BREX: Beoran's regular expressions, 2013. MIT license. */

typedef struct BrexState_    BrexState;
typedef struct BrexRegexp_   BrexRegexp;
typedef struct BrexCompiled_ BrexCompiled;

/* Regexps are compiled to a nondeterministic state machine. 
 */
struct BrexState_ {
  int          match; /* Character that this state matches */
  BrexState ** out;
  int          max_out;
};


/* These values are compatible with MRI. */
#define BREX_IGNORECASE 1
#define BREX_MULTILINE  4


struct BrexRegexp_ { 
  const char    * source;
  int             flags;
  BrexCompiled *  compiled;
  /* Add compiled regexp structure here... Also add matches. */
};





/* 
Relevant part of spec for implementation reinterpreted:

Regexp must have a pattern that may not be NULL, except for initialization. 
Regexp must have a ignore case flag that's false by default.
Regexp must have a multiline flag (or .) that's false by default.
There's a IGNORECASE constant that's a power of two.
There's a MULTILINE  constant that's also a different power of two.
These two are used to set the flags on the regexp.

Syntax of regexp patterns:
Patterns

pattern ::
  alternative
  | pattern 1 | alternative 2

alternative ::
  [ empty ]
  | alternative 3 term

term ::
  anchor
  | atom 1
  | atom 2 quantifier

anchor ::
  left-anchor | right-anchor

left-anchor ::
  \A | ^

right-anchor ::
  \z | ~

quantifier ::
  * | + | ?

atom ::
  pattern-character
  | grouping
  | .
  | atom-escape-sequence

pattern-character ::
  source-character but not regexp-meta-character

regexp-meta-character ::
  | | . | * | + | ^ | ? | ( | ) | # | \ | $
  | future-reserved-meta-character

future-reserved-meta-character ::
  [ | ] | { | }

grouping ::
  ( pattern )
  
atom-escape-sequence ::
  decimal-escape-sequence
  | regexp-character-escape-sequence

decimal-escape-sequence ::
  \ decimal-digit-except-zero
  
regexp-character-escape-sequence ::
  regexp-escape-sequence
  | regexp-non-escaped-sequence
  | hexadecimal-escape-sequence
  | regexp-octal-escape-sequence
  | regexp-control-escape-sequence
  
regexp-escape-sequence ::
  \ regexp-escaped-character

regexp-escaped-character ::
  n | t | r | f | v | a | e

regexp-non-escaped-sequence ::
  \ regexp-meta-character

regexp-octal-escape-sequence ::
  octal-escape-sequence but not decimal-escape-sequence

regexp-control-escape-sequence ::
  \ ( C- | c ) regexp-control-escaped-character

regexp-control-escaped-character ::
  regexp-character-escape-sequence
  | ?
  | source-character but not ( \ | ? )

Future-reserved-meta-character s are reserved for the extension of the pattern of regular expressions.


Regexp Semantics

A regular expression selects specific substrings from a string called a target string according  to the pattern of the regular expression. If the pattern matches more than one substring, the substring which begins earliest in the target string is selected. If there is more than one such substring beginning at that point, the substring that has the highest priority, which is described
below, is selected. Each component of the pattern matches a substring of the target string as follows:
a) A pattern matches the following substring:
  1)  If the pattern is an alternative 1 , it matches the string matched with 
      the alternative 1 .
  2)  If the pattern is a pattern 1 | alternative 2 , it matches the string
      matched with either the pattern 1 or the alternative 2 . The one matched
      with the pattern 1 has a higher priority.

  EXAMPLE 1: "ab".slice(/(a|ab)/) returns "a", not "ab".

b) An alternative matches the following substring:
  1)  If the alternative is [empty], it matches an empty string.
  2)  If the alternative is an alternative 3 term, the alternative matches 
      the substring whose first part is matched with the alternative 3 and 
      whose rest part is matched with the term.
      If there is more than one such substring, the priority of the substrings is determined as follows:
      i) If there is more than one candidate which is matched with the  
         alternative 3 , a substring whose first part is a candidate with 
         a higher priority has a higher priority.

  EXAMPLE 2: "abc".slice(/(a|ab)(c|b)/) returns "ab", not "abc". In this case,
  (a|ab) is prior to (c|b).
  
      ii) If the first parts of substrings are the same, and if there is more
          than one candidate which is matched with the term, a substring whose
          rest part is a candidate with a higher priority has a higher priority.
          
  EXAMPLE 3: "abc".slice(/a(b|bc)/) returns "ab", not "abc".
  
c) A term matches the following substring:
  1) If the term is an atom 1 , it matches the string matched with the atom 1.
  2) If the term is an atom 2 quantifier, it matches a string as follows:
    i)  If the quantifier is *, it matches a sequence of zero or more strings
        matched with the atom 2.
    ii) If the quantifier is +, it matches a sequence of one or more strings
        matched with atom 2.
    iii) If the quantifier is ?, it matches a sequence of zero or one string matched with the atom 2. A longer sequence has a higher priority.
    
  EXAMPLE 4:  "aaa".slice(/a* /) returns "aaa", none of "", "a", and "aa".
  (above it was /a* / without a space, which would end the C comment.)

  3) If the term is an anchor, it matches the empty string at a specific
  position within the  target string S, as follows:
    i) If the anchor is \A, it matches an empty string at the beginning of S.
    ii) If the anchor is ^, it matches an empty string at the beginning of S or
      just after a line-terminator which is followed by at least one character.
    iii) If the anchor is \z, it matches an empty string at the end of S.
    iv) If the anchor is ~, it matches an empty string at the end of S or just before a line-terminator.
    
d) An atom matches the following substring:
  1) If the atom is a pattern-character, it matches a character C represented 
    by the pattern-character. If the atom is present in the pattern of an
    instance of the class Regexp whose ignorecase-flag attribute is true, it
    also matches a corresponding upper-case character of C, if C is a lower-case
    character, or a corresponding lower-case character of C, if C 
    is an upper-case character.
  2) If the atom is a grouping, it matches the string matched with the grouping.
  3) If the atom is “.”, it matches any character except for a line-terminator.
    If the atom is present in the pattern of an instance of the class Regexp
    whose multiline-flag attribute is true, it also matches a line-terminator.
  4) If the atom is an atom-escape-sequence, it matches the string matched with
  the atom-escape-sequence.

e) A grouping matches the substring matched with the pattern.
f) An atom-escape-sequence matches the following substring:
  1) If the atom-escape-sequence is a decimal-escape-sequence, it matches the
    string matched with the decimal-escape-sequence.
  2) If the atom-escape-sequence is a regexp-character-escape-sequence, it
    matches a string of length one, the content of which is the character
    represented by the regexp-character-escape-sequence.

g)
  A decimal-escape-sequence matches the following substring:
  1) Let i be an integer represented by decimal-digit-except-zero.
  2) Let G be the i th grouping in the pattern, counted from 1, in the order 
    of the occurrence of “(” of groupings from the left of the pattern.
  3) If the decimal-escape-sequence is present before G within the pattern, 
    it does not match any string.
  4) If G matches any string, the decimal-escape-sequence matches the same
    string.
  5) Otherwise, the decimal-escape-sequence does not match any string.

h) A regexp-character-escape-sequence represents a character as follows:
A regexp-escape-sequence represents a character as shown in 8.7.6.3.3, Table 1.

i) A regexp-non-escaped-sequence represents a regexp-meta-character.
j) A hexadecimal-escape-sequence represents a character as described in 8.7.6.3.3.
k) A regexp-octal-escape-sequence is interpreted in the same way as an octal-escape-sequence (see 8.7.6.3.3).
l) A regexp-control-escape-sequence represents a character, the code of which is com-puted by taking bitwise AND of 0x9f and the code of the character represented by the regexp-control-escaped-character, except when the regexp-control-escaped-character is ?, in which case, the regexp-control-escape-sequence represents a character whose code is 127.

Matching process

A pattern P is considered to successfully match the given string S, if there exists a substring of  S (including S itself) matched with P.
a)                                                                                                                                                                                                                                                                              
When an index is specified, it is tested if P matches the part of S which begins at the index and ends at the end of S. However, if the match succeeds, the string attribute of theresulting instance of the class MatchData is S, not the part of S which begins at the index, as described below.
b)                                                                                        
A matching process returns either an instance of the class MatchData (see 15.2.16) if the match succeeds or nil if the match fails.
c)
An instance of the class MatchData is created as follows:
1) Let B be the substring of S which P matched.
2) Create a direct instance of the class MatchData, and let M be the instance.
3) Set the string attribute of M (see 15.2.16.1) to S.
4) Create a new empty list L.
5) Let O be a pair of the substring B and the index of the first character of B within S. Append O to L.
6) For each grouping G in P, in the order of the occurrence of its “(” within P, take the following steps:
9 i) If G matches a substring of B under the matching process of P, let BG be the substring. Let O be a pair of the substring BG and the index of the first character of BG within S. Append O to L.
ii) Otherwise, append to L a pair whose substring and index of the substring are nil.
7) Set the match result attribute of M to L.
8) M is the instance of the class MatchData returned by the matching process.
A matching process creates or updates a local variable binding with name “$~”, which is  specifically used by the method Regexp.last match (see 15.2.15.6.3), as follows:
1) Let M be the value which the matching process returns.
2) If the binding for the name “_̃” can be resolved by the process described in  as if “_̃” were a local-variable-identifier, replace the value of the binding with M.
3) Otherwise, create a local variable binding with name "_̃” and value M in the uppermost non-block element of [local-variable-bindings] where the non-block element means the element which does not correspond to a block.
d)
A conforming processor may name the binding other than “_̃”; however, it shall not be of the form local-variable-identifier.

Singleton methods
Regexp.compile( *args )
Behavior: Same as the method new (see 15.2.3.3.3).

Regexp.escape
Regexp.escape( string )
Behavior:
a) If string is not an instance of the class String, the behavior is unspecified.
b) Let S be the content of string.
c) Return a a new direct instance of the class String whose content is the same as S,
    except that every occurrences of characters on the left of Table 4 are replaced with the
   corresponding sequences of characters on the right of Table 4.

Table 4 – Regexp escaped characters
Characters replaced Escaped sequence
0x0a 0x09 \f
0x20 \ 0x20
# \#
~ \~
( \(
) \)
* \*
+ \+
- \-
. \.
? \?
[ \[
\ \\
] \]
^ \^
{ \{
| \|
} 
10
\r
0x0c 
15.2.15.6.3
\t
0x0d 
9
\n
\}

Regexp.last match
Regexp.last match( *index )
227
1 Visibility: public
2 Behavior:
3 a) Search for a binding of a local variable with name “ ̃” as described in 9.2 as if “ ̃” were
              a local-variable-identifier.
b) If the binding is found and its value is an instance of the class MatchData, let M be
  the instance. Otherwise, return nil.
7 c) If the length of index is 0, return M.
8 d) If the length of index is larger than 1, raise a direct instance of the class ArgumentError.
9 e) If the length of index is 1, let A be the only argument.
10 f) If A is not an instance of the class Integer, the behavior of the method is unspecified.
11 g) Let R be the result returned by invoking the method [] (see 15.2.16.3.1) on M with
     A as the only argument.
13 h) Return R.
14 15.2.15.6.4
4
5
6
12
Regexp.quote
15 Regexp.quote
16 Visibility: public
17 Behavior: Same as the method escape (see 15.2.15.6.2).
18 15.2.15.7
19 15.2.15.7.1
Instance methods
Regexp#initialize
20 initialize( source, flag =nil )
21 Visibility: private
22 Behavior:
23 a) If source is an instance of the class Regexp, let S be the pattern attribute of source.
     If source is an instance of the class String, let S be the content of source. Otherwise,
    the behavior is unspecified.
b) If S is not of the form pattern (see 15.2.15.4), raise a direct instance of the class
  RegexpError.
c) Set the pattern attribute of the receiver to S.
24
25
26
27
28
228
1
d)
If flag is an instance of the class Integer, let n be the value of the instance.
1) If computing bitwise AND of the value of the constant IGNORECASE of the class
  Regexp and n results in non-zero value, set the ignorecase-flag attribute of the
   receiver to true.
2) 
2
If computing bitwise AND of the value of the constant MULTILINE of the class
Regexp and n results in non-zero value, set the multiline-flag attribute of the
 receiver to true.
3
4
5
6
7
8
e) If flag is not an instance of the class Integer, and if flag is a trueish object, then set
      the ignorecase-flag attribute of the receiver to true.
f) Return an implementation-defined value.
9
10
11
15.2.15.7.2
Regexp#initialize copy
12 initialize copy( original )
13 Visibility: private
14 Behavior:
15 a) If original is not an instance of the class of the receiver, raise a direct instance of the
     class TypeError.
17 b) Set the pattern attribute of the receiver to the pattern attribute of original .
18 c) Set the ignorecase-flag attribute of the receiver to the ignorecase-flag attribute of orig-
         inal .
20 d) Set the multiline-flag attribute of the receiver to the multiline-flag attribute of original .
21 e) Return an implementation-defined value.
16
19
22
15.2.15.7.3
Regexp#==
23 = =( other )
24 Visibility: public
25 Behavior:
26 a) If other is not an instance of the class Regexp, return false.
27 b) If the corresponding attributes of the receiver and other are the same, return true.
28 c) Otherwise, return false.
229
1
15.2.15.7.4
Regexp#===
2 = = =( string )
3 Visibility: public
4 Behavior:
5 a) If string is not an instance of the class String, the behavior is unspecified.
6 b) Let S be the content of string.
7 c) Test if the pattern of the receiver matches S (see 15.2.15.4 and 15.2.15.5). Let M be
    the result of the matching process.
9 d) If M is an instance of the class MatchData, return true.
10 e) Otherwise, return false.
15.2.15.7.5
Regexp#= ̃
8
11
12 =~( string )
13 Visibility: public
14 Behavior:
15 a) If string is not an instance of the class String, the behavior is unspecified.
16 b) Let S be the content of string.
17 c) Test if the pattern of the receiver matches S (see 15.2.15.4 and 15.2.15.5). Let M be
     the result of the matching process.
19 d) If M is nil return nil.
20 e) If M is an instance of the class MatchData, let P be first element of the match result
       attribute of M, and let i be the index of the substring of P.
f) Return an instance of the class Integer whose value is i.
18
21
22
23
15.2.15.7.6
Regexp#casefold?
24 casefold?
25 Visibility: public
26 Behavior: The method returns the value of the ignorecase-flag attribute of the receiver.
230
1
15.2.15.7.7
Regexp#match
2 match( string )
3 Visibility: public
4 Behavior:
5 a) If string is not an instance of the class String, the behavior is unspecified.
6 b) Let S be the content of string.
7 c) Test if the pattern of the receiver matches S (see 15.2.15.4 and 15.2.15.5). Let M be
    the result of the matching process.
9 d) Return M.
10 15.2.15.7.8
11 source
12 Visibility: public
8
13
14
Behavior: The method returns a direct instance of the class String whose content is the
pattern of the receiver.
15 15.2.16
16 15.2.16.1
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
Regexp#source
MatchData
General description
Instances of the class MatchData represent results of successful matches of instances of the class
Regexp against instances of the class String.
An instance of the class MatchData has the attributes called string and match result, which
are initialized as described in 15.2.15.5. The string attribute is the target string S of a matching
process. The match result attribute is a list whose element is a pair of a substring B matched
by the pattern of an instance of the class Regexp or a grouping in the pattern, and the index I
of the first character of B within S. B is called the substring of the element, and I is called the
index of the substring of the element. Elements of the match result attribute are indexed by
integers starting from 0.
Given an instance M of the class MatchData, three values named matched substring, pre-
match and post-match of M, respectively, are defined as follows:
Let S be the string attribute of M. Let F be the first element of the match result attribute of
M. Let B and O be the substring of F and the index of the substring of F. Let i be the sum of
O and the length of B.
matched substring: The matched substring of M is B.
231
pre-match: The pre-match of M is a part of S, from the first up to, but not including the
Oth character of S.
1
2
post-match: The post-match of M is a part of S, from the i th up to the last character of
S.
3
4
5 15.2.16.2
6 The class Object
7 15.2.16.3
8 15.2.16.3.1
Instance methods
MatchData#[]
[]( *args )
9
Visibility: public
10
Behavior: Invoke the method to a on the receiver (see 15.2.16.3.12), and invoke the
method [] on the resulting instance of the class Array with args as the arguments (see
15.2.12.5.4), and then, return the resulting value of the invocation of the method [].
11
12
13
14
Direct superclass
15.2.16.3.2
MatchData#begin
15 begin( index )
16 Visibility: public
17 Behavior:
18 a) If index is not an instance of the class Integer, the behavior is unspecified.
19 b) Let L be the match result attribute of the receiver, and let i be the value of index .
20 c) If i is smaller than 0, or larger than or equal to the number of elements of L, raise a
     direct instance of the class IndexError.
22 d) Otherwise, return the second portion of the i th element of L.
23 15.2.16.3.3
21
MatchData#captures
24 captures
25 Visibility: public
26 Behavior:
232
1 a) Let L be the match result attribute of the receiver.
2 b) Create an empty direct instance A of the class Array.
3 c) Except for the first element, for each element e of L, in the same order in the list,
      append to A a direct instance of the class String whose content is the substring of e.
5 d) Return A.
6 15.2.16.3.4
4
MatchData#end
7 end( index )
8 Visibility: public
9 Behavior:
10 a) If index is not an instance of the class Integer, the behavior is unspecified.
11 b) Let L be the match result attribute of the receiver, and let i be the value of index .
12 c) If i is smaller than 0, or larger than or equal to the number of elements of L, raise a
     direct instance of the class IndexError.
d) Let F and S be the substring and the index of the substring of the i th element of L,
  respectively.
16 e) If F is nil, return nil.
17 f) Otherwise, let f be the length of F. Return an instance of the class Integer whose
     value is the sum of S and f.
13
14
15
18
19
15.2.16.3.5
MatchData#initialize copy
20 initialize copy( original )
21 Visibility: private
22 Behavior:
23 a) If original is not an instance of the class of the receiver, raise a direct instance of the
     class TypeError.
25 b) Set the string attribute of the receiver to the string attribute of original .
26 c) Set the match result attribute of the receiver to the match result attribute of original .
27 d) Return an implementation-defined value.
24
233
1 15.2.16.3.6
2 length
3 Visibility: public
4 Behavior:
5 The method returns the number of elements of the match result attribute of the receiver.
6
15.2.16.3.7
MatchData#length
MatchData#offset
7 offset( index )
8 Visibility: public
9 Behavior:
10 a) If index is not an instance of the class Integer, the behavior is unspecified.
11 b) Let L be the match result attribute of the receiver, and let i be the value of index .
12 c) If i is smaller than 0, or larger than or equal to the number of elements of L, raise a
     direct instance of the class IndexError.
d) Let S and b be the substring and the index of the substring of the i th element of L,
  respectively. Let e be the sum of b and the length of S.
e) Return a new instance of the class Array which contains two instances of the class
  Integer, the one whose value is b and the other whose value is e, in this order.
13
14
15
16
17
18
15.2.16.3.8
MatchData#post match
19 post match
20 Visibility: public
Behavior: The method returns an instance of the class String the content of which is the
post-match of the receiver.
21
22
23
15.2.16.3.9
MatchData#pre match
24 pre match
25 Visibility: public
Behavior: The method returns an instance of the class String the content of which is the
pre-match of the receiver.
26
27
234
1 15.2.16.3.10
2 size
3 Visibility: public
4 Behavior: Same as the method length (see 15.2.16.3.6).
5
15.2.16.3.11
MatchData#size
MatchData#string
6 string
7 Visibility: public
8 Behavior:
9
10
The method returns an instance of the class String the content of which is the string
attribute of the receiver.
11 15.2.16.3.12
12 to a 13 Visibility: public
14 Behavior:
15 a) Let L be the match result attribute of the receiver.
16 b) Create an empty direct instance A of the class Array.
17 c) For each element e of L, in the same order in the list, append to A an instance of the
     class String whose content is the substring of e.
d) Return A.
18
19
MatchData#to a
20 15.2.16.3.13
21 to s
22 Visibility: public
23
24
MatchData#to s
Behavior: The method returns an instance of the class String the content of which is the
matched substring of the receiver.
235

*/

