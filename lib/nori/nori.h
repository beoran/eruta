#ifndef _NORI_H_
#define _NORI_H_

/* 
  Some platforms will need a __declspec(dllexport) or something for 
  functions in a shared library.
*/
#ifndef NORI_EXPORT_FUNC
#define NORI_EXPORT_FUNC extern
#endif
  
/*
* Some platforms may require a change in calling convention
*/  
#ifndef NORI_CALL_FUNC
#define NORI_CALL_FUNC
#endif 

/* All in a handy wrapper macro */
#define NORI_FUNC(RESULT) NORI_EXPORT_FUNC RESULT NORI_CALL_FUNC
 
/* An another wrapper macro to help typedefing such functions */
#define NORI_FUNCTYPE(RESULT) typedef RESULT NORI_CALL_FUNC



// bag, bin, bar, row, box, jar, keg, kit, pea, pod, pot,rig, can, tin, tub, wad

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// For sizes 
typedef size_t NoriSize;

/** Rows are generic low level arrays. */








#endif
