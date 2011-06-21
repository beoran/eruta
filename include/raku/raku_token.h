#ifndef RAKU_TOKEN_H
#define RAKU_TOKEN_H 

#include "si_wire.h"

// Single character tokens are encoded as their character code. 
enum RakuTokenKind_ {
    RakuTokenFail       = 256,      
    RakuTokenEof        = 257,
    RakuTokenSymbol     = 258,
    RakuTokenDo         = 259,
    RakuTokenEnd        = 260,
    RakuTokenString     = 261,
    RakuTokenInteger    = 262,
    RakuTokenFloat      = 263,
};

typedef enum RakuTokenKind_ RakuTokenKind; 

struct RakuToken_;
typedef struct RakuToken_ RakuToken;


RakuToken * rakutoken_alloc();

RakuToken * rakutoken_init(RakuToken * self, 
                           RakuTokenKind kind, 
                           SiWire * value, int line, int col); 

RakuToken * rakutoken_new(RakuTokenKind kind, SiWire * value, int line, int col);

RakuToken * rakutoken_newfail(SiWire * value, int line, int col);
RakuToken * rakutoken_neweof(SiWire * value, int line, int col);
RakuToken * rakutoken_newsymbol(SiWire * value, int line, int col);
RakuToken * rakutoken_newstring(SiWire * value, int line, int col);
RakuToken * rakutoken_newinteger(SiWire * value, int line, int col);
RakuToken * rakutoken_newfloat(SiWire * value, int line, int col);

RakuToken * rakutoken_done(RakuToken * self);

RakuToken * rakutoken_free(RakuToken * self); 

int rakutoken_eof_p(RakuToken * self);

int rakutoken_fail_p(RakuToken * self);

RakuTokenKind rakutoken_kind(RakuToken * self); 

int rakutoken_line(RakuToken * self);

int rakutoken_col(RakuToken * self);

SiWire * rakutoken_value(RakuToken * self);

#endif
