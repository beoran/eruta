#include "si_mem.h"
#include "si_test.h"

TEST_FUNC(si_malloc) {
  void * aid = si_malloc(10);
  TEST_NOTNULL(aid);
  TEST_NULL(si_free(aid));  
  TEST_DONE();
}


TEST_FUNC(si_memcpy) {
  char src[10] = "banana";
  char dst[10];
  char * res = si_memcpy(dst, src, 7);
  TEST_NOTNULL(res);
  TEST_INTEQ(res[0], 'b');
  TEST_INTEQ(res[5], 'a');
  TEST_DONE();
}

TEST_FUNC(si_copyalloc) {
  char src[10] = "banana";
  char *   res = si_copyalloc(5, src, 3);
  TEST_NOTNULL(res);
  TEST_INTEQ(res[0], 'b');
  TEST_INTEQ(res[1], 'a');
  TEST_INTEQ(res[2], 'n');
  TEST_INTEQ(res[3],  0 );
  si_free(res);
  TEST_DONE();
}


TEST_FUNC(si_smemcpy) {
  char src[8] = "banana";
  char dst[10] = { 0, 1, 2, 3, 4, 5, 6 , 7 ,  8, 9 };
  char * res = si_smemcpy(dst, 14,  7, src, 8, 1, 77);
  TEST_NOTNULL(res);
  TEST_INTEQ(res[0], 0);
  TEST_INTEQ(res[6], 6);
  TEST_INTEQ(res[7], 'a');
  TEST_INTEQ(res[8], 'n');
  TEST_INTEQ(res[9], 'a');
  TEST_DONE();
}


TEST_FUNC(si_realloc) {
  char * aid = si_malloc(10);
  char * new = NULL;  
  TEST_NOTNULL(aid);
  TEST_NULL(new);
  aid[2] = 10;
  TEST_INTEQ(aid[2], 10);
  new    = si_realloc(aid, 20);
  TEST_NOTNULL(new);
  TEST_INTEQ(new[2], 10);
  TEST_NULL(si_free(new));
  TEST_DONE();
}

struct TestMem1_ {
  int i;
  char c;
};
typedef struct TestMem1_ TestMem1;

TEST_FUNC(si_mem) {
  char * aid = "Hi!";
  char buf[32];
  SiMem * mem, *me2, *me3, *me4;
  TestMem1 tm1 = { 1, 2 } ;
  TestMem1 *tmptr;  
  mem = simem_new(16);
  simem_putc(mem, 0 , 'x');
  TEST_INTEQ(simem_getc(mem, 0), 'x');
  TEST_INTEQ(simem_room(mem), 16);
  TEST_ZERO(simem_cmpc(mem, 0, 'x'));
  TEST_INTEQ(simem_putdata(mem, 1 , aid, 4), 4);
  TEST_ZERO(simem_cmpc(mem, 0, 'x'));
  TEST_ZERO(simem_cmpc(mem, 1, 'H'));
  TEST_ZERO(simem_cmpc(mem, 2, 'i'));
  TEST_ZERO(simem_cmpc(mem, 3, '!'));
  TEST_ZERO(simem_cmpc(mem, 4, '\0'));
  TEST_INTEQ(simem_putdata(mem, 16, aid, 4), 0);
  TEST_INTEQ(simem_putdata(mem, 1 , aid, 0), 0);
  TEST_INTEQ(simem_putdata(mem, 15, aid, 4), 1);
  
  TEST_INTEQ(simem_getdata(mem, 1 , buf, 4), 4);
  TEST_STREQ(buf, aid);  
  TEST_INTEQ(simem_getdata(mem, 16, buf, 4), 0);  
  TEST_INTEQ(simem_getdata(mem, 1 , buf, 0), 0);
  TEST_STREQ(buf, aid);
  TEST_INTEQ(simem_getdata(mem, 15, buf, 4), 1);
  TEST_INTEQ(buf[0], 'H');
  TEST_ZERO(simem_cmpdata(mem, 0, "xHi!\0", 5));
  TEST_NOTNULL(simem_realloc(mem, 32));
  TEST_INTEQ(simem_room(mem), 32);
  TEST_NULL(simem_cmpdata(mem, 0, "xHi!\0", 5));
  TEST_ZERO(simem_cmpc(mem, 15, 'H'));
  me2   = simem_dup(mem);
  simem_fillall(mem, 0);
  TEST_ZERO(simem_cmpc(mem, 0, 0));
  TEST_NULL(simem_cmpdata(me2, 0, "xHi!\0", 5));
  TEST_NULL(simem_cmpdata(me2, 15, "H", 1));
  me3   = simem_newptr(64); 
  TEST_NOTNULL(simem_putptr(me3, 6, me2));
  TEST_NOTNULL(simem_putptr(me3, 7, me3));
  TEST_NULL(simem_putptr(me3, 88, mem));
  TEST_PTREQ(me2, simem_getptr(me3, 6));
  TEST_PTREQ(me3, simem_getptr(me3, 7));
  TEST_NULL(simem_getptr(me3, 88));
  TEST_NULL(simem_getptr(me3, 1));
  me4   = simem_newelement(10, sizeof(TestMem1));
  TEST_NOTNULL(simem_putelement(me4, 7, &tm1, sizeof(TestMem1)));
  TEST_NULL(simem_putelement(me4, 10, &tm1, sizeof(TestMem1)));
  tmptr = (TestMem1 *) simem_getelement(me4, 7, sizeof(TestMem1));
  TEST_NULL(simem_getelement(me4, 10, sizeof(TestMem1)));
  TEST_NOTNULL(tmptr);
  TEST_INTEQ(tmptr->i, 1);
  TEST_INTEQ(tmptr->c, 2);
  TEST_PTRNEQ(&tm1, tmptr);

  simem_free(mem);
  simem_free(me2);
  simem_free(me3);
  simem_free(me4);
  TEST_DONE();
}

int main(void) {
  TEST_INIT();
  TEST_RUN(si_malloc);
  TEST_RUN(si_memcpy);
  TEST_RUN(si_smemcpy);
  TEST_RUN(si_copyalloc);  
  TEST_RUN(si_realloc);
  TEST_RUN(si_mem);
  TEST_REPORT();
}




