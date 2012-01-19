/**
* Generic includes for SI.H
*/

#ifndef SI_H_INCLUDED
#define SI_H_INCLUDED

/*(uint(n)_t  is used extensively, use stdint, or pstdint. */
#ifdef SI_USE_PSTDINT
  #include <pstdint.h>
#else 
  #include <stdint.h> 
#endif 


/* size_t is used extensively. */
#include <stdlib.h>




#endif
