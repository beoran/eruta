/** All posix OS functionality is here. */
#include "si_macro.h"
#include "si_term.h"

#ifdef SI_OS_POSIX

#include "si_mem.h"
#include <stdio.h>


// Implementation of the SiTerm
struct SiTerm_ { 
  int x, y;
  int fore, back;
};

/** Do not compile unless we're on this OS. */
// Commands
/*
static const char * escape          = "\33[";
static const char * command_clear   = "\33[2J";
 */
SiTerm * siterm_new() {
  return SI_ALLOCATE(SiTerm);
} 


SiTerm * siterm_free(SiTerm * term) {
  return si_free(term);
}

int siterm_clear(SiTerm * self) {
  printf("\33[2J");
  return siterm_goto(self, 0, 0);
}

int siterm_colors_(SiTerm * self, SiTermAttribute fore, SiTermAttribute back) {
  self->fore = fore;
  self->back = back;
  return printf( "\33[%dm\33[%dm", fore, back);
}

int siterm_goto(SiTerm * self, int x, int y) {
  self->x = x;
  self->y = y;
  return printf( "\33[%d;%dH", x + 1, y + 1 );
}

int siterm_cursor_(SiTerm * self, int show) {
  return printf( "\33[?25%c", show ? 'h' : 'l' );
}







#endif
