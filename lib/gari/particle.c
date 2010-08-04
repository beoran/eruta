/** Gari particle engine for all sors of glitzy effects. :) */
#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"


struct GariDust_ {
  GariColor 	   	  color;
  GariDustDrawFunction  * draw;
  int		   	  lifetime;
  int		   	  x, y;
  
};

struct GariParticleEngine_ {
  size_t 	 size;
  GariDust     * particles;
  size_t	 active;
};


























