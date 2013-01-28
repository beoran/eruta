#ifndef ERUTA_DYNAR_H
#define ERUTA_DYNAR_H

#include <stdlib.h> // need size_t
// can use with every, so include the header 
#include "every.h"
#include "mem.h"

typedef struct Dynar_ Dynar;

typedef struct Lilis_ Lilis;

int dynar_size (Dynar * self );
int dynar_amount (Dynar * self );
int dynar_elementsize (Dynar * self );
Dynar * dynar_done (Dynar * self );
Dynar * dynar_destroy (Dynar * self , MemDestructor * destroy );
Dynar * dynar_free (Dynar * self );
Dynar * dynar_free_destroy (Dynar * self , MemDestructor * destroy );
Dynar * dynar_alloc(void);
Dynar * dynar_initempty (Dynar * self , int elsz );
Dynar * dynar_newempty (int elsz );
Dynar * dynar_size_ (Dynar * self , int newsize );
Dynar * dynar_init (Dynar * self , int amount , int elsz );
Dynar * dynar_initptr (Dynar * self , int amount );
Dynar * dynar_new (int amount , int elsz );
Dynar * dynar_newptr (int amount );
Dynar * dynar_grow (Dynar * self , int amount );
int dynar_sizeindex_ok (Dynar * self , int index );
int dynar_index_ok (Dynar * self , int index );
void * dynar_getraw_unsafe (Dynar * self , int index );
void * dynar_getcopy_unsafe (Dynar * self , int index , void * out );
Dynar * dynar_putraw_unsafe (Dynar * self , int index , void * ptr );
void * dynar_getraw (Dynar * self , int index );
void * dynar_getcopy (Dynar * self , int index , void * ptr );
Dynar * dynar_putraw (Dynar * self , int index , void * ptr );
void * dynar_putptr (Dynar * self , int index , void * ptr );
void * dynar_getptr (Dynar * self , int index );
void * dynar_putdata (Dynar * self , int index , void * ptr );
void * dynar_getdata (Dynar * self , int index );

Every * dynar_everynow_data (Every * every );
Every * dynar_everynow_ptr (Every * every );
Every * dynar_everyinit_data (Every * every );
Every * dynar_everynext_data (Every * every );
void * dynar_everyput_data (Every * every , void * data );
void * dynar_everydone (Every * every );
Every * dynar_everyinit_ptr (Every * every );
Every * dynar_everynext_ptr (Every * every );
void * dynar_everyput_ptr (Every * every , void * data );
Every * dynar_every_data (Dynar * dynar );
Every * dynar_every_ptr (Dynar * dynar );

void * dynar_each_ptr (Dynar * self , EachDo * eachdo , void * extra );
void * dynar_walkdata (Dynar * self , EachDo * eachdo , void * extra );

Lilis * lilis_freetail (Lilis * self );
Lilis * lilis_done (Lilis * self );
Lilis * lilis_free (Lilis * self );
Lilis * lilis_alloc (void);
Lilis * lilis_init (Lilis * self , Lilis * next , Lilis * prev , void * data );
Lilis * lilis_initempty (Lilis * self );
Lilis * lilis_new (Lilis * next , Lilis * prev , void * data );
Lilis * lilis_newempty(void);
Lilis * lilis_add (Lilis * self , Lilis * other );
Lilis * lilis_addnew (Lilis * self , void * data );
Lilis * lilis_removenext (Lilis * self );
Lilis * lilis_remove (Lilis * self );
Lilis * lilis_erase (Lilis * self );
Lilis * lilis_erasenext (Lilis * self );
Lilis * lilis_next (Lilis * self );
Lilis * lilis_previous (Lilis * self );
void * lilis_data (Lilis * self );




#endif


