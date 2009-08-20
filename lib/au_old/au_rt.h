/* 
 * File:   au_rt.h
 * Author: bjmey
 *
 * Created on May 20, 2008, 2:21 PM
 */

#ifndef _AU_RT_H
#define	_AU_RT_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdlib.h>  

    
/* How many methods a class can have at most. Powers of 2 should be nice. */
#ifndef AU_METHODS_PER_CLASS
#define AU_METHODS_PER_CLASS 2048   
#endif

 /* 
  * Size of the hash table that is used to dispatch messages. 
  * Should be the next prime aftert 2 * AU_METHODS_PER_CLASS. 
  */    
#ifndef AU_METHODS_HASHSIZE 
#define AU_METHODS_HASHSIZE 4099
#endif
    
/** How many 8 bit bytes a message may be composed of at most. */    
#ifndef  AU_MESSAGE_SIZE
#define  AU_MESSAGE_SIZE 256
#endif    

/* Forward declaration of the au_runtime. Consider it an opaque structure. */
struct au_rt_;
typedef struct au_rt_ au_rt;
    
/* Forward declaration of the au_id. Consider it an opaque structure. */    
struct au_id_;
typedef struct au_id_ au_id;

typedef au_id * (*au_method) (struct au_id_ *, ...);

/* Forward declaration of the au_message. Consider it an opaque structure. */    
struct au_message_;
typedef struct au_message_ au_message;



/** Functions to handle the runtime. Multiple runtimes are allowed. */
au_rt *     au_rt_new();
int         au_rt_free(au_rt *self);


/** Functions for memory management. */
#ifdef _NO_WAY_
void *      au_alloc(au_rt *rt, size_t size);
int         au_free(au_rt * rt, void * pointer);
#endif



/** Functions to create messages. */
au_message *    au_message_new(const char * cstr);
int             au_message_free(au_message * self);
const char *    au_message_cstr(au_message * self);
int             au_message_equal(au_message * self, au_message * other);

/* Create a basic AU object.*/
au_id *         au_id_new(au_rt  * rt);
int             au_id_free(au_id * self);
/* Add a method to the object, overriding the old one if it existed. */
au_id *         au_id_set(au_id * self, au_message * message, au_method method);
/* Gets the method that will be called when given this message, or NULL if no 
 * such method. Allows method caching. 
 */
au_method *     au_id_get(au_id * self, au_message * message);
/* Sets the method_missing special method for this object. */
au_id *         au_id_missing_set(au_id * self, au_method * method);
/* Sets the method_missing special method for this object. */
au_method *     au_id_missing_get(au_id * self);
/* The core dispatching function. */
au_id *         au_id_send(au_id * self, au_message * message, ... );





    

    



#ifdef	__cplusplus
}
#endif

#endif	/* _AU_RT_H */

