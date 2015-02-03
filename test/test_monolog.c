/**
* This is a test for monolog in $package$
*/
#include "si_test.h"
#include "monolog.h"




DEFINE_FILE_LOGGER(file_logger);
DEFINE_STDOUT_LOGGER(stdout_logger);
DEFINE_STDERR_LOGGER(stderr_logger);


TEST_FUNC(monolog) {
  monolog_init();
  FILE * fout;
  fout = fopen("test_monolog.log", "w");
  monolog_add_logger(NULL, &stdout_logger);
  monolog_add_logger(fout, &file_logger);
  LOG_ENABLE_ERROR();
  monolog_enable_level("WARNING");
  LOG_LEVEL("WARNING", "Warning %d\n", 77);  
  LOG_ERROR("Error %d\n", 77);
  LOG("This is not shown", 77);

  /* fclose(fout); */ 
  monolog_done();
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(monolog);
  TEST_REPORT();
}



