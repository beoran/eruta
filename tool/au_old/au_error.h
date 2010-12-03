/* 
 * File:   au_error.h
 * Bjorn De Meyer, 2008
 * Error and exception handling. 
 * 
 */

#ifndef _AU_ERROR_H
#define	_AU_ERROR_H

#ifdef	__cplusplus
extern "C" {
#endif

enum au_jmpbufkind_enum { 
    AU_JMPBUF_NONE, AU_JMPBUF_FATAL, AU_JMPBUF_RAISE, AU_JMPBUF_THROW 
};

typedef enum au_jmpbufkind_enum au_jmpbufkind;
    
typedef struct au_jmpbuf_struct * au_jmpbuf;    

/* Creates a new jump buffer. */
au_jmpbuf au_jmpbuf_new();
/* Frees a jump buffer*/
au_jmpbuf au_jmpbuf_free( au_jmpbuf self );

/* Setjump and longjump wrappers */
void au_jmpbuf_jump( au_jmpbuf self, long status, au_jmpbufkind kind );
int  au_jmpbuf_set ( au_jmpbuf self );



#ifdef	__cplusplus
}
#endif

#endif	/* _AU_ERROR_H */

