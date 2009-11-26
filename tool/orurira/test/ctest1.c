#include <stdlib.h>
#include <stdio.h>

/* In C data pointers and function pointers are NOT guaranteed 
   to be of the same size, due to some platforms  */
typedef void *     or_data_ptr ;
typedef or_data_ptr (*or_func_ptr)(or_data_ptr) ; 
typedef double or_float;

union or_all_ptr_;
typedef union or_all_ptr_ or_all_ptr; 

union or_all_ptr_ {
  or_data_ptr data;
  or_func_ptr func;  
}; 

union or_cell;
typedef union or_cell_ or_cell;

union or_cell_ {
  or_all_ptr  ptr;
  int         num;
};

struct or_obj_s_ {
  or_cell   flags;
  or_cell   space;
  or_cell   size;
  or_cell   contents[];
};

union or_obj_u_ {

};



char * str = "Hello ctest!";
double flo = 1.234;
int    iii = 324;
or_cell cell;
  
int foo(int bar) {
  return bar + 10;
}


int main(void) {
  cell.num  = 1000;
  cell.num += iii;
  printf("%s %d %d %p\n", str, sizeof(or_cell), cell.num, cell.ptr.data);
  return 123;
}


