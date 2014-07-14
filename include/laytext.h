#ifndef laytext_H_INCLUDED
#define laytext_H_INCLUDED


#include "dynar.h"

/* Type of the callback that should calulate the dimensions of an 
 * arbitrary text string encoded in utf-8. It should only take into consideration up to bytes bytes of that string */
typedef int laytext_callback(char * str, int bytes, void * extra, float * w, float * h);

Dynar * laytext_layout(char * str, float max_width, laytext_callback * callback, void * extra);



#endif




