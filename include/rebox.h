#ifndef rebox_H_INCLUDED
#define rebox_H_INCLUDED


#include "eruta.h"

typedef struct Rebox_ Rebox;

/** Rebox is short for REctangular BOX. It's a rectangular 
box with given position and size. */
struct Rebox_ {
  Point at;
  Point size;
};


/* This file was generated with:
'cfunctions -c -aoff -n -w rebox_proto src/rebox.c' */
#ifndef CFH_REBOX_PROTO
#define CFH_REBOX_PROTO

/* From 'src/rebox.c': */

Rebox rebox_make (int x , int y , int w , int h );

Rebox * rebox_initbounds (Rebox * self , Rebox bounds );

Rebox * rebox_init (Rebox * self , int x , int y , int w , int h );

Rebox rebox_new (cpFloat x , cpFloat y , cpFloat w , cpFloat h );

Point rebox_at (Rebox * self );

Point rebox_x_ (Rebox * self , float x );

Point rebox_y_ (Rebox * self , float y );

Point rebox_xy_ (Rebox * self , float x , float y );

Point rebox_at_ (Rebox * self , Point at );

float rebox_x (Rebox * self );

float rebox_y (Rebox * self );

float rebox_w (Rebox * self );

float rebox_h (Rebox * self );

Point rebox_br(Rebox * self);

float rebox_br_x (Rebox * self );

float rebox_br_y (Rebox * self );

float rebox_center_x (Rebox * self );

float rebox_center_y (Rebox * self );

Point rebox_center (Rebox * self );

Point rebox_center_ (Rebox * self , Point center );

int rebox_inside_p (Rebox * self , Rebox * other );

double rebox_delta_x (Rebox * self , Rebox * other );

double rebox_delta_y (Rebox * self , Rebox * other );

#endif /* CFH_REBOX_PROTO */



#endif




