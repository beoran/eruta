#ifndef ERUTA_SOUND_H
#define ERUTA_SOUND_H

#include "eruta.h"


struct Sound_;
typedef struct Sound_ Sound;

struct Music_;
typedef struct Music_ Music;


/** Call this to enable sound. samples is the amount of samples to play.
* Returns TRUE if ok false if it failed. 
*/
BOOL audio_start();

/** Returns true if sound can be played, false if not. */
BOOL sound_ok();

/** Loads a sound from file. */
Sound * sound_load(char * filename);

/** Loads music from file, and starts to play it immediately. */
Music * music_load(char * filename);

/** Plays the sound. */
Sound * sound_play(Sound *);



#endif