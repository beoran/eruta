// Copyright (c) 2004-2012 Sergey Lyubka <valenok@gmail.com>
// All rights reserved
//
// Enhanced and modified by beoran@gmail.com, 2013.
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

#ifndef SLRE_HEADER_DEFINED
#define SLRE_HEADER_DEFINED

// This is a regular expression library that implements a subset of Perl RE.
// Please refer to http://slre.googlecode.com for detailed description.
//
// Supported syntax:
//    ^        Match beginning of a buffer
//    $        Match end of a buffer
//    ()       Grouping and substring capturing
//    [...]    Match any character from set
//    [^...]   Match any character but ones from set
//    \s       Match whitespace
//    \S       Match non-whitespace
//    \d       Match decimal digit
//    \D       Match anything but a decimal digit
//    \a       Match alphabetical character
//    \A       Match anything but an alphabetical character
//    \w       Match alphanumerical character
//    \W       Match anything but an alphanumerical character
//    \b       Match a blank character, i.e. space or tab
//    \B       Match anything but a blank character
//    \x       Match a hexadecimal digit
//    \X       Match anything but a hexadecimal digit
//    \t       Match a tab
//    \r       Match carriage return
//    \n       Match newline
//    +        Match one or more times (greedy)
//    +?       Match one or more times (non-greedy)
//    *        Match zero or more times (greedy)
//    *?       Match zero or more times (non-greedy)
//    ?        Match zero or once
//    \xDD     Match byte with hex value 0xDD
//    \meta    Match one of the meta character: ^$().[*+\?

// Match string buffer "buf" of length "buf_len" against "regexp", which should
// conform the syntax outlined above. "options" could be either 0 or
// SLRE_CASE_INSENSITIVE for case-insensitive match. If regular expression
// "regexp" contains brackets, slre_match() will capture the respective
// substring into the passed placeholder. Thus, each opening parenthesis
// should correspond to three arguments passed:
//   placeholder_type, placeholder_size, placeholder_address
//
// Usage example: parsing HTTP request line.
//
//  char method[10], uri[100];
//  int http_version_minor, http_version_major;
//  const char *error;
//  const char *request = " \tGET /index.html HTTP/1.0\r\n\r\n";

//  error = slre_match(0, "^\\s*(GET|POST)\\s+(\\S+)\\s+HTTP/(\\d)\\.(\\d)",
//                     request, strlen(request),
//                     SLRE_STRING,  sizeof(method), method,
//                     SLRE_STRING, sizeof(uri), uri,
//                     SLRE_INT,sizeof(http_version_major),&http_version_major,
//                     SLRE_INT,sizeof(http_version_minor),&http_version_minor);
//
//  if (error != NULL) {
//    printf("Error parsing HTTP request: %s\n", error);
//  } else {
//    printf("Requested URI: %s\n", uri);
//  }
//
// If the option SLRE_NO_CAPTURE is passed, captures are not stored.
//
// If SLRE_CALLBACK is passed as the first variable arument, then the 2 next 
// arguments must be a function pointer of the type * slre_callback, and a void *
// that points to extra data or to NULL. The callback will be called once 
// for every match.
//
// If SLRE_CAPTURED is passed, addresses to slre_captured structs must be passed 
// for storage of the results.
// 
// If SLRE_IGNORE is passed, then all further captures are ignored.
//
// Return:
//   NULL: string matched and all captures successfully made
//   non-NULL: in this case, the return value is an error string

/* Match options. */
enum slre_option { 
  SLRE_CASE_INSENSITIVE = 1, SLRE_NO_CAPTURE = 2
};

/* Possible capture types. */
enum slre_capture { 
   SLRE_STRING, SLRE_INT, SLRE_FLOAT, SLRE_CALLBACK, SLRE_CAPTURED, SLRE_IGNORE
};

/* Captured substring */
struct slre_captured {
  const char *ptr;  // Pointer to the substring
  int len;          // Substring length
};


/* Possible results of slre_match. */
enum slre_result {
  SLRE_OK                       = 0,
  SLRE_ERROR_NO_MATCH           = 1,      
  SLRE_ERROR_JUMP_OFFSET        = 2,
  SLRE_ERROR_CODE_TOO_LONG      = 3,     
  SLRE_ERROR_DATA_TOO_LONG      = 4,
  SLRE_ERROR_NO_PAREN           = 5,      
  SLRE_ERROR_BAD_PAREN          = 6,     
  SLRE_ERROR_NO_BRACKET         = 7,    
  SLRE_ERROR_TOO_MANY_PAREN     = 8,
  SLRE_ERROR_INT_FAILED         = 9,  
  SLRE_ERROR_INT_SIZE           = 10,    
  SLRE_ERROR_FLOAT_SIZE         = 11,  
  SLRE_ERROR_FLOAT_FAILED       = 12,
  SLRE_ERROR_STRING_SIZE        = 13, 
  SLRE_ERROR_UNKNOWN_TYPE       = 14,
  SLRE_ERROR_TEXT_TOO_LONG      = 15,    
  SLRE_ERROR_NULL_CAPTURED      = 16,
  SLRE_ERROR_LAST               = 225,
};

/* Maximum amount of captures. */
#ifndef SLRE_CAPURES_MAX
#define SLRE_CAPURES_MAX 64
#endif


/* Callback type to use for SLRE_CALLBACK. */
typedef int slre_callback(int index, const char * capture, int size, void * data);

/* Matching function. */
int slre_match(enum slre_option options, const char *regexp,
                       const char *buf, int buf_len, ...);

/* Converts error code to string or NULL if not known. */
const char * slre_error(int code);

#endif /* SLRE_HEADER_DEFINED */
