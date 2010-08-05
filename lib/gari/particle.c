/** Gari particle engine for all sorts of glitzy effects. :) */
#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"


struct GariDrop_ {
  GariFlow                *  flow;
  GariColor 	   	           color;
  GariDropDrawFunction    *  draw;
  GariDropUpdateFunction  *  update;
  int		   	  lifetime;
  int		   	  x, y;
  int         vx, vy;
  int         ax, ay;
};

GariDrop * gari_drop_init(GariDrop * drop, GariFlow * flow) {
  if(!drop) return NULL;
  drop->flow      = flow;
  drop->lifetime  = 0;
  drop->vx        = 0;
  drop->vy        = 0;
  drop->ax        = 0;
  drop->ay        = 0;
  return drop;
}

GariDrop * gari_drop_update(GariDrop * drop, int time) {
  if (!drop)              return NULL;
  if (!drop->update)      return NULL;
  if (drop->lifetime < 1) return NULL;
  // skip drops that are not active or not set up correctly. 
  drop->update(drop, time);
  return drop; 
}

GariDrop * gari_drop_draw(GariDrop * drop, GariImage * im) {
  if (!drop) return NULL;
  if (!drop->draw)        return NULL;
  if (drop->lifetime < 1) return NULL;
  // skip drops that are not active or not set up correctly.
  drop->draw(drop, im);  
  return drop; 
}


/* GariWell is a a point location source where particles are generated. 
 It's in other words a kind of initial configuration for particles. */
struct GariWell_ {
  GariColor                   color;
  int                         x, y;
  int                         kind;
  GariDropDrawFunction    *   draw;
  GariDropUpdateFunction  *   update;
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
  flow->particles = GARI_MALLOC(sizeof(GariFlow) * flow->size);
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
  drop->x               += drop->vx * time;
  drop->y               += drop->vy * time;
  return drop;
}
    
//Draws the drop as a small filled recrtangle
GariDrop * gari_drop_drawslab(GariDrop * drop, GariImage * im) {
  // gari_image_slab(im, , 2, 2, drop->color);
}


GariDrop * gari_drop_initsnow(GariDrop * drop) { 
  drop->x         = gari_random(0, drop->flow->screenw);
  drop->y         = gari_random(0, drop->flow->screenh);
  drop->vx        = 0;
  drop->vy        = 0;
  drop->lifetime  = gari_random(10, 100);
  return drop;
}  
      
GariDrop * gari_drop_updatesnow(GariDrop * drop, int time) {
  if (!drop->lifetime % 5) { 
    drop->vx = gari_random(-1, 1);
  } else {
    drop->vx = 0;
  }
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




GariWell * gari_well_init(GariWell * well, int x, int y, 
                          GariColor color, int kind) {
  if(!well) return NULL;
  well->x     = x;
  well->y     = y;
  well->color = color;
  well->kind  = kind;
  well->draw    = gari_drop_drawslab;
  well->update  = gari_drop_updatespeed;
  
  switch(well->kind) {
    case GariFlowSnow:
      well->update = gari_drop_updatesnow;  
      return well;
    default:  
      return well;
  }
  return well;
} 


/** 
* Finds the first unused GariDrop in a GariFlow. Returns NULL if all drops 
* are in use. 
*/
GariDrop * gari_flow_unused(GariFlow * flow) {
  int index;
  // find first unused droplet
  for (index = 0 ; index < flow->size ; index ++) {
    GariDrop * drop = flow->particles + index;
    if (drop->lifetime < 1) {       
      return drop;
    }
  }
  return NULL;
} 







