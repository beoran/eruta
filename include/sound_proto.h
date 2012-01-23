#ifndef SOUND_PROTO_H
#define SOUND_PROTO_H
/*
This file was autogenerated from src/sound.c
by bin/genproto on 2012-01-21 18:34:10 +0100
Please do not hand edit.
*/

/** Global audio settings. */
static Audio audio_;

/** Stops the audio. */
void audio_stop();

/** Call this to enable sound. samples is the amount of samples to play.
* Returns TRUE if ok false if it failed.
*/
BOOL audio_start();

/** Returns true if sound can be played, false if not. */
BOOL sound_ok();

/** Frees memory associated with sound. */
Sound * sound_free(Sound * self);

/** Loads a sound from file. */
Sound * sound_load(char * filename);

/** Loads a sound from file. */
Music * music_load(char * filename);

/** Loads music from file, and starts to play it immediately. */


/** Plays the sound with detailed parameters. */
Sound * sound_playback(Sound * sound,
  float gain, float pan, float speed, BOOL loop);

/** Plays the sound once only with default parameters. */
Sound * sound_play(Sound * sound);

#endif // SOUND_PROTO_H

