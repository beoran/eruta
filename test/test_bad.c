/**
* This is a test for bad in $package$
*/
#include "si_test.h"
#include "bad.h"


TEST_FUNC(bad) {
  TEST_DONE();
}


TEST_FUNC(badchildtree) {
  int          index;
  BadChildtree tnodes[32];
  BadChildtree * nodes[32];
  for(index = 0; index < 32; index ++) {
    badchildtree_initnull(tnodes + index);
      nodes[index] = tnodes + index;
  }
  badchildtree_appendchild(nodes[0], nodes[1]);
  badchildtree_appendchild(nodes[0], nodes[2]);
  badchildtree_appendchild(nodes[0], nodes[3]);
  badchildtree_appendchild(nodes[0], nodes[4]);
  badchildtree_appendchild(nodes[2], nodes[5]);
  badchildtree_appendchild(nodes[2], nodes[6]);
  badchildtree_appendchild(nodes[2], nodes[7]);
  badchildtree_appendchild(nodes[4], nodes[8]);
  badchildtree_appendchild(nodes[4], nodes[9]);
  badchildtree_appendchild(nodes[9], nodes[10]);
  badchildtree_appendchild(nodes[9], nodes[11]);
  badchildtree_printgraph(nodes[0], 0);
  
  
  TEST_DONE();
}

TEST_FUNC(badpar) {
  void * arr;
  size_t size = 123; 
  arr = badpar_new(size);
  TEST_NOTNULL(arr);
  TEST_NOTNULL(badpar_put(arr, size, 17, "Hello"));
  TEST_NOTNULL(badpar_get(arr, size, 17));
  TEST_NULL(badpar_get(arr, size, 321));
  puts(badpar_get(arr, size, 17));
  TEST_NOTNULL(badpar_resize(&arr, &size, 100));  
  TEST_NOTNULL(badpar_put(arr, size, 210, "World"));
  TEST_NOTNULL(badpar_get(arr, size, 210));
  puts(badpar_get(arr, size, 210));  
  TEST_NULL(badpar_free(arr));
  TEST_DONE();
}

struct TestBadgar {
  int           i;
  double        d; 
  char        * cstr;
};

TEST_FUNC(badgar) {
  struct TestBadgar value1 = { 797, 3.2, "Hello Gar!" }; 
  struct TestBadgar value2 = { 204, 2.3, "World Far!" }; 
  struct TestBadgar * ptr; 
  void  * arr;
  size_t nmemb = 123; 
  size_t size = sizeof(value1);
  arr = badgar_new(nmemb, size);
  TEST_NOTNULL(arr);  
  TEST_NOTNULL(badgar_put(arr, nmemb, size, 17, &value1));
  ptr = badgar_get(arr, nmemb, size, 17);
  TEST_NOTNULL(ptr);
  TEST_NOTNULL(ptr->cstr);  
  puts(ptr->cstr);  
  TEST_NULL(badgar_get(arr, nmemb, size, 321));
  TEST_NOTNULL(badgar_resize(&(arr), &nmemb, size,  100));  
  TEST_NOTNULL(badgar_put(arr, nmemb, size, 210, &value2));
  ptr = badgar_get(arr, nmemb, size, 210);
  TEST_NOTNULL(ptr);
  puts(ptr->cstr);  
  TEST_NULL(badpar_free(arr));
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(bad);
  TEST_RUN(badchildtree);
  TEST_RUN(badpar);
  TEST_RUN(badgar);
  TEST_REPORT();
}



