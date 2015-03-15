
#include "monolog.h"
#include "dynar.h"


struct Monologger {
  struct MonologLoggerInterface logger;
  void                        * data;
  int                           index;
};

struct Monolevel {
  char * name;
  int enabled;
};


struct Monolog {
  Dynar * loggers;
  Dynar * levels;
  int last_logger;
};

static struct Monolog monolog_struct = { NULL, NULL, 0 };
static struct Monolog *      monolog = &monolog_struct;

void monolog_done() {
  int index;
  if (monolog->loggers) {
    /* clean up all loggers */
    for (index = 0; index < dynar_size(monolog->loggers); index ++) {
      monolog_remove_logger(index);
    }
     
    dynar_free(monolog->loggers);
    monolog->loggers = NULL;
  }
  
  if (monolog->levels) {
    dynar_free(monolog->levels);
    monolog->levels = NULL;
  }

  monolog->last_logger = 0;
}

/* Compares loggers with each other for sorting.  This is qsort compatible. */
static int monolog_compare_loggers(void * p1, void * p2) {
  struct Monologger * me   = *((struct Monologger **) p1);
  struct Monologger * you  = *((struct Monologger **) p2);
  if (!me) return -1;
  if (!you) return 1;
  if (!me->logger.log) return -1;
  if (!you->logger.log) return 1;

  /* Compare index if no nulls. */
  if (me->index < you->index) return -1;
  if (me->index > you->index) return  1;
  return 0;
}


/* Compares loglevels with each other for sorting.  This is qsort compatible. */
static int monolog_compare_loglevels(const void * p1, const void * p2) {
  const struct Monolevel * me   = p1;
  const struct Monolevel * you  = p2;
  if(!me) return -1;
  if(!you) return 1;
  if(!me->name) return -1;
  if(!you->name) return 1;
  /* Compare name if no nulls. */
  return strcmp(me->name, you->name);
  return 0;
}




#define MONOLOG_START_LEVELS  32
#define MONOLOG_START_LOGGERS 32

static int monolog_get_loggers_max() {
  return dynar_size(monolog->loggers);
}

static struct Monologger * monolog_get_logger(int index) {
  return dynar_getdata(monolog->loggers, index);
}

static int monolog_get_unused_logger() {
  int index;
  for (index = 0 ; index < dynar_size(monolog->loggers); index++) {
    struct Monologger * logger = monolog_get_logger(index);
    if (!logger->logger.log) {
      return index;
    }
  }
  return -1;
}


static struct Monologger *
monolog_set_logger(int index, void *data, struct MonologLoggerInterface * ifa) {
  struct Monologger * logger = monolog_get_logger(index);
  if(!logger) return NULL;
  logger->index  = index;
  logger->data   = data;
  logger->logger = (*ifa);
  return logger;
}

static struct Monolevel * monolog_get_level(int index) {
  return dynar_getdata(monolog->levels, index);
}

static int monolog_get_unused_level() {
  int index;
  for (index = 0 ; index < dynar_size(monolog->levels); index++) {
    struct Monolevel * level = monolog_get_level(index);
    if (!level->name) {
      return index;
    }
  }
  return -1;
}


static struct Monolevel *
monolog_set_level (int index, char * name, int enabled) {
  struct Monolevel * level = monolog_get_level(index);
  if (!level) return NULL;
  level->name     = name;
  level->enabled  = enabled;
  return level;
}


int monolog_init() {
  int index;
  monolog->loggers = dynar_new(MONOLOG_START_LEVELS, sizeof(struct Monologger));
  if(!monolog->loggers) {
    return FALSE;
  }
  
  monolog->levels  = dynar_new(MONOLOG_START_LOGGERS, sizeof(struct Monolevel));
    if(!monolog->levels) {
    monolog_done();
    return FALSE;
  }

  for (index = 0; index < dynar_amount(monolog->loggers) ; index++ ) {
    struct MonologLoggerInterface ifa = { NULL, NULL };
    monolog_set_logger(index, NULL, &ifa);
  }
  
  for (index = 0; index < dynar_amount(monolog->levels) ; index++ ) {
    monolog_set_level(index, NULL, FALSE);
  }
  
  return TRUE;
}



/** Adds a loger. Returns negative or error zero or positive (the index of the logger)
 * on success. */
int monolog_add_logger(void * data, struct MonologLoggerInterface * logger) {
  int logindex;

  if (monolog->last_logger >= dynar_amount(monolog->loggers))   {
    if (!dynar_grow(monolog->loggers, MONOLOG_START_LOGGERS)) {
      return -1;
    }
  }

  logindex = monolog_get_unused_logger();
  if (logindex < 0) {
    return -2;
  }

  monolog_set_logger(logindex, data, logger);
  return logindex;
}


int monolog_remove_logger(int index) {
  struct MonologLoggerInterface ifa = { NULL, NULL };
  struct Monologger * logger = monolog_get_logger(index);
  /* Call logger's destructor if needed. */
  if (logger->logger.free) {
    logger->logger.free(logger->data);
  }
  monolog_set_logger(index, NULL, &ifa);
  return index;
} 

struct Monolevel * monolog_find_level_by_name(char * name) {
  struct Monolevel key;
  key.name    = name;
  key.enabled = 123;
  return dynar_bsearch(monolog->levels, &key, monolog_compare_loglevels); 
}

int monolog_setup_level(char * name, int enabled) {
  struct Monolevel * level;
  int levelindex;
  level = monolog_find_level_by_name(name);
  if (!level) { 
    levelindex = monolog_get_unused_level();
    if (levelindex < 0) return -1;
    level = monolog_get_level(levelindex);
  }
  level->name    = name;
  level->enabled = enabled;
  dynar_qsort(monolog->levels, monolog_compare_loglevels);
  return 1;
}


int monolog_enable_level(char * name) {
  return monolog_setup_level(name, TRUE);
}

int monolog_disable_level(char * name) {
  return monolog_setup_level(name, FALSE);  
}

int monolog_log_va(char * file, int line, char * name, char * format, va_list args) {
  int index;
  va_list args_copy;
  struct Monolevel * level = monolog_find_level_by_name(name);
  if ((!level) || (!level->enabled)) {
    return -1;
  }
  
  for (index = 0; index < dynar_size(monolog->loggers); index++) {
    struct Monologger * logger = monolog_get_logger(index);
    if (logger && logger->logger.log) {
      va_copy(args_copy, args);
      logger->logger.log(file, line, level->name, logger->data, format, args_copy);
      va_end(args_copy);
    }
  }
  return 0;
}


int monolog_log(char * file, int line, char * level, char * format, ...) {
  int result;
  va_list args;
  va_start(args, format);
  result = monolog_log_va(file, line, level, format, args);
  va_end(args);
  return result;
}


/* Log function for logger that logs to stdout. */
int monolog_stdout_logger
  (char * file, int line, char * level, void * data, char * format, va_list args)
{
  (void) data;
  printf("%s: %s: %d: ", level, file, line);
  return vprintf(format, args);
}

/* Log function for logger that logs to stderr. */
int monolog_stderr_logger
  (char * file, int line, char * level, void * data, char * format, va_list args)
{
  (void) data;
  fprintf(stderr, "%s: %s: %d: ", level, file, line);
  return vfprintf(stderr, format, args);
}


/** Log function for logger that logs to a FILE. */
int monolog_file_logger
(char * file, int line, char * level, void * data, char * format, va_list args) {
  FILE * fout = data;
  if (fout) {
    fprintf(fout, "%s: %s: %d: ", level, file, line);
    return vfprintf(fout, format, args);
  }
  return -1;
}


/** Free function for logger that logs to a FILE. */
void monolog_file_free(void * data) {
  FILE * fout = data;
  if (fout) {
    fclose(fout);
  }
}







