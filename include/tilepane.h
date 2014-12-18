#ifndef ERUTA_TILEPANE_H
#define ERUTA_TILEPANE_H

// Uses camera too
#include "camera.h"


typedef struct Tilepane_  Tilepane;


Tilepane * tilepane_done (Tilepane * pane );

Tilepane * tilepane_free (Tilepane * pane );

Tilepane * tilepane_init (Tilepane * pane , Tileset * set , int gridwide , int gridhigh );

Tilepane * tilepane_new (Tileset * set , int gridwide , int gridhigh );

int tilepane_gridwide (Tilepane * pane );

int tilepane_gridhigh (Tilepane * pane );

int tilepane_tilewide (Tilepane * pane );

int tilepane_tilehigh (Tilepane * pane );

int tilepane_wide (Tilepane * pane );

int tilepane_high (Tilepane * pane );

int tilepane_outsidegrid (Tilepane * pane , int gridx , int gridy );

Tile * tilepane_set (Tilepane * pane , int gridx , int gridy , Tile * tile );

Tile * tilepane_get (Tilepane * pane , int gridx , int gridy );

int tilepane_getindex (Tilepane * pane , int gridx , int gridy );

Tile * tilepane_rect (Tilepane * pane , int gridx , int gridy , int gridw , int gridh , Tile * tile );

Tile * tilepane_fill (Tilepane * pane , Tile * tile );

void tilepane_draw (Tilepane * pane , Camera * camera );

void tilepane_update (Tilepane * pane , double dt );

Tileset * tilepane_tileset_ (Tilepane * pane , Tileset * set );

Tileset * tilepane_tileset (Tilepane * pane );

Tile * tilepane_getfromset (Tilepane * pane , int index );

Tile * tilepane_setindex (Tilepane * pane , int gridx , int gridy , int index );

Tile * tilepane_rectindex (Tilepane * pane , int gridx , int gridy , int gridw , int gridh , int index );

Tile * tilepane_fillindex (Tilepane * pane , int index );

bool tilepane_init_blend(Tilepane * self, int index);

void tilepane_draw_blends(Tilepane * pane, Camera * camera);

void tilepane_draw_shadows_of(Tilepane * pane, Tilepane * pane_below, Camera * camera);

#endif
