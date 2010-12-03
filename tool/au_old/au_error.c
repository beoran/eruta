
#include <setjmp.h>
#include <assert.h>
#include "au_error.h"
#include "au_memory.h"

struct au_jmpbuf_struct {
    jmp_buf         buffer;
    long            status;
    au_jmpbufkind   kind;    
};   

/* Longjump wrapper */
void au_jmpbuf_jump(au_jmpbuf self, long status, au_jmpbufkind kind) {
    assert(self);
    self->status = status;
    self->kind   = kind;    
    longjmp(self->buffer, 1);
}

/* Setjmp wrapper */
int au_jmpbuf_set(au_jmpbuf self) {
    assert(self);
    self->status = 0;
    self->kind   = AU_JMPBUF_NONE;    
    if(setjmp(self->buffer)) { 
        /* Return from direct call*/
        return 0;
    } else {
        return 1;
        /* Return from longjmp */
    }
}

/* allocates a jump buffer */
au_jmpbuf au_jmpbuf_new() {
    au_jmpbuf self = au_struct_new(au_jmpbuf);
    return self;
}

/* Frees a jump buffer*/
au_jmpbuf au_jmpbuf_free( au_jmpbuf self ) {
    return au_free(self);
}

