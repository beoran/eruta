#include "si_macro.h"

/** All win32 OS functionality is here. */

#ifdef SI_OS_WIN32
/** Do not compile unless we're on this OS. */
#include "si_term.h"

#include <windows.h>


/* Windows attributes */
struct SiTerm_ { 
  short int wattr[SiTermAttributeLast];
}

void siterm_initcolors(SiTerm * self)
 {
    self->wattr[SiTermBlack]   = 0;
    self->wattr[SiTermBlue]    = FOREGROUND_BLUE;
    self->wattr[SiTermRed]     = FOREGROUND_RED;
    self->wattr[SiTermMagenta] = FOREGROUND_BLUE | FOREGROUND_RED;
    self->wattr[SiTermGreen]   = FOREGROUND_GREEN;
    self->wattr[SiTermCyan]    = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    self->wattr[SiTermYellow]  = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
    self->wattr[SiTermWhite]   = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
    self->wattr[SiTermBlack]   = 0;
    self->wattr[SiTermBlue]    = BACKGROUND_BLUE;
    self->wattr[SiTermRed]     = BACKGROUND_RED;
    self->wattr[SiTermMagenta] = BACKGROUND_BLUE | BACKGROUND_RED;
    self->wattr[SiTermGreen]   = BACKGROUND_GREEN;
    self->wattr[SiTermCyan]    = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
    self->wattr[SiTermYellow]  = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
    self->wattr[SiTermWhite]   = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
}

// rest to be implemented later :)

#endif

