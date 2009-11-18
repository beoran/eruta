#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

int main(void) {
  void * ptr;
  int  * iptr;
  ptr = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS , -1, 0);
  perror("test1 ");
  
  iptr = ptr;
  iptr[64] = 10;
  printf("%p %d %d\n", ptr, iptr[64], iptr[63]);
  munmap(ptr, 1024);
  perror("test1 ");
  return 0;
}
