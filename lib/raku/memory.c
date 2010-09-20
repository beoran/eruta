#include "raku.h"
#include <stdlib.h>

void * ra_mem_allot(RaSize size) {
  return calloc(size, 1);
}

void ra_mem_free(void * mem) {
  free(mem);
}
