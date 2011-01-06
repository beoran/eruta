#include "gari.h"

#include "SDL_mixer.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"

#ifndef GARI_AUDIO_BUFFER
#define GARI_AUDIO_BUFFER 4096
#endif


#define GARI_AUDIO_MONO   1
#define GARI_AUDIO_STEREO 2

struct GariAudioInfo_ {
  int frequency;
  int channels; 
  int format;
};  


GariAudioInfo * gari_audioinfo_new(int freq, int form, int chan) {
  GariAudioInfo * info = GARI_ALLOCATE(GariAudioInfo);
  if(!info) return NULL;
  info->frequency = freq;
  info->channels  = chan;
  info->format    = form;
  return info;  
} 

GariAudioInfo * gari_audioinfo_free(GariAudioInfo * info) {
  GARI_FREE(info);
}

int gari_audioinfo_frequency(GariAudioInfo * info) {
  if(!info) return -1;
  return info->frequency;
}

int gari_audioinfo_channels(GariAudioInfo * info) {
  if(!info) return -1;
  return info->channels;
}

int gari_audioinfo_format(GariAudioInfo * info) {
  if(!info) return -1;
  return info->format;
}


GariAudioInfo * gari_audio_query() {
  int frequency; Uint16 format; int channels; int res;  
  res = Mix_QuerySpec(&frequency, &format, &channels);
  if(res < 0) { return NULL ; } 
  return gari_audioinfo_new(frequency, format, channels);
}  



/** Initializes the audio subsystem for a game. */
GariGame * gari_audio_init(GariGame * game, int frequency) {
  int res;
  res =  Mix_OpenAudio(frequency, MIX_DEFAULT_FORMAT, 
                       GARI_AUDIO_STEREO, GARI_AUDIO_BUFFER);    
  if(res == -1) {
    fprintf(stderr, "Mix_OpenAudio: %s\n", Mix_GetError());
    return NULL;
  }  
  return game;
}

/** Cleans up the audio subsystem for a game. */
void gari_audio_done(GariGame * game) {
  Mix_CloseAudio();
}

struct GariSound_ { 
  Mix_Chunk * chunk;
};


/** Allocates a new empty GariSound */
GariSound * gari_sound_new() {
  GariSound * sound = GARI_ALLOCATE(GariSound);
  if (!sound) { return NULL; }
  sound->chunk = NULL; /* ensure chunk is empty. */
  return sound;
}

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
  GariSound * sound = gari_sound_new();
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
  uint32_t    start;
  uint32_t    current;
};

/** Allocates a new empty GariMusic */
GariMusic * gari_music_new() {
  GariMusic * music = GARI_ALLOCATE(GariMusic);
  if (!music) { return NULL; }
  music->tune = NULL; /* ensure tune is empty. */
  return music;
}


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
  GariMusic * music = gari_sound_new(); 
  if(!music)        { return NULL; }
  music->tune       = NULL;
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


