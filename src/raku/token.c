#include "si_macro.h"
#include "si_wire.h"
#include "si_mem.h"
#include "raku_token.h"

// Single character tokens are encoded as their character code. 
enum RakuTokenKind_ {
    RakuTokenFail       = 256,      
    RakuTokenEOF        = 257,
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

struct RakuToken_ {
  SiWire        *  value; 
  RakuTokenKind    kind;
  int              line;
  int              col;
};

RakuToken * rakutoken_alloc(RakuToken * self) {
  return SI_ALLOCATE(RakuToken);
}

RakuToken * rakutoken_init(RakuToken * self, 
                           RakuTokenKind kind, SiWire * value, int line, int col) 
{
  if(!self) return NULL;
  self->kind  = kind;
  self->value = siwire_dup(value);    
  self->line  = line;
  self->col   = col;
  return self;
}

RakuToken * rakutoken_done(RakuToken * self) {    
  if(!self) return NULL;
  siwire_free(self->value);
  self->value = NULL;
  return self;
}

RakuToken * rakutoken_free(RakuToken * self) {
  return si_free(rakutoken_done(self));
}

int rakutoken_eof_p(RakuToken * self) {
  return self->kind == RakuTokenEOF;
}


int rakutoken_fail_p(RakuToken * self) {
  return self->kind == RakuTokenFail;
}

RakuTokenKind rakutoken_kind(RakuToken * self) {
  return self->kind;
}

int rakutoken_line(RakuToken * self) {
  return self->line;
}

int rakutoken_col(RakuToken * self) {
  return self->col;
}

SiWire * rakutoken_value(RakuToken * self) {
  return self->value;
}

/*
int rakutoken_value_int(RakuToken * self) {
  return siwire_int(self->value);
}

double rakutoken_value_double(RakuToken * self) {
  return siwire_double(self->value);
}
*/

                         
                             



