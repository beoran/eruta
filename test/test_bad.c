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


int main(void) {
  TEST_INIT();
  TEST_RUN(bad);
  TEST_RUN(badchildtree);
  TEST_REPORT();
}



