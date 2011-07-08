/*
* Description: 
* Tests for the SiTree.
*
* Author: Beoran <beoran@rubyforge.org>, (C) 2011
*
* Copyright: See COPYING file that comes with this distribution
*
*/


#include <stdio.h>
#include <stddef.h>
#include "si_test.h"
#include "si_tree.h"

TEST_FUNC(sitree) {
  char   * s1   = "Hello";
  char   * s2   = "World";
  char   * s3   = "This";
  char   * s4   = "Is";
  char   * s5   = "Me";
  SiTree * t1   = sitree_new(s1);
  SiTree * t2   = NULL;
  SiTree * t3   = NULL;
  TEST_NOTNULL(t1);
  TEST_NOTNULL(sitree_data(t1));
  t2 = sitree_add(t1, s2);
  t3 = sitree_add(t2, s3);
  TEST_NOTNULL(t2);
  TEST_STREQ(sitree_data(t1), s1);
  TEST_NOTNULL(t3);
  TEST_STREQ(sitree_data(t3), s3);
  sitree_free(t1);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(sitree);
  TEST_REPORT();
}

