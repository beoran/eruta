#include "gari.h"

#include "SDL_mixer.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"


/** Initializes the audio subsystem for a game. */
void gari_audio_init(GariGame * game) {
  /*
  // start SDL with audio support
  if(SDL_Init(SDL_INIT_AUDIO)==-1) {
      printf("Could not init SDL audio: %s\n", SDL_GetError());
      return;
  }
  */
  // open 22.05KHz, signed 16bit, system byte order,
  //      stereo audio, using 1024 byte chunks
  if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024)==-1) {
    printf("Mix_OpenAudio: %s\n", Mix_GetError());
    return;
  }  
}

/** Cleans up the audio subsystem for a game. */
void gari_audio_done(GariGame * game) {
  Mix_CloseAudio();
}

struct GariSound_ { 
  Mix_Chunk * chunk;
};


/** Initialises and loads sound into an existing GariSound record. */
GariSound * gari_sound_init(GariSound * sound, char * filename) {
  if(!sound)        { return NULL; } 
  // Unload chunk first
  if (sound->chunk) {  gari_sound_done(sound); }
  sound->chunk    = Mix_LoadWAV(filename);
  if(!sound->chunk) { return NULL; }
  return sound;
}

/** Creates a new GariSound and loads the sound from a file. */
GariSound * gari_sound_load(char * filename) {
  GariSound * sound = GARI_ALLOCATE(GariSound);
  if (!sound) { return NULL; }
  if (!gari_sound_init(sound, filename)) {
    gari_sound_free(sound);
    return NULL;
  }
  return sound;
}

/** Deallocates the loaded sound, but not the wrapper GariSound itself. */
GariSound * gari_sound_done(GariSound * sound) {
  if (!sound) return NULL;
  if (sound->chunk) Mix_FreeChunk(sound->chunk);
  sound->chunk = NULL; // set to null to signal it's unloaded. 
  return sound;
}

/** Calls gari_sound_done and then frees the sound itself.*/
GariSound * gari_sound_free(GariSound * sound) {
  if(!sound) return NULL;
  gari_sound_done(sound); 
  GARI_FREE(sound);
  return NULL;
}


/** Plays a sound once. */
GariSound * gari_sound_play(GariSound * sound) {
  if ((!sound) || (!sound->chunk)) return NULL;
  Mix_PlayChannel(-1, sound->chunk, 0);
}


struct GariMusic_ {
  Mix_Music * tune;
  uint32_t start;
  uint32_t current;  
};


/** Initialises and loads music into an existing GariMusic record. */
GariMusic * gari_music_init(GariMusic * music, char * filename) {  
  if (!music)       { return NULL; }
  if (music->tune)  { gari_music_done(music); } 
  music->start    = 0;
  music->current  = 0;
  music->tune     = Mix_LoadMUS(filename);
  if (!music->tune) { return NULL; } 
  return music;
}

/** Creates a new GariMusic and loads the music from a file. */
GariMusic * gari_music_load(char * filename) {
  GariMusic * music = GARI_ALLOCATE(GariMusic);
  if(!music)        { return NULL; }
  if(!gari_music_init(music, filename)) {
    gari_music_free(music); 
    return NULL; 
  }
  return music;  
}

/** Deallocates up the loaded music, but not the wrapper GariMusic itself. */
GariMusic * gari_music_done(GariMusic * music) {
  if((!music) || (!music->tune)) {
    return NULL;
  }
  Mix_FreeMusic(music->tune); 
  music->tune = NULL;
  return music;
}

/** Calls gari_music_done and then frees the music itself.*/
GariMusic * gari_music_free(GariMusic * music) {
  if(!music) return NULL;
  gari_music_done(music);
  GARI_FREE(music);
  return NULL;
}

/** Starts playing the music, fading in after fade ms. */
GariMusic * gari_music_fadein(GariMusic * music, int loops, int fade) {
  if((!music) || (!music->tune)) return NULL;
  // SDL ticks are always milliseconds...
  music->start = SDL_GetTicks();
  Mix_FadeInMusic(music->tune, loops, fade);
  return music;
}

/** Stops playing the music, fading out after fade ms. */
GariMusic * gari_music_fadeout(GariMusic * music, int fade) {
  if((!music) || (!music->tune)) return NULL;
  music->current = SDL_GetTicks() + fade;
  Mix_FadeOutMusic(fade);
} 


