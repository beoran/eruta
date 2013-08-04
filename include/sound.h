#ifndef ERUTA_SOUND_H
#define ERUTA_SOUND_H

#include "eruta.h"

struct Sound_;
typedef struct Sound_ Sound;

struct Music_;
typedef struct Music_ Music;

struct Audio_;
typedef struct Audio_ Audio;


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


/* New api based on "global" state. */

int audio_init_global();
int audio_done_global();
int audio_samples_max();

int audio_play_sample_ex
(int store_id, float gian, float pan, float speed, BOOL loop);

int audio_play_sample(int store_id);
int audio_stop_sample(int play_id);

int audi_sample_playing_p(int play_id);

int audio_play_id();
int audio_play_music(int store_id);
int audio_stop_music();
int audio_pause_music();
int audio_music_playing_p();





#endif

