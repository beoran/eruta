#ifndef _GARI_INTERN_H_
#define _GARI_INTERN_H_


#ifndef GARI_INTERN_ONLY
#error This include file is only for used inside the Gari Library source files.
#endif

/* Initializes a gari game. Externa lusers will use gari_game_make. */
GariGame * gari_game_init(GariGame * game);




#endif