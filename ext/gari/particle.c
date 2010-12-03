/** Gari particle engine for all sorts of glitzy effects. :) */
#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"



// Speed and accelleration of particles is expressed 
// in units of 1 / 1024 th of a pixel per frame time, (or >> 10) 
// or 1/1024th of pixel per frame time squared (for accellerations)
#ifndef GARI_DROP_SHIFT
#define GARI_DROP_SHIFT 10 
#endif

// macro to scale speeds and accellerations.
#define GARI_DROP_DIVIDE(VAL) 	((VAL) / 1024.0)
// macro to scale speeds and accellerations.
#define GARI_DROP_MULTIPLY(VAL) ((VAL) * 1024.0)

typedef float GariNumber; 




struct GariDropClass_;
typedef struct GariDropClass_ GariDropClass;

struct GariDropClass_ {
  GariDropInitFunction    * init;
  GariDropDrawFunction    * draw;  
  GariDropUpdateFunction  * update;
};


GariDropClass * gari_dropclass_init(GariDropClass * klass, 
                                    GariDropUpdateFunction  * update,
                                    GariDropDrawFunction    * draw,  
                                    GariDropInitFunction    * init
                                   ){
  if (!klass) return NULL;
  klass->update = update; 
  klass->draw   = draw;
  klass->init   = init;
  return klass;
}



struct GariDrop_ {
  GariFlow                * flow;
  GariColor                 color;
  GariImage               * image;
  char                    * text;
  GariDropDrawFunction    * draw;
  GariDropUpdateFunction  * update;
  int                       kind;
  int                       lifetime;
  GariNumber                x, y;
  GariNumber                vx, vy;
  GariNumber                ax, ay;
};

/** Returns TRUE if this drop is idle(unused) or NULL, FALSE if not. */
int gari_drop_idle(GariDrop * drop) { 
  if (!drop) return TRUE;
  if (drop->lifetime < 1) return TRUE;
  return FALSE;
}  

/** Returns TRUE if this drop is still active, FALSE if not. */
int gari_drop_active(GariDrop * drop) { 
  return !gari_drop_idle(drop);
}  

/** Deactivates a drop. */
void gari_drop_disable(GariDrop * drop) {
  if(!drop) return;
  drop->lifetime = 0;
}

/** Deactivates a drop if it is of the given kind. */
void gari_drop_disablekind(GariDrop * drop, int kind) {
  if (!drop) return;
  if (drop->kind != kind) return;
  gari_drop_disable(drop);
}



GariDrop * gari_drop_init(GariDrop * drop, GariFlow * flow) {
  if(!drop) return NULL  ;
  drop->flow      = flow;
  drop->lifetime  = 0;
  drop->vx        = 0;
  drop->vy        = 0;
  drop->ax        = 0;
  drop->ay        = 0;
  drop->text      = NULL;
  drop->image     = NULL;
  drop->update	  = NULL;
  drop->draw	  = NULL;
  return drop;
}



GariDrop * gari_drop_update(GariDrop * drop, int time) {
  if (gari_drop_idle(drop)) return NULL;
  // automatically reduce lifetime
  drop->lifetime -= time;
  
  if (!drop->update)        {
    // fprintf(stderr, "cannot update drop!\n");
    return NULL;
  }  
  // skip drops that are not active or not set up correctly. 
  drop->update(drop, time);
  return drop; 
}

// Draws the drop as a small filled rectangle
GariDrop * gari_drop_drawslab(GariDrop * drop, GariImage * im) {
  gari_image_slab(im, drop->x, drop->y, 5, 5, drop->color);
  return drop;
}

// Draws the drop as a small filled circle
GariDrop * gari_drop_drawdisk(GariDrop * drop, GariImage * im) {
  gari_image_slab(im, drop->x, drop->y, 3, 3, drop->color);
  return drop;
}

// Draws the drop as snow
GariDrop * gari_drop_drawsnow(GariDrop * drop, GariImage * im) {
  gari_image_disk(im, drop->x, drop->y, 3, drop->color);
  // gari_image_slab(im, drop->x, drop->y, 3, 3, drop->color);
  return drop;
}


// Draws a generic drop.
GariDrop * gari_drop_draw(GariDrop * drop, GariImage * im) {
  // Skip drops that are not active or not set up correctly.
  if (gari_drop_idle(drop)) return NULL;
  
  // Draw images for drops that have an image
  if (drop->image) {
    gari_image_blit(im, drop->x, drop->y, drop->image); 
    return drop;
  }  
  // Use the draw function for drops that have one
  if (drop->draw)  {
     drop->draw(drop, im);
     return drop;
  }
  // Otherwise just draw a filled rectangle. 
  gari_drop_drawslab(drop, im);
  return drop; 
}


/* GariWell is a a point location source where particles are generated. 
 It's in other words a kind of initial configuration for particles. */
struct GariWell_ {
  int                         kind;
  GariColor                   color;  
  GariNumber                  x, y;  
  GariImage               *   image;
  char                    *   text;
  GariDropDrawFunction    *   draw;
  GariDropUpdateFunction  *   update;
  GariDropInitFunction    *   init;
};


/* GariFlow models a particle engine. */
struct GariFlow_ {
  GariCamera   * camera;
  size_t 	       size;
  GariDrop     * particles;
  size_t	       active;
  int screenw, screenh;
};

GariFlow * gari_flow_init(GariFlow * flow, size_t size) {
  int index;
  if(!flow) return NULL;
  flow->size      = size;
  flow->active    = 0;
  flow->screenw   = 640;
  flow->screenh   = 480;
  flow->particles = GARI_MALLOC(sizeof(GariDrop) * flow->size);
  if(!flow->particles) {
    flow->size = 0;
    return NULL;
  }
  for(index = 0; index < flow->size; index++) {
    gari_drop_init(flow->particles+index, flow);
  }
  
  return flow;
}


GariFlow * gari_flow_done(GariFlow * flow) {
  if(!flow) { return NULL; }  
  flow->size   = 0;
  flow->active = 0;
  GARI_FREE(flow->particles);
  flow->particles = NULL;
  return flow;  
}

GariFlow * gari_flow_make(size_t size) {
  GariFlow * flow = GARI_ALLOCATE(GariFlow);
  return gari_flow_init(flow, size);
}

void gari_flow_free(GariFlow * flow) {
  gari_flow_done(flow);
  GARI_FREE(flow);
}

void gari_flow_update(GariFlow * flow, int time) {
  int index;
  // XXX: should perhaps be limited to active particles only?
  for(index = 0; index < flow->size ; index++) {
    gari_drop_update(flow->particles+index, time);
  }
}
 
void gari_flow_draw(GariFlow * flow, GariImage * im) {
  int index;
  for(index = 0; index < flow->size; index++) {
    gari_drop_draw(flow->particles+index, im);
  }
}


// Automatically integrate motion based upon speed
// To be called in your custom callback, etc. 
GariDrop * gari_drop_updatespeed(GariDrop * drop, int time) { 
  drop->x               += GARI_DROP_DIVIDE(drop->vx * time);
  drop->y               += GARI_DROP_DIVIDE(drop->vy * time);
  return drop;
}

// Automatically update speed based on accelleration
// To be called in your custom callback, etc. 
GariDrop * gari_drop_updateaccell(GariDrop * drop, int time) { 
  drop->vx              += GARI_DROP_DIVIDE(drop->ax * time);
  drop->vy              += GARI_DROP_DIVIDE(drop->ay * time);
  return drop;
}


GariDrop * gari_drop_initsnow(GariDrop * drop, GariWell * well) { 
  drop->x         = gari_random(0, drop->flow->screenw);
  drop->y         = gari_random(0, drop->flow->screenh);
  drop->ax	      = 0;
  drop->ay	      = GARI_DROP_MULTIPLY(100);
  drop->vx        = 0;
  drop->vy        = gari_random(GARI_DROP_MULTIPLY(1), GARI_DROP_MULTIPLY(3));
  drop->lifetime  = gari_random(10, 100);
  return drop;
}  
      
GariDrop * gari_drop_updatesnow(GariDrop * drop, int time) {
  if (!(drop->lifetime % 5)) { 
    drop->vx = GARI_DROP_MULTIPLY(gari_random(-1, 1));
  } else {
    drop->vx = 0;
  }
  gari_drop_updateaccell(drop, time);
  return gari_drop_updatespeed(drop, time);
}
      
/*  
      # Draws a single particle. 
      # Override this in child classes.
      def render_particle(x, y, surface, particle, ci) 
        x2 = x
        y2 = y + rand(5) + 2
        l  = 7 + rand(3)
        w  = 1 #  + rand(2) 
        # surface.draw_line(x, y, x2, y2, ci)
        surface.draw_rect(x, y, w, l, ci, true)
      end
    end
  
  
    class Blood < Stream
      BLOOD_G = DUST_TIME + 1
      def initialize(color = [255, 0, 0], amount = 15, max = 100)
        super
      end
      
      # Sets up an individual particle. Overide this in child clases.
      def init_particle()
        x     = 0.0
        y     = 0.0
        vx    = (rand(1001)- 500) / 1000.0  
        vy    = 0
        g     = 0.00000001
        # Speeds set randomly for this kind of stream          
        time  = 0 # rand(100)
        # Randomly shorten their lifetime
        return Stardust.make_particle(x, y, vx, vy, time, g)
      end
      
      def update_particle(particle, time)
        time = particle[DUST_TIME]
        if time < @timemax * 0.1
            particle[DUST_Y] -= (rand(100) + 1) / 25.0 
        elsif time  < @timemax * 0.2
          particle[DUST_Y] += (rand(100) + 1) / 25.0 
          particle[DUST_VX] /= 2   
        else
          particle[DUST_VY]  = 0
          particle[DUST_VX]  = 0   
        end
        # particle[BLOOD_G]
        # particle[DUST_VY] = 2 if particle[DUST_VY] > -2
        if time > @timemax
          return false
        end     
        return true     
      end  
     
      # Draws a single particle. 
      # Override this in child classes.
      def render_particle(x, y, surface, particle, ci) 
        surface.draw_circle(x, y, 2,  ci, true, 0.5)
      end
    end


    class Slash < Stream
      SLASH_SX = DUST_TIME + 1
      SLASH_SY = DUST_TIME + 2
      SLASH_W  = DUST_TIME + 3
      def initialize(color = [255, 0, 0], amount = 1, max = 30)
        super
      end
      
      # Sets up an individual particle. Overide this in child clases.
      def init_particle()
        x     = 0.0
        y     = 0.0
        vx    = 0.5
        vy    = 1
        g     = 0.00000001
        # Speeds set randomly for this kind of stream          
        time  = 0 # rand(100)
        # Randomly shorten their lifetime
        return Stardust.make_particle(x, y, vx, vy, time, 0.0, 0.0, 1)
      end
      
      def update_particle(particle, time)
        time = particle[DUST_TIME]
        # particle[BLOOD_G]
        # particle[DUST_VY] = 2 if particle[DUST_VY] > -2
        if time > @timemax
          return false
        end     
        return true     
      end  
     
      # Draws a single particle. 
      # Override this in child classes.
      def render_particle(x, y, surface, particle, ci) 
        dx = x - particle[DUST_X]
        dy = y - particle[DUST_Y]
        sx = dx + particle[SLASH_SX]
        sy = dy + particle[SLASH_SY]
        sw = particle[SLASH_W]
        surface.draw_line(x, y, sx, sy, ci, true)
        surface.draw_line(x, y+sw, sx, sy+sw, ci, true)
        surface.draw_line(x, y-sw, sx, sy-sw, ci, true)
      end
    end
*/




GariWell * gari_well_init(GariWell * well, int kind,  
                          int x, int y, GariColor color, 
                          GariImage * im, char * text) {
  if(!well) return NULL; 
  well->x       = x;
  well->y       = y;
  well->color   = color;
  well->kind    = kind;
  well->text    = text;
  well->image   = im;
  well->draw    = gari_drop_drawslab;
  well->update  = gari_drop_updatespeed;
  well->init    = gari_drop_initsnow;
  
  switch(well->kind) {
    case GariFlowSnow:
      well->draw   = gari_drop_drawsnow;
      well->update = gari_drop_updatesnow;
      return well;
    default:  
      fprintf(stderr, "Unknown effect type.");
      return well;
  }
  return well;
} 

/** Initializes a drop with info from a Well */
GariDrop * gari_drop_initwell(GariDrop * drop, GariFlow * flow, GariWell *well)
{
  if(!gari_drop_init(drop, flow)) return NULL;
  if(!well) return NULL;
  drop->x         = well->x;
  drop->y         = well->y;
  drop->color     = well->color;
  drop->image     = well->image;
  drop->text      = well->text;
  drop->kind	    = well->kind;
  drop->update	  = well->update;
  drop->draw	    = well->draw;
  
  if(well->init) {
    well->init(drop, well);
  }
  return drop;
}


/**
* Gets the next idle (unused) drop from the particle engine. 
* Returns NULL if no idle drop is available. 
*/
GariDrop * gari_flow_idle(GariFlow * flow) {
  int index;
  if(!flow) return NULL;
  for (index = 0; index < flow->size; index++) {
    GariDrop * drop = flow->particles + index;
    if(gari_drop_idle(drop)) return drop;
  } 
  return NULL;
} 

/**
* Activates amount Drops (particles) in the GariFlow flow. 
* The particle will be of kind kind, and will be placed, depending on the 
* kind, at x a,d y, and displayed using given color, image or text 
*/
GariFlow * gari_flow_activate(GariFlow * flow, 
  int amount, int kind, int x, int y, GariColor color, 
  GariImage * im, char * text) {
  int index;
  GariWell well;
  if(!gari_well_init(&well, kind, x, y, color, im, text)) { 
    return NULL; 
  } 
  for(index = 0; index < amount; index++) {
    GariDrop  * drop;
    drop = gari_flow_idle(flow);
    if(!drop) {
      // fprintf(stderr, "No idle drops: %d!\n", amount ); 
      return NULL;
    }  
    gari_drop_initwell(drop, flow, &well);
  } 
  return flow;
}



/**
* Deactivates all Drops of the given kind of effect.
*/
GariFlow * gari_flow_disablekind(GariFlow * flow, int kind) {  
  int index;
  for (index = 0; index < flow->size; index++) {
    GariDrop * drop = flow->particles + index;
    gari_drop_disablekind(drop, kind);    
  }   
  return flow;
}




