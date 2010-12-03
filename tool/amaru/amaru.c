/*
 Ruby extension for accelerated tile maps to be used in conjunction with Ruby/SDL 2.1.0
 

*/

#include "amaru.h"
 

 
static VALUE cSurface = Qnil;
static VALUE mSDL     = Qnil;
static VALUE mAmaru   = Qnil;
/* static VALUE cTile    = Qnil; */
static VALUE cLayer   = Qnil;

/*
static VALUE cSprite  = Qnil;
static VALUE cMap     = Qnil;
*/



/* Indirect surface that RubySDL uses. */
typedef struct {
  SDL_Surface* surface;
} RubySDL_Surface;

struct Tile_ {
  SDL_Surface *active;
  SDL_Surface *frames;
  int          index;
  int          size;
};

typedef struct Tile_ Tile;

typedef VALUE Element;

struct Layer_ {
  Element          **tiles;
  int                    w;
  int                    h;
  int               tile_w;
  int               tile_h;
  int                    x;
  int                    y;
  int                    z;
};

typedef struct Layer_ Layer;

struct Tileset_ {
  Tile *  tiles;
  int     size;
};

typedef struct Tileset_ Tileset;

struct Map_ {
  Layer * layers;
  int           size; 
};

typedef struct Map_ Map;

struct Sprite_ {
  SDL_Surface *active;
  SDL_Surface *frames;
  int          index;
  int          size;
  int          x;
  int          y;
  int          z;
};

typedef struct Sprite_ Sprite;

#define ALLOCATE(struct) malloc(sizeof(struct));

DEFINE_GET_STRUCT(Layer, GetLayer, cLayer, "Amaru::Layer");
DEFINE_GET_STRUCT(RubySDL_Surface, GetSurface, cSurface, "SDL::Surface");

SDL_Surface * Element_surface(Element tile) {
  RubySDL_Surface * rsdlsurf;
  if( !tile )   { return NULL; }
  rsdlsurf        = GetSurface(tile);
  if(!rsdlsurf) { return NULL; }
  return rsdlsurf->surface;  
}

Layer * Layer_init(Layer * self, int wide, int high, int tilewide, int tilehigh) {
  int index, jdex;
  self->w        = wide; 
  self->h        = high;
  self->tile_w   = tilewide;
  self->tile_h   = tilehigh;
  self->tiles    = malloc(self-> h * sizeof(Element *));
  if(!self->tiles) { return self; } 
  for (index = 0; index < self->h; index++) {
    self->tiles[index] = calloc(self->w , sizeof(Element));
    for(jdex = 0; jdex < self->w; jdex++) {
      self->tiles[index][jdex] = Qnil;
    }
  }  
  return self;
}

Layer *Layer_new(int wide, int high, int tilewide, int tilehigh) {
  Layer * self = ALLOCATE(Layer);
  if(!self) { return self; } 
  return Layer_init(self, wide, high, tilewide, tilehigh);
}

/* Layer does not own it's member tiles, just an array of pointers to them. */
void Layer_free(Layer * self) {
  int index;
  if(!self) { return; }
  for(index = 0; index < self->h; index++) {
    free(self->tiles[index]);
  }
  free(self->tiles);
  self->tiles = NULL;
  free(self);
}

void Layer_mark(Layer *self) {
  int xx; int yy;
  for (yy = 0; (yy < self->h) ; yy++) {
    for(xx = 0; xx < (self->w) ; xx++) {
      if (self->tiles[yy][xx]) {
        rb_gc_mark(self->tiles[yy][xx]);
        /* rb_mark(self->tiles[yy][xx]); */
      }  
    } 
  }
}

Layer * Layer_setsurface(Layer *self, int x, int y, Element surface) {
  if (x >= self->w)   { return self; }
  if (x < 0)          { return self; }
  if (y >= self->w)   { return self; }
  if (y < 0)          { return self; }
  self->tiles[y][x] = surface;
  return self; 
}

Element Layer_get(Layer *self, int x, int y) {
  if (x >= self->w)   { return Qnil; }
  if (x < 0)          { return Qnil; }
  if (y >= self->w)   { return Qnil; }
  if (y < 0)          { return Qnil; }
  return self->tiles[y][x]; 
}

static void surface_rect(SDL_Surface * to, short x, short y, unsigned short w, unsigned short h, Uint32 color) {
  SDL_Rect drect = {0,0,0,0};
  drect.x = x;
  drect.y = y;
  drect.w = w;
  drect.h = h;
  SDL_FillRect(to, &drect, color);
}

static int surface_blit(SDL_Surface * from, SDL_Surface * to, short x, short y) {
  SDL_Rect drect = {0,0,0,0};
  int res;
  Uint32 color;
  drect.x = x;
  drect.y = y;  
  /*
  color = SDL_MapRGB(to->format, rand()%255, rand()%255, rand()%255);
  color = SDL_MapRGB(to->format, 255, 255, 0); 
  surface_rect(to, x, y, 32, 32, color); 
  */
  res = SDL_BlitSurface(from, NULL, to, &drect); 
  return res; 
}
 


Layer * Layer_draw(Layer * self, SDL_Surface * target, int camera_x, int camera_y, int view_w, int view_h) {
    Sint16 tilewide   = self->tile_w;
    Sint16 tilehigh   = self->tile_h;
    Sint16 camx       = camera_x;
    Sint16 camy       = camera_y;
    Sint16 txstart    = (camx / self->tile_w);
    Sint16 tystart    = (camy / self->tile_h);
    Sint16 xtilestop  = (view_w   / self->tile_w) + 1;
    Sint16 ytilestop  = (view_h   / self->tile_h) + 1;
    Sint16 txstop     = xtilestop + txstart;
    Sint16 tystop     = ytilestop + tystart;
    Sint16 drawx      = 0;
    Sint16 drawy      = 0;
    Sint16 tydex      = 0;
    Sint16 txdex      = 0;
    SDL_Rect   drect  = {0,0,0,0};
    SDL_Rect   srect= { 0, 0, 32, 32}; 
    Element     *row= NULL;
    Element     tile= Qnil;
    SDL_Surface *aid= NULL;
    srect.w         = tilewide;
    srect.h         = tilehigh;
        
    if (txstart >= self->w || tystart >= self->h) { return self; }
    if (txstart < 0) { txstart = 0; }
    if (tystart < 0) { tystart = 0; }
    if (txstop > self->w) { txstop = self->w ; }
    if (tystop > self->h) { tystop = self->h ; }
    /* drect.w = tilewide;
    drect.h = tilehigh; */
    drawy   = -camy + ( (tystart-1) * tilehigh );
    tydex   = tystart;
    /*printf("tx: %d -> %d by %d ty: %d -> %d by %d (drawy: %d) view %d x %d \n", txstart, txstop, tilewide, tystart, tystop, tilehigh, drawy, view_w, view_h); 
    */   
    while (tydex < tystop) { 
      drawy   += tilehigh;
      drawx   =  -camx + ( (txstart-1) * tilewide );
      row     = self->tiles[tydex];
      txdex   = txstart;
      while (txdex < txstop) { 
        drawx   += tilewide; 
        tile     = self->tiles[tydex][txdex];
        if (tile != Qnil) {
          aid      = Element_surface(tile);
          if (aid) { 
            int res;
            /* drawy; */
            /* printf("draw %p -> %p tx: %hd@%hd , ty: %hd@%hd\n", aid, target, txdex, drawx, tydex, drawy); */
            res = surface_blit(aid, target, drawx, drawy);
            /* res = SDL_BlitSurface(aid, NULL, target, &drect); */
            /* printf("blit result: %d ", res);
            printf("blit area:  (%hd,%hu)  %hu x %hu\n", drect.x, drect.y, drect.w, drect.h);*/
          }
        }        
        txdex += 1;
      }
      tydex  += 1;
    }
  return self;
} 


Layer * Layer_testdraw(Layer * self, SDL_Surface * target, int camera_x, int camera_y, int view_w, int view_h) {
    int tx;
    int ty;
    int xx;
    int yy;
    int tilehigh = self->tile_h;
    int tilewide = self->tile_w;
    Element     tile= Qnil;
    SDL_Surface *aid= NULL;
    tile            = self->tiles[0][0];
    if (tile == Qnil) { return NULL; }
    aid             = Element_surface(tile);
    if (!aid) { return NULL; }
    xx         = -32 ;
    yy         = -32 ;
    for (ty = 0 ; ty < 3 ; ty ++) {  
     yy   += tilehigh;
     for (tx = 0 ; tx < 3 ; tx ++) { 
        xx += tilewide;
        printf("draw %p:  (%hd,%hd) %hdx%hd %p\n", aid, xx, yy, aid->w, aid->h, aid->pixels); 
        surface_blit(aid, target, xx, yy);
      }
      xx    = -32;
    }
  return self;
} 

VALUE cLayer_new(VALUE klass, VALUE wide, VALUE high, VALUE tw, VALUE th) { 
  Layer * layer = Layer_new(NUM2INT(wide), NUM2INT(high), NUM2INT(tw), NUM2INT(th));  
  return Data_Wrap_Struct(cLayer, Layer_mark, Layer_free, layer); 
}


VALUE cLayer_set(VALUE self, VALUE x, VALUE y, VALUE img) {
  Layer * layer           = GetLayer(self);
  Layer_setsurface(layer, NUM2INT(x), NUM2INT(y), img);
  return self;
}

VALUE cLayer_get(VALUE self, VALUE x, VALUE y) {
  Layer * layer           = GetLayer(self);
  return Layer_get(layer, NUM2INT(x), NUM2INT(y));
}


VALUE cLayer_w(VALUE self) {
  Layer * layer           = GetLayer(self);
  return INT2NUM(layer->w);
}

VALUE cLayer_h(VALUE self) {
  Layer * layer           = GetLayer(self);
  return INT2NUM(layer->h);
}

VALUE cLayer_tile_w(VALUE self) {
  Layer * layer           = GetLayer(self);
  return INT2NUM(layer->tile_w);
}

VALUE cLayer_tile_h(VALUE self) {
  Layer * layer           = GetLayer(self);
  return INT2NUM(layer->tile_h);
}


VALUE cLayer_draw(VALUE self, VALUE tar, VALUE x, VALUE y, VALUE h, VALUE w) {
  SDL_Surface * target;
  Layer       * layer   = GetLayer(self);
  target                = Element_surface(tar);
  if (!target) { return Qnil; }
  Layer_draw(layer, target, NUM2INT(x), NUM2INT(y), NUM2INT(h), NUM2INT(w)); 
  return self;
}

VALUE cLayer_testdraw(VALUE self, VALUE tar, VALUE x, VALUE y, VALUE h, VALUE w) {
  SDL_Surface * target;
  Layer       * layer   = GetLayer(self);
  target                = Element_surface(tar);
  if (!target) { return Qnil; }
  Layer_testdraw(layer, target, NUM2INT(x), NUM2INT(y), NUM2INT(h), NUM2INT(w)); 
  return self;
}


void Init_amaru() {  
  mSDL     = rb_const_get(rb_cObject, rb_intern("SDL"));
  if (mSDL == Qnil) {
    rb_raise(rb_eRuntimeError, "Ruby/SDL not available. Did you forget to require it?");    
  } 
  cSurface = rb_const_get(mSDL, rb_intern("Surface")); 
  if (cSurface == Qnil) {
    rb_raise(rb_eRuntimeError, "SDL::Surface not available. How could this be?");
  }
  mAmaru = rb_define_module("Amaru");
  cLayer = rb_define_class_under(mAmaru, "Layer", rb_cObject);
  rb_define_singleton_method(cLayer,"new", cLayer_new, 4);
  rb_define_method(cLayer , "set"   , cLayer_set , 3);
  rb_define_method(cLayer , "get"   , cLayer_get , 2);
  rb_define_method(cLayer , "draw"  , cLayer_draw , 5);
  rb_define_method(cLayer , "testdraw", cLayer_testdraw , 5);
  rb_define_method(cLayer , "w"     , cLayer_w , 0);
  rb_define_method(cLayer , "h"     , cLayer_h , 0);  
  rb_define_method(cLayer , "tile_w", cLayer_tile_w, 0);
  rb_define_method(cLayer , "tile_h", cLayer_tile_h, 0);
  
  
  
  
}
