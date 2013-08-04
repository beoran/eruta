#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "sound.h"
#include "mem.h"
#include "store.h"



struct Sound_;
typedef struct Sound_ Sound;

struct Music_;
typedef struct Music_ Music;

struct Audio_;
typedef struct Audio_ Audio;


/** Wraper structure around an allegro sample. */
struct Sound_ {
  ALLEGRO_SAMPLE    * handle;
  ALLEGRO_SAMPLE_ID   id;
};

/** Wrapper structure around an allegro sound stream. */
struct Music_ {
  ALLEGRO_AUDIO_STREAM * handle;
};


#define AUDIO_PLAYING_SAMPLES_MAX 64

#define AUDIO_VOICES 8

/** 
* Wraps around the Allegro sound system, and put severything in one handy 
* struct.
*/
struct Audio_ {
  BOOL                       ok;
  
  ALLEGRO_MIXER         * mixer;
  ALLEGRO_VOICE         * voice;
  
  ALLEGRO_AUDIO_STREAM  * stream;
  int                     stream_id;
  
  ALLEGRO_SAMPLE_ID       playing[AUDIO_PLAYING_SAMPLES_MAX];
  int                     playing_id[AUDIO_PLAYING_SAMPLES_MAX];
  int                     playing_last;
};


/* The "global" audio state. */
static Audio * audio_state;

#ifndef SOUND_SAMPLES
#define SOUND_SAMPLES 16
#endif

#define AUDIO_RATE     44100
#define AUDIO_DEPTH    ALLEGRO_AUDIO_DEPTH_INT16
#define AUDIO_CHANNELS ALLEGRO_CHANNEL_CONF_2

/* Call this when the use of audio is no longer needed. */
BOOL audio_done() {
  if (!audio_state) {
    return FALSE;
  }
  
  al_stop_samples();

  if (audio_state->stream) {
    al_detach_audio_stream(audio_state->stream);
  }

  if (audio_state->mixer) {
      al_detach_mixer(audio_state->mixer);
      al_destroy_mixer(audio_state->mixer);
  }

  if (audio_state->voice) {
    al_destroy_voice(audio_state->voice);
    audio_state->voice = NULL;
  }
  
  mem_free(audio_state);
  al_uninstall_audio();
  return TRUE;
}

/** Call this to alloate space for the audio system. */
Audio * audio_alloc(void) {
  return STRUCT_ALLOC(Audio);
}


int audio_playing_samples_max() {
  return AUDIO_PLAYING_SAMPLES_MAX;
}


/** Call this to start up the audio system. */
BOOL audio_init() {
  int index;
  
  if(!al_install_audio())     return FALSE;
  if(!al_init_acodec_addon()) return FALSE;
  
  audio_state = audio_alloc();
  if (!audio_state)           return FALSE;
  
  audio_state->mixer =
    al_create_mixer(AUDIO_RATE, AUDIO_DEPTH, AUDIO_CHANNELS);
  audio_state->voice =
    al_create_voice(AUDIO_RATE, AUDIO_DEPTH, AUDIO_CHANNELS);
  al_attach_mixer_to_voice(audio_state->mixer, audio_state->voice);
  
  al_set_default_mixer(audio_state->mixer);
  al_reserve_samples(SOUND_SAMPLES);
  
  audio_state->stream    = NULL;
  audio_state->stream_id = -1;
  
  for (index = 0; index < AUDIO_PLAYING_SAMPLES_MAX; index++) {
    audio_state->playing_id[index] = -1;
  }

  audio_state->ok = TRUE;
  return TRUE;
}

/** Returns true if sound can be played, false if not. */
BOOL audio_ok(void) {
  return audio_state && audio_state->ok;
}


#ifndef MUSIC_BUFFERS
#define MUSIC_BUFFERS 2
#endif

#ifndef MUSIC_BUFSIZE
#define MUSIC_BUFSIZE 2048
#endif


/* Set the music with the given index as the active usic. 
If the index is negative, sets no music as active. */
BOOL audio_set_music(int store_id)  {
  ALLEGRO_AUDIO_STREAM * stream;
  if (!audio_ok()) return FALSE;
  
  if (store_id < 0) {
    if(audio_state->stream) {
      al_detach_audio_stream(audio_state->stream);
    }
    audio_state->stream   = NULL;
    audio_state->stream_id = -1;
    return TRUE;
  }
  stream = store_get_audio_stream(store_id);

  if (!stream) {
    return FALSE;
  }
  audio_state->stream = stream;
  
  
  al_attach_audio_stream_to_mixer(audio_state->stream, audio_state->mixer);
  return TRUE;
}

BOOL audio_play_music() {
  if (!audio_ok()) return FALSE;
  if(!audio_state->stream) return FALSE;
  return  al_set_audio_stream_playing(audio_state->stream, TRUE);
}


BOOL audio_stop_music() {
  if (!audio_ok()) return FALSE;
  if(!audio_state->stream) return FALSE;
  return  al_set_audio_stream_playing(audio_state->stream, FALSE);
}


BOOL audio_music_playing_p() {
  if (!audio_ok()) return FALSE;
  if(!audio_state->stream) return FALSE;
  return al_get_audio_stream_playing(audio_state->stream);
}

int audio_playing_music_id() {
  if (!audio_ok()) return -1;
  if(!audio_state->stream) return -1;
  return audio_state->stream_id;
}


int audio_play_sound_ex
(int store_id, float gain, float pan, float speed, BOOL loop) {
  int play_id;
  ALLEGRO_SAMPLE * sample;
  ALLEGRO_PLAYMODE mode =
    (loop ? ALLEGRO_PLAYMODE_LOOP : ALLEGRO_PLAYMODE_ONCE);
  if (!audio_ok()) return -2;
  sample = store_get_sample(store_id);
  if (!sample) return -1;
  play_id = audio_state->playing_last;
  audio_state->playing_last++;
  /* Work in ring buffer style. */
  if (audio_state->playing_last >= AUDIO_PLAYING_SAMPLES_MAX) {
    audio_state->playing_last = 0;
  }
  al_play_sample(sample, gain, pan, speed, mode,
                 audio_state->playing + play_id);
  audio_state->playing_id[play_id] = store_id;
  return play_id;
}

void audio_stop_all_sounds() {
  al_stop_samples();
  /* reset ring buffer pointer */
  audio_state->playing_last = 0;
}

/** Plays the sound once only with default parameters. */
int audio_play_sound(int sample_id) {
  return audio_play_sound_ex(sample_id, 1.0, 0.0, 1.0, FALSE);
}

/* 
Stops the sound with the given play id from playing. Doesn't work if 
the integer handle is stale, but thesoun sould have stopped by itself then.
*/
BOOL audio_stop_sound(int play_id) {
  if (!audio_ok()) return FALSE;
  if (play_id  < 0) return FALSE;
  if (play_id >= AUDIO_PLAYING_SAMPLES_MAX) return FALSE;
  al_stop_sample(audio_state->playing + play_id);
  return TRUE;
}



