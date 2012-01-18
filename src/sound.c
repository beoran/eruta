#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "sound.h"
#include "mem.h"

struct Sound_ {
  ALLEGRO_SAMPLE    * handle;
  ALLEGRO_SAMPLE_ID   id;
};

struct Music_ {
  ALLEGRO_AUDIO_STREAM * handle;
};

#define AUDIO_VOICES 8
struct Audio_ {
  BOOL               ok;
  ALLEGRO_MIXER * mixer;
  ALLEGRO_VOICE * voice;
};

typedef struct Audio_ Audio;

#ifndef SOUND_SAMPLES
#define SOUND_SAMPLES 16
#endif

#define AUDIO_RATE     44100
#define AUDIO_DEPTH    ALLEGRO_AUDIO_DEPTH_INT16
#define AUDIO_CHANNELS ALLEGRO_CHANNEL_CONF_2

Audio * audio_done(Audio * audio) {
  if(audio) {
    al_detach_mixer(audio->mixer);
    if (audio->mixer) al_destroy_mixer(audio->mixer);
    audio->mixer = NULL;
    if (audio->voice) al_destroy_voice(audio->voice);
    audio->voice = NULL;
  }
  return audio;
}

Audio * audio_init(Audio * audio) {
  if(!audio) return NULL;
  audio->mixer = al_create_mixer(AUDIO_RATE, AUDIO_DEPTH, AUDIO_CHANNELS);
  audio->voice = al_create_voice(AUDIO_RATE, AUDIO_DEPTH, AUDIO_CHANNELS);
  al_attach_mixer_to_voice(audio->mixer, audio->voice);
  return audio;
}


/** Global audio settings. */
static Audio audio_;

/** Stops the audio. */
void audio_stop() {
  audio_done(&audio_);
  audio_.ok = FALSE;
}


/** Call this to enable sound. samples is the amount of samples to play.
* Returns TRUE if ok false if it failed.
*/
BOOL audio_start() {
  audio_.ok = FALSE;
  if(!al_install_audio()) return FALSE;
  if(!al_init_acodec_addon()) return FALSE;
  // if(!audio_init(&audio_)) return FALSE;  
  al_reserve_samples(SOUND_SAMPLES);
  audio_.mixer = al_get_default_mixer();
  audio_.ok = TRUE;
  return audio_.ok;
}

/** Returns true if sound can be played, false if not. */
BOOL sound_ok() {
  return audio_.ok;
}

/** Frees memory associated with sound. */
Sound * sound_free(Sound * self) {
  if(self) {
    if(self->handle) {
      al_destroy_sample(self->handle);
    }
    mem_free(self);
  }
  return NULL;
}

/** Loads a sound from file. */
Sound * sound_load(char * filename) {
  Sound * sound = STRUCT_ALLOC(Sound);
  if(!sound) return NULL;
  sound->handle = al_load_sample(filename);
  if(!sound->handle) return sound_free(sound);
  return sound;
}

Music * music_free(Music * self) {
  if(self) {
    if(self->handle) {
      al_detach_audio_stream(self->handle);
      al_destroy_audio_stream(self->handle);
    }
    mem_free(self);
  }
  return NULL;
}

#ifndef MUSIC_BUFFERS
#define MUSIC_BUFFERS 2
#endif

#ifndef MUSIC_BUFSIZE
#define MUSIC_BUFSIZE 2048
#endif


/** Loads a sound from file. */
Music * music_load(char * filename)  {
  Music * music = STRUCT_ALLOC(Music);
  if(!music) return NULL;
  // printf("Audio status: %d, %p, %p\n", audio_.ok, audio_.voice, audio_.mixer);

  music->handle =
      al_load_audio_stream(filename, 2, MUSIC_BUFSIZE);
      
  if(!music->handle) {
    perror("could not load music file");
    return music_free(music);
  }
  al_attach_audio_stream_to_mixer(music->handle, audio_.mixer);  
  return music;
}


/** Loads music from file, and starts to play it immediately. */


/** Plays the sound with detailed parameters. */
Sound * sound_playback(Sound * sound,
  float gain, float pan, float speed, BOOL loop) {
  ALLEGRO_PLAYMODE mode =
    (loop ? ALLEGRO_PLAYMODE_LOOP : ALLEGRO_PLAYMODE_ONCE);    
  if(!sound) return NULL;
  if(!sound->handle) return NULL;
  if(!sound_ok()) return NULL;
  al_play_sample(sound->handle, gain, pan, speed, mode, &sound->id);
  return sound;
}

/** Plays the sound once only with default parameters. */
Sound * sound_play(Sound * sound) {
  return sound_playback(sound, 1.0, 0.0, 1.0, FALSE);
}


