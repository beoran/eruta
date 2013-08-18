#ifndef ERUTA_SOUND_H
#define ERUTA_SOUND_H

#include "eruta.h"

/*


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
*/


/* New api based on "global" state. */

BOOL audio_init();
BOOL audio_done();
int audio_playing_samples_max();

int audio_play_sound_ex
(int store_id, float gain, float pan, float speed, BOOL loop);

int audio_play_sound(int store_id);
BOOL audio_stop_sound(int play_id);

BOOL audio_set_music(int store_id);
BOOL audio_play_music();
BOOL audio_stop_music();
BOOL audio_music_playing_p();





#endif

