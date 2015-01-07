

#include "eruta.h"
#include "toruby.h"
#include "rh.h"
#include "state.h"
#include "image.h"
#include "fifi.h"
#include "store.h"
#include "scegra.h"
#include "sound.h"
#include <mruby/hash.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/array.h>
#include "tr_macro.h"
#include "tr_audio.h"

TR_WRAP_NOARG_INT(tr_playing_samples_max, audio_playing_samples_max)
TR_WRAP_IIIIB_INT(tr_play_sound_ex, audio_play_sound_ex)
TR_WRAP_I_INT(tr_play_sound, audio_play_sound)
TR_WRAP_I_BOOL(tr_stop_sound,audio_stop_sound)

TR_WRAP_I_BOOL(tr_set_music, audio_set_music)
TR_WRAP_NOARG_BOOL(tr_play_music, audio_play_music)
TR_WRAP_NOARG_BOOL(tr_stop_music, audio_stop_music)
TR_WRAP_NOARG_BOOL(tr_music_playing_p, audio_music_playing_p)


/** Initialize mruby bindings to audio functionality.
 * Eru is the parent module, which is normally named "Eruta" on the
 * ruby side. */
int tr_audio_init(mrb_state * mrb, struct RClass * eru) {
  struct RClass *aud;
  /* Audio class/module and class/module methods. */
  aud = mrb_define_class_under(mrb, eru, "Audio" , mrb->object_class);

  TR_CLASS_METHOD_NOARG(mrb, aud, "playing_sounds_max", tr_playing_samples_max);
  TR_CLASS_METHOD_ARGC(mrb, aud , "play_sound_ex"     , tr_play_sound_ex, 4);
  TR_CLASS_METHOD_ARGC(mrb, aud , "play_sound"        , tr_play_sound, 1);
  TR_CLASS_METHOD_ARGC(mrb, aud , "stop_sound"        , tr_stop_sound, 1);
  TR_CLASS_METHOD_ARGC(mrb, aud , "music_id="         , tr_set_music, 1);
  TR_CLASS_METHOD_NOARG(mrb, aud , "play_music"       , tr_play_music);
  TR_CLASS_METHOD_NOARG(mrb, aud , "stop_music"       , tr_stop_music);
  TR_CLASS_METHOD_NOARG(mrb, aud , "music_playing?"   , tr_music_playing_p);

  return 0;
}
  

