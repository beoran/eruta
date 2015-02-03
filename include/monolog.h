#ifndef monolog_H_INCLUDED
#define monolog_H_INCLUDED

#include <stdarg.h>

#include "eruta.h"

/* Finer grained logging. */

/** The interface for a logger */
struct MonologLoggerInterface {
  /* Logger function. */ 
  int   (*log)(char * file, int line, char * level, void * data, char * format, va_list args);
  /* Optional destructor. */
  void (*free)(void * data);
}; 

int monolog_add_logger(void * data, struct MonologLoggerInterface * logger);
int monolog_remove_logger(int index);
int monolog_enable_level(char * name);
int monolog_disable_level(char * name);
int monolog_log_va(char * file, int line, char * level, char * format, va_list args);
int monolog_log(char * file, int line, char * level, char * format, ...);
int monolog_init(void);
void monolog_done();

int monolog_stdout_logger(char * file, int line, char * level, void * data, char * format, va_list args);

int monolog_stderr_logger(char * file, int line, char * level,  void * data, char * format, va_list args);

/** Log function for logger that logs to a FILE. */
int monolog_file_logger(char * file, int line, char * level,  void * data, char * format, va_list args);

/** Free function for logger that logs to a FILE. */
void monolog_file_free(void * data);


/* Logger helper macros */
#define DEFINE_LOGGER(NAME, LOGF, FREEF) \
  static struct MonologLoggerInterface NAME  = { LOGF, FREEF }

#define DEFINE_STDERR_LOGGER(NAME) DEFINE_LOGGER(NAME, monolog_stderr_logger, NULL)
#define DEFINE_STDOUT_LOGGER(NAME) DEFINE_LOGGER(NAME, monolog_stdout_logger, NULL)
#define DEFINE_FILE_LOGGER(NAME)   DEFINE_LOGGER(NAME, monolog_file_logger, monolog_file_free)

/* Logging helper macros */

#define LOG_LEVEL(LEVEL, FORMAT, ...) \
  monolog_log(__FILE__, __LINE__, LEVEL, FORMAT,  __VA_ARGS__)

#define LOG(FORMAT, ...)          LOG_LEVEL(__FILE__,   FORMAT, __VA_ARGS__)
#define LOG_ERROR(FORMAT, ...)    LOG_LEVEL("ERROR",    FORMAT, __VA_ARGS__)
#define LOG_WARNING(FORMAT, ...)  LOG_LEVEL("WARNING",  FORMAT, __VA_ARGS__)
#define LOG_NOTE(FORMAT, ...)     LOG_LEVEL("NOTE",     FORMAT, __VA_ARGS__)
#define LOG_DEBUG(FORMAT, ...)    LOG_LEVEL("DEBUG",    FORMAT, __VA_ARGS__)

#define LOG_ENABLE(FORMAT)            \
    monolog_enable_level(#FORMAT)

#define LOG_ENABLE_ERROR()        LOG_ENABLE(ERROR)  
#define LOG_ENABLE_WARNING()      LOG_ENABLE(WARNING)
#define LOG_ENABLE_NOTE()         LOG_ENABLE(NOTE)
#define LOG_ENABLE_DEBUG()        LOG_ENABLE(DEBUG)

#define LOG_DISABLE(FORMAT)            \
    monolog_disable_level(#FORMAT)
    
#define LOG_DISABLE_ERROR()       LOG_DISABLE(error)  
#define LOG_DISABLE_WARNING()     LOG_DISABLE(warning)
#define LOG_DISABLE_NOTE()        LOG_DISABLE(note)
#define LOG_DISABLE_DEBUG()       LOG_DISABLE(debug)




#endif




