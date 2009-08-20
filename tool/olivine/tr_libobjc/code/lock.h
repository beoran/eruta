#include <pthread.h>

#define DECLARE_LOCK(x) static pthread_mutex_t x##_lock;
#define DECLARE_STATIC_LOCK(x) static pthread_mutex_t x##_lock = PTHREAD_MUTEX_INITIALIZER;
#define INIT_LOCK(x)  pthread_mutex_init(&x##_lock, NULL);
#define LOCK(x) pthread_mutex_lock(&x##_lock);
#define UNLOCK(x) pthread_mutex_unlock(&x##_lock);
