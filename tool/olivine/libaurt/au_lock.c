/*
* Portable locks, based on Lamport's bakery algorythm.
* Unfortunately, I czan't see mto get it right. perhaps it's because  
* writes are not guaranteed to succeed in the face of threads in C.
*/


#include "au.h"

struct au_lock_;
struct au_cust_;

enum AU_LOCK_COLOR_ { AU_LOCK_NONE = 0, AU_LOCK_WHITE = 1, AU_LOCK_BLACK = 2 };

typedef volatile struct au_lock_ au_lock;
typedef volatile struct au_cust_ au_cust;
typedef enum AU_LOCK_COLOR_ AU_LOCK_COLOR;


typedef volatile unsigned short au_lock_t;
typedef void (*au_lock_func)(void);

#define AU_LOCK_MAX sizeof(au_lock_t) 
#define AU_LOCK_UNLOCKED 0

struct au_lock_ {
  volatile au_lock_t  id;
  AU_LOCK_COLOR       color;
  volatile long       others[AU_LOCK_MAX];
};
 
struct au_cust_ {
  volatile au_lock_t      id;
  volatile AU_LOCK_COLOR  color;
  volatile int            entering;
  volatile long           ticket;  
}; 

au_cust  au_customers[AU_LOCK_MAX]; 

volatile int  lock_entering[AU_LOCK_MAX];

volatile AU_LOCK_COLOR     lock_color = AU_LOCK_WHITE;
volatile int  lock_colors  [AU_LOCK_MAX];
volatile long lock_number  [AU_LOCK_MAX];


void au_lock_init() {
  volatile int index;
  lock_color  = AU_LOCK_WHITE;
  for(index   = 0 ; index < AU_LOCK_MAX; index++) {
    au_customers[index].id      = index;
    au_customers[index].ticket  = AU_LOCK_UNLOCKED; 
    au_customers[index].color   = AU_LOCK_NONE;
    au_customers[index].entering= FALSE;
  }
} 

volatile long au_lock_max(au_cust * cust) {
  volatile int index;
  volatile long tickets[AU_LOCK_MAX];
  volatile long result;
  result = AU_LOCK_UNLOCKED;
  volatile long lock_local[AU_LOCK_MAX]; 
  for(index = 0 ; index < AU_LOCK_MAX; index++) {
    tickets[index] = au_customers[index].ticket; 
  }
  /* We MUST make a local copy before calculating the max. */
  /* Must make local copies. */
  for(index = 0 ; index < AU_LOCK_MAX; index++) {
    if(tickets[index] > result) { 
      result = tickets[index];
    }    
  }
  return result;
}


#include <stdio.h>

/* Returns true if cust1 has priority over cust2. */
volatile int au_cust_priority(au_cust * cust1, au_cust * cust2) { 
  volatile AU_LOCK_COLOR cust1_color, cust2_color;
  volatile long cust1_ticket, cust2_ticket, cust1_id, cust2_id;
  cust1_id        = cust1->id;
  cust2_id        = cust1->id;
  cust1_ticket    = cust1->ticket;
  cust2_ticket    = cust2->ticket;
  cust1_color     = cust1->color;
  cust2_color     = cust2->color;
  if (cust1_color != cust2_color) {
    return (cust1_color != lock_color);
    /* 
      If the two customers have a different colored ticket, the one with the
      different general lock color has the lowest priority. 
    */
  } 
  if(cust1_ticket != cust2_ticket) {
    return cust1_ticket < cust2_ticket;
  }
  return cust1_id < cust2_id;
}


volatile int au_lock_mustwait(au_cust * customer, volatile int index) {
  volatile au_cust * other_customer;
  volatile AU_LOCK_COLOR mycolor, other_color;
  volatile long my_ticket, other_ticket, my_id;
  volatile int index_locked, higher_priority, same_priority, index_priority;  
  other_customer  = au_customers + index;
  other_ticket    = other_customer->ticket;
  if (other_customer->ticket) { return FALSE; }  
  /* We don't have to wait of index if index isn't locked. */
  return au_cust_priority(other_customer, customer);
  /* We have to wait if our priority is lower than that of the other customer*/
}

    
void au_lock_lock(au_lock_t lock, au_lock_func callback) {
  volatile int index;
  volatile int max;
  volatile au_cust    * customer;  
  customer            = au_customers + lock;
  customer->entering  = TRUE;
  customer->id        = lock;
  customer->color     = lock_color; /* Current lock color. */
  /* printf("Lock %d : entering \n", lock); */
  max                 = au_lock_max(customer);
  max ++;
  customer->ticket       = max;
  if (customer->color < AU_LOCK_WHITE) {
    printf("Lock %d : wrong ticket color: %d from %d\n", lock, customer->color);
  }  
  while(customer->ticket < 1) {  
    printf("Lock %d : wrong ticket number: %d from %d\n", lock, customer->ticket, max);
    max                  = au_lock_max(customer);
    max ++;
    customer->ticket    = max;
  }
  customer->entering     = FALSE; 
  for(index = 0; index < AU_LOCK_MAX ; index ++) { 
    if (index == customer->id) continue;
    /* Waith for thread index to recieve it's number, if it's entering. */
    while(au_customers[index].entering) {
      printf("Lock %d : waiting to enter: %d\n", customer->id, index);
      if(callback) callback(); 
      /* Call the callback function.*/
    }
    while(au_lock_mustwait(customer, index))  
     {
      /* printf("Lock %d: waiting for my turn: %d has %d\n", lock, index, lock_ticket[index]); */
      /* Wait for all other threads that have a lower number (higher priority) */
      if(callback) callback(); 
      /* Call the callback function.*/
     }
   }
   /* If we get here,  we're should be ok  to go into the locked section. */
}


void au_unlock(au_lock_t lock) {
  if (au_customers[lock].color == AU_LOCK_WHITE) {
    lock_color             = AU_LOCK_BLACK;
  } else {
    lock_color             = AU_LOCK_WHITE;
  }
  /* Flip global lock color */
  au_customers[lock].color  = AU_LOCK_NONE;    
  au_customers[lock].ticket = AU_LOCK_UNLOCKED;
}

void au_thread_yield(void) {
  pthread_yield();
}


#include <stdlib.h>
#include <pthread.h>

volatile int shared_int;

static void *thread_func(void *args)
{
    int         i;
    volatile int         threadno;
    au_lock_t   lock;
    threadno = (int) args; 
    lock     = threadno;
 
    
    /* au_lock((threadno), NULL); */
    for (i = 0; i < 1000000; i++)
    {
        au_lock_lock(lock, NULL); 
        shared_int     = threadno;
        /* au_thread_yield(); /* Let other threads do their thing. */
        if(shared_int != (threadno)) { 
          printf("Overwritten in thread %d: step %d: %d, thread lock: %d\n", (threadno), i, shared_int, 
            lock_number[threadno]  
          );
          /* This will print only if the threads overwrote each other's shared_int. */
        } else {

        }
        au_unlock(lock); 
    } 
    /* au_unlock((threadno)); */
    
    return NULL;
}
 
#define THREAD_COUNT 64 
 
int main(void)
{
    int i;
    pthread_t thread[THREAD_COUNT];
    pthread_t thread2;
    int tn1    = 1;
    int tn2    = 2;
    shared_int = 27;
    au_lock_init();
    
    for (i=0; i < THREAD_COUNT; i++) {    
      if (pthread_create(thread + i, NULL, thread_func, (void *)i) != 0)  {
          return EXIT_FAILURE;
      }
    }
    puts("Threads are running...");
    for(i=0; i < THREAD_COUNT; i++) { 
      if (pthread_join(thread[i], NULL) != 0)
      {
          return EXIT_FAILURE;
      }
    }  
    puts("Threads are done!");
    return EXIT_SUCCESS;
}

