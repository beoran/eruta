/* 
 * File:   au_memory.h
 * Author: bjmey
 *
 * Created on August 5, 2008, 9:37 AM
 */

#ifndef _AU_MEMORY_H
#define	_AU_MEMORY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stddef.h>    
    
void *  au_malloc   (size_t size);
void *  au_realloc  (void * old, size_t size);
void *  au_free     (void * ptr);

/* Must be called on the tom of main to initialize 
 * the garbage collector on some platforms. 
 */
void    au_mem_init (void);

/* Macro to allocate a C abstract data type that has been hidden in a 
 * pointer to a struct. 
 */
#define au_struct_new(name) ((name) au_malloc(sizeof(struct name##_struct)))

/* Forward declatarion of ADT stack. */
struct  au_rtstack_struct;
typedef struct au_rtstack_struct * au_rtstack;




#ifdef	__cplusplus
}
#endif

#endif	/* _AU_MEMORY_H */

