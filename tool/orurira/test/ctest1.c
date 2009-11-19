#include <stdlib.h>
#include <stdio.h>



char * str = "Hello ctest!";
double flo = 1.234;

int foo(int bar) {
  return bar + 10;
}


int main(void) {
  int i;
  i = foo(10);
  printf("%s %lf\n", str, flo);
  return 123;
}


