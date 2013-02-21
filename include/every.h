#ifndef EVERY_H_INCLUDED
#define EVERY_H_INCLUDED

/*
* Every is an iterator that allows to iterate over every element of
* a collection.
*/

struct Every_;
typedef struct Every_ Every;

struct EveryActs_;
typedef struct EveryActs_ EveryActs;


// types for the funtion pointers that every uses
typedef Every * (EveryNext(Every * every));
typedef void  * (EveryPut(Every * every, void * element));
typedef void  * (EveryDone(Every * every));
typedef Every * (EveryInit(Every * every));

// Struct with all function pointers for Every
struct EveryActs_ {
  EveryDone * done;
  EveryInit * init;
  EveryNext * next;
  EveryPut  * put;
};

/*
* Every is a struct that can be used with collections like Dynar to fetch
* every element of the collection.
*/
struct Every_ {
  void         * now;
  void         * on;
  int            index;
  void         * extra;
  EveryActs    * acts;
};


struct Each_;
typedef struct Each_ Each;
/*
* Each is an alternative iterator interface, that
* requires a function callback in stead of an object. 
*/
struct Each_ {
  void * on;
  void * now;
  void * extra;
  int    index;
};

/* Function pointer for the each iteration method.  */
typedef void * (EachDo(Each * element));

/* An even simpler iterator interface, one for simply iterating and one 
for searching. Iteration is stopped if AllData returns not-NULL, 
and the returned data will be returned by the function that uses AllData 
too. Extra is used for passing in extra data if any. */
typedef void * (AllData)(void * data, void * extra);


/* Generic comparator function. Much like strcmp. */
typedef int (AllCompare)(void * one, void * two);

Every * every_alloc (void);
Every * every_done (Every * self );
Every * every_free (Every * self );
Every * every_init (Every * self , EveryActs * acts );
Every * every_new (EveryActs * acts );
void * every_get (Every * self );
void * every_put (Every * self , void * data );

Every * every_next (Every * self , void * data );
Each * each_init (Each * self , void * on , void * data );
Each * each_next (Each * self , void * now );

void * each_now (Each * self );
void * each_on (Each * self );
void * each_extra (Each * self );
int each_index (Each * self);

/* Yet another iteration interface: a Walker is a simple callback function. */
typedef void * (Walker)(void * element, void * extra);  

// and some helper macros
#define EACH_NOW(EACH, TYPE) ((TYPE *) each_now(EACH))


#endif
