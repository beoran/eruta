#ifndef ERUTA_SOUND_H
#define ERUTA_SOUND_H

#include "eruta.h"

struct Sound_;
typedef struct Sound_ Sound;

struct Music_;
typedef struct Music_ Music;

struct Audio_;
typedef struct Audio_ Audio;


/* This file was generated with:
'cfunctions -c -aoff -n -w sound_proto src/sound.c' */
#ifndef CFH_SOUND_PROTO
#define CFH_SOUND_PROTO

/* From 'src/sound.c': */

Audio * audio_done (Audio * audio );

Audio * audio_alloc (void);

Audio * audio_init (Audio * audio );

Audio * audio_new (void);

Audio * audio_free (Audio * self );

void audio_stop (void);

BOOL audio_start (void);

BOOL sound_ok (void);

Sound * sound_free (Sound * self );

Sound * sound_load (char * filename );

Music * music_free (Music * self );

Music * music_load (char * filename );

Sound * sound_playback (Sound * sound , float gain , float pan , float speed , BOOL loop );

Sound * sound_play (Sound * sound );

#endif /* CFH_SOUND_PROTO */


#endif