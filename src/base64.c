/* base64.c
 * Michael McTernan, mm7323@bris.ac.uk
 * Functions to encode and decode strings
 *  into base 64 (RFC1521 section 7).
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "base64.h"


/* Lookup tables for fast translation */
static const char base64EncTable[64]="ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                     "abcdefghijklmnopqrstuvwxyz"
                                     "0123456789+/";
static const char base64DecTable[256] = {
                   00,00,00,00, 00,00,00,00, 00,00,00,00, 00,00,00,00,
                   00,00,00,00, 00,00,00,00, 00,00,00,00, 00,00,00,00,
                   00,00,00,00, 00,00,00,00, 00,00,00,62, 00,00,00,63,
                   52,53,54,55, 56,57,58,59, 60,61,00,00, 00,00,00,00,
                    0, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
                   15,16,17,18, 19,20,21,22, 23,24,25,00, 00,00,00,00,
                   00,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
                   41,42,43,44, 45,46,47,48, 49,50,51,00, 00,00,00,00,
                   00,00,00,00, 00,00,00,00, 00,00,00,00, 00,00,00,00,
                   00,00,00,00, 00,00,00,00, 00,00,00,00, 00,00,00,00,
                   00,00,00,00, 00,00,00,00, 00,00,00,00, 00,00,00,00,
                   00,00,00,00, 00,00,00,00, 00,00,00,00, 00,00,00,00,
                   00,00,00,00, 00,00,00,00, 00,00,00,00, 00,00,00,00,
                   00,00,00,00, 00,00,00,00, 00,00,00,00, 00,00,00,00,
                   00,00,00,00, 00,00,00,00, 00,00,00,00, 00,00,00,00,
                   00,00,00,00, 00,00,00,00, 00,00,00,00, 00,00,00,00,
                  };


/*B64_Decode*/
/* Decode string into a new buffer and
 *  return it.  Memory for the new
 *  buffer is allocated.
 */
char *B64_Decode(const char *string) {
  int   stringLen=strlen(string);
  char *result=malloc((ceil(stringLen/4.0)*3)+1);
  int   t,o;

  if(result==NULL) {
    /* TODO: Some proper handling here */
    abort();
  }

  t=o=0;
  do {
    result[o++]=((base64DecTable[(int)string[t]]   << 2 ) & 0xfc ) |
                ((base64DecTable[(int)string[t+1]] >> 4 ) & 0x03 );

    result[o++]=((base64DecTable[(int)string[t+1]] << 4 ) & 0xf0 ) |
                ((base64DecTable[(int)string[t+2]] >> 2 ) & 0x0f );

    result[o++]=((base64DecTable[(int)string[t+2]] << 6 ) & 0xc0 ) |
                ((base64DecTable[(int)string[t+3]]));

    t+=4;
  } while(t<strlen(string));

  return result;
}


/*B64_Encode*/
/* Takes string and encodes it in base 64.
 *  Returns the new string in allocated memory.
 */
char *B64_Encode(const char *string) {
  int   stringLen=strlen(string);
  char *result=malloc((int)(ceil(stringLen/3.0)*4)+1);
  int   t,o;

  if(result==NULL) {
    /* TODO: Some proper handling here */
    abort();
  }

  t=o=0;
  do {
    result[o++]=base64EncTable[(string[t]    >> 2) & 0x3f];

    result[o++]=base64EncTable[((string[t]   << 4) & 0x30) |
                               ((string[t+1] >> 4) & 0x0f)];

    if(t+1<stringLen)
      result[o++]=base64EncTable[((string[t+1] << 2) & 0x3c) |
                                 ((string[t+2] >> 6) & 0x03)];
    else
      result[o++]='=';

    if(t+2<stringLen)
      result[o++]=base64EncTable[((string[t+2] ) & 0x3f)];
    else
      result[o++]='=';

    t+=3;
  } while(t<strlen(string));

  /* Null terminate */
  result[o++]='\0';

  return result;
}

