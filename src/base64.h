/* base64.h
 * Michael McTernan, mm7323@bris.ac.uk
 * Functions to encode and decode strings
 *  into base 64 (RFC1521 section 7).
 */

#ifndef BASE64_HEADER
#define BASE64_HEADER


/*B64_Decode*/
/* Decode string into a new buffer and
 *  return it.  Memory for the new
 *  buffer is allocated.
 */
char *B64_Decode(const char *string);


/*B64_Encode*/
/* Takes string and encodes it in base 64.
 *  Returns the new string in allocated memory.
 */
char *B64_Encode(const char *string);


#endif
