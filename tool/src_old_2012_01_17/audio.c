#include "gy.h"

#include "SDL_mixer.h"

#define GY_INTERN_ONLY
#include "gy_intern.h"

#ifndef GY_AUDIO_BUFFER
#define GY_AUDIO_BUFFER 4096
#endif


#define GY_AUDIO_MONO   1
#define GY_AUDIO_STEREO 2

struct GyAudioInfo_ {
  int frequency;
  int channels; 
  int format;
};  


GyAudioInfo * gyaudioinfo_new(int freq, int form, int chan) {
  GyAudioInfo * info = GY_ALLOCATE(GyAudioInfo);
  if(!info) return NULL;
  info->frequency = freq;
  info->channels  = chan;
  info->format    = form;
  return info;  
} 

GyAudioInfo * gyaudioinfo_free(GyAudioInfo * info) {
  GY_FREE(info);
  return NULL;
}

int gyaudioinfo_frequency(GyAudioInfo * info) {
  if(!info) return -1;
  return info->frequency;
}

int gyaudioinfo_channels(GyAudioInfo * info) {
  if(!info) return -1;
  return info->channels;
}

int gyaudioinfo_format(GyAudioInfo * info) {
  if(!info) return -1;
  return info->format;
}


GyAudioInfo * gyaudio_query() {
  int frequency; Uint16 format; int channels; int res;  
  res = Mix_QuerySpec(&frequency, &format, &channels);
  if(res < 0) { return NULL ; } 
  return gyaudioinfo_new(frequency, format, channels);
}  



/** Initializes the audio subsystem for a game. */
GyGame * gyaudio_init(GyGame * game, int frequency) {
  int res;
  res =  Mix_OpenAudio(frequency, MIX_DEFAULT_FORMAT, 
                       GY_AUDIO_STEREO, GY_AUDIO_BUFFER);    
  if(res == -1) {
    fprintf(stderr, "Mix_OpenAudio: %s\n", Mix_GetError());
    return NULL;
  }  
  return game;
}

/** Cleans up the audio subsystem for a game. */
void gyaudio_done(GyGame * game) {
  Mix_CloseAudio();
}

struct GySound_ { 
  Mix_Chunk * chunk;
};


/** Allocates a new empty GySound */
GySound * gysound_new() {
  GySound * sound = GY_ALLOCATE(GySound);
  if (!sound) { return NULL; }
  sound->chunk = NULL; /* ensure chunk is empty. */
  return sound;
}

/** Initialises and loads sound into an existing GySound record. */
GySound * gysound_init(GySound * sound, char * filename) {
  if(!sound)        { return NULL; } 
  // Unload chunk first
  if (sound->chunk) {  gysound_done(sound); }
  sound->chunk    = Mix_LoadWAV(filename);
  if(!sound->chunk) { return NULL; }
  return sound;
}

/** Creates a new GySound and loads the sound from a file. */
GySound * gysound_load(char * filename) {
  GySound * sound = gysound_new();
  if (!sound) { return NULL; }
  if (!gysound_init(sound, filename)) {
    gysound_free(sound);
    return NULL;
  }
  return sound;
}

/** Deallocates the loaded sound, but not the wrapper GySound itself. */
GySound * gysound_done(GySound * sound) {
  if (!sound) return NULL;
  if (sound->chunk) Mix_FreeChunk(sound->chunk);
  sound->chunk = NULL; // set to null to signal it's unloaded. 
  return sound;
}

/** Calls gysound_done and then frees the sound itself.*/
GySound * gysound_free(GySound * sound) {
  if(!sound) return NULL;
  gysound_done(sound); 
  GY_FREE(sound);
  return NULL;
}


/** Plays a sound once. */
GySound * gysound_play(GySound * sound) {
  if ((!sound) || (!sound->chunk)) return NULL;
  Mix_PlayChannel(-1, sound->chunk, 0);
  return sound;
}


struct GyMusic_ {
  Mix_Music * tune;
  uint32_t    start;
  uint32_t    current;
};

/** Allocates a new empty GyMusic */
GyMusic * gymusic_new() {
  GyMusic * music = GY_ALLOCATE(GyMusic);
  if (!music) { return NULL; }
  music->tune = NULL; /* ensure tune is empty. */
  return music;
}


/** Initialises and loads music into an existing GyMusic record. */
GyMusic * gymusic_init(GyMusic * music, char * filename) {  
  if (!music)       { return NULL; }
  if (music->tune)  { gymusic_done(music); } 
  music->start    = 0;
  music->current  = 0;
  music->tune     = Mix_LoadMUS(filename);
  if (!music->tune) { return NULL; } 
  return music;
}

/** Creates a new GyMusic and loads the music from a file. */
GyMusic * gymusic_load(char * filename) {
  GyMusic * music = gymusic_new();
  if(!music)        { return NULL; }
  music->tune       = NULL;
  if(!gymusic_init(music, filename)) {
    gymusic_free(music); 
    return NULL; 
  }
  return music;  
}

/** Deallocates up the loaded music, but not the wrapper GyMusic itself. */
GyMusic * gymusic_done(GyMusic * music) {
  if((!music) || (!music->tune)) {
    return NULL;
  }
  Mix_FreeMusic(music->tune); 
  music->tune = NULL;
  return music;
}

/** Calls gymusic_done and then frees the music itself.*/
GyMusic * gymusic_free(GyMusic * music) {
  if(!music) return NULL;
  gymusic_done(music);
  GY_FREE(music);
  return NULL;
}

/** Starts playing the music, fading in after fade ms. */
GyMusic * gymusic_fadein(GyMusic * music, int loops, int fade) {
  if((!music) || (!music->tune)) return NULL;
  // SDL ticks are always milliseconds...
  music->start = SDL_GetTicks();
  Mix_FadeInMusic(music->tune, loops, fade);
  return music;
}

/** Stops playing the music, fading out after fade ms. */
GyMusic * gymusic_fadeout(GyMusic * music, int fade) {
  if((!music) || (!music->tune)) return NULL;
  music->current = SDL_GetTicks() + fade;
  Mix_FadeOutMusic(fade);
  return music;
} 


