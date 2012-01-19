/** Gy particle engine for all sorts of glitzy effects. :) */
#include "gy.h"

#define GY_INTERN_ONLY
#include "gy_intern.h"



// Speed and accelleration of particles is expressed 
// in units of 1 / 1024 th of a pixel per frame time, (or >> 10) 
// or 1/1024th of pixel per frame time squared (for accellerations)
#ifndef GY_DROP_SHIFT
#define GY_DROP_SHIFT 10 
#endif

// macro to scale speeds and accellerations.
#define GY_DROP_DIVIDE(VAL) 	((VAL) / 1024.0)
// macro to scale speeds and accellerations.
#define GY_DROP_MULTIPLY(VAL) ((VAL) * 1024.0)

typedef float GyNumber; 




struct GyDropClass_;
typedef struct GyDropClass_ GyDropClass;

struct GyDropClass_ {
  GyDropInitFunction    * init;
  GyDropDrawFunction    * draw;  
  GyDropUpdateFunction  * update;
};


GyDropClass * gydropclass_init(GyDropClass * klass, 
                                    GyDropUpdateFunction  * update,
                                    GyDropDrawFunction    * draw,  
                                    GyDropInitFunction    * init
                                   ){
  if (!klass) return NULL;
  klass->update = update; 
  klass->draw   = draw;
  klass->init   = init;
  return klass;
}



struct GyDrop_ {
  GyFlow                * flow;
  GyColor                 color;
  GyImage               * image;
  char                    * text;
  GyDropDrawFunction    * draw;
  GyDropUpdateFunction  * update;
  int                       kind;
  int                       lifetime;
  GyNumber                x, y;
  GyNumber                vx, vy;
  GyNumber                ax, ay;
};

/** Returns TRUE if this drop is idle(unused) or NULL, FALSE if not. */
int gydrop_idle(GyDrop * drop) { 
  if (!drop) return TRUE;
  if (drop->lifetime < 1) return TRUE;
  return FALSE;
}  

/** Returns TRUE if this drop is still active, FALSE if not. */
int gydrop_active(GyDrop * drop) { 
  return !gydrop_idle(drop);
}  

/** Deactivates a drop. */
void gydrop_disable(GyDrop * drop) {
  if(!drop) return;
  drop->lifetime = 0;
}

/** Deactivates a drop if it is of the given kind. */
void gydrop_disablekind(GyDrop * drop, int kind) {
  if (!drop) return;
  if (drop->kind != kind) return;
  gydrop_disable(drop);
}



GyDrop * gydrop_init(GyDrop * drop, GyFlow * flow) {
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



GyDrop * gydrop_update(GyDrop * drop, int time) {
  if (gydrop_idle(drop)) return NULL;
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
GyDrop * gydrop_drawslab(GyDrop * drop, GyImage * im) {
  gyimage_slab(im, drop->x, drop->y, 5, 5, drop->color);
  return drop;
}

// Draws the drop as a small filled circle
GyDrop * gydrop_drawdisk(GyDrop * drop, GyImage * im) {
  gyimage_slab(im, drop->x, drop->y, 3, 3, drop->color);
  return drop;
}

// Draws the drop as snow
GyDrop * gydrop_drawsnow(GyDrop * drop, GyImage * im) {
  gyimage_disk(im, drop->x, drop->y, 3, drop->color);
  // gyimage_slab(im, drop->x, drop->y, 3, 3, drop->color);
  return drop;
}


// Draws a generic drop.
GyDrop * gydrop_draw(GyDrop * drop, GyImage * im) {
  // Skip drops that are not active or not set up correctly.
  if (gydrop_idle(drop)) return NULL;
  
  // Draw images for drops that have an image
  if (drop->image) {
    gyimage_blit(im, drop->x, drop->y, drop->image); 
    return drop;
  }  
  // Use the draw function for drops that have one
  if (drop->draw)  {
     drop->draw(drop, im);
     return drop;
  }
  // Otherwise just draw a filled rectangle. 
  gydrop_drawslab(drop, im);
  return drop; 
}


/* GyWell is a a point location source where particles are generated. 
 It's in other words a kind of initial configuration for particles. */
struct GyWell_ {
  int                         kind;
  GyColor                   color;  
  GyNumber                  x, y;  
  GyImage               *   image;
  char                    *   text;
  GyDropDrawFunction    *   draw;
  GyDropUpdateFunction  *   update;
  GyDropInitFunction    *   init;
};


/* GyFlow models a particle engine. */
struct GyFlow_ {
  GyCamera   * camera;
  size_t 	       size;
  GyDrop     * particles;
  size_t	       active;
  int screenw, screenh;
};

GyFlow * gyflow_init(GyFlow * flow, size_t size) {
  int index;
  if(!flow) return NULL;
  flow->size      = size;
  flow->active    = 0;
  flow->screenw   = 640;
  flow->screenh   = 480;
  flow->particles = GY_MALLOC(sizeof(GyDrop) * flow->size);
  if(!flow->particles) {
    flow->size = 0;
    return NULL;
  }
  for(index = 0; index < flow->size; index++) {
    gydrop_init(flow->particles+index, flow);
  }
  
  return flow;
}


GyFlow * gyflow_done(GyFlow * flow) {
  if(!flow) { return NULL; }  
  flow->size   = 0;
  flow->active = 0;
  GY_FREE(flow->particles);
  flow->particles = NULL;
  return flow;  
}

GyFlow * gyflow_make(size_t size) {
  GyFlow * flow = GY_ALLOCATE(GyFlow);
  return gyflow_init(flow, size);
}

void gyflow_free(GyFlow * flow) {
  gyflow_done(flow);
  GY_FREE(flow);
}

void gyflow_update(GyFlow * flow, int time) {
  int index;
  // XXX: should perhaps be limited to active particles only?
  for(index = 0; index < flow->size ; index++) {
    gydrop_update(flow->particles+index, time);
  }
}
 
void gyflow_draw(GyFlow * flow, GyImage * im) {
  int index;
  for(index = 0; index < flow->size; index++) {
    gydrop_draw(flow->particles+index, im);
  }
}


// Automatically integrate motion based upon speed
// To be called in your custom callback, etc. 
GyDrop * gydrop_updatespeed(GyDrop * drop, int time) { 
  drop->x               += GY_DROP_DIVIDE(drop->vx * time);
  drop->y               += GY_DROP_DIVIDE(drop->vy * time);
  return drop;
}

// Automatically update speed based on accelleration
// To be called in your custom callback, etc. 
GyDrop * gydrop_updateaccell(GyDrop * drop, int time) { 
  drop->vx              += GY_DROP_DIVIDE(drop->ax * time);
  drop->vy              += GY_DROP_DIVIDE(drop->ay * time);
  return drop;
}


GyDrop * gydrop_initsnow(GyDrop * drop, GyWell * well) { 
  drop->x         = gyrandom(0, drop->flow->screenw);
  drop->y         = gyrandom(0, drop->flow->screenh);
  drop->ax	      = 0;
  drop->ay	      = GY_DROP_MULTIPLY(100);
  drop->vx        = 0;
  drop->vy        = gyrandom(GY_DROP_MULTIPLY(1), GY_DROP_MULTIPLY(3));
  drop->lifetime  = gyrandom(10, 100);
  return drop;
}  
      
GyDrop * gydrop_updatesnow(GyDrop * drop, int time) {
  if (!(drop->lifetime % 5)) { 
    drop->vx = GY_DROP_MULTIPLY(gyrandom(-1, 1));
  } else {
    drop->vx = 0;
  }
  gydrop_updateaccell(drop, time);
  return gydrop_updatespeed(drop, time);
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




GyWell * gywell_init(GyWell * well, int kind,  
                          int x, int y, GyColor color, 
                          GyImage * im, char * text) {
  if(!well) return NULL; 
  well->x       = x;
  well->y       = y;
  well->color   = color;
  well->kind    = kind;
  well->text    = text;
  well->image   = im;
  well->draw    = gydrop_drawslab;
  well->update  = gydrop_updatespeed;
  well->init    = gydrop_initsnow;
  
  switch(well->kind) {
    case GyFlowSnow:
      well->draw   = gydrop_drawsnow;
      well->update = gydrop_updatesnow;
      return well;
    default:  
      fprintf(stderr, "Unknown effect type.");
      return well;
  }
  return well;
} 

/** Initializes a drop with info from a Well */
GyDrop * gydrop_initwell(GyDrop * drop, GyFlow * flow, GyWell *well)
{
  if(!gydrop_init(drop, flow)) return NULL;
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
GyDrop * gyflow_idle(GyFlow * flow) {
  int index;
  if(!flow) return NULL;
  for (index = 0; index < flow->size; index++) {
    GyDrop * drop = flow->particles + index;
    if(gydrop_idle(drop)) return drop;
  } 
  return NULL;
} 

/**
* Activates amount Drops (particles) in the GyFlow flow. 
* The particle will be of kind kind, and will be placed, depending on the 
* kind, at x a,d y, and displayed using given color, image or text 
*/
GyFlow * gyflow_activate(GyFlow * flow, 
  int amount, int kind, int x, int y, GyColor color, 
  GyImage * im, char * text) {
  int index;
  GyWell well;
  if(!gywell_init(&well, kind, x, y, color, im, text)) { 
    return NULL; 
  } 
  for(index = 0; index < amount; index++) {
    GyDrop  * drop;
    drop = gyflow_idle(flow);
    if(!drop) {
      // fprintf(stderr, "No idle drops: %d!\n", amount ); 
      return NULL;
    }  
    gydrop_initwell(drop, flow, &well);
  } 
  return flow;
}



/**
* Deactivates all Drops of the given kind of effect.
*/
GyFlow * gyflow_disablekind(GyFlow * flow, int kind) {  
  int index;
  for (index = 0; index < flow->size; index++) {
    GyDrop * drop = flow->particles + index;
    gydrop_disablekind(drop, kind);    
  }   
  return flow;
}




