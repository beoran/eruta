/* This file was generated with:
'cfunctions -c -aoff -n -w camera_proto src/camera.c' */
#ifndef CFH_CAMERA_PROTO
#define CFH_CAMERA_PROTO

/* From 'src/camera.c': */

#line 42 "src/camera.c"
int panner_setflag (Panner * self , int flag );

#line 47 "src/camera.c"
int panner_unsetflag (Panner * self , int flag );

#line 54 "src/camera.c"
int panner_flag_ (Panner * self , int flag , int set );

#line 59 "src/camera.c"
int panner_flag (Panner * self , int flag );

#line 67 "src/camera.c"
Panner * panner_init (Panner * self , Point goal , float speed , int immediate );

#line 78 "src/camera.c"
Panner * panner_done (Panner * self );

#line 87 "src/camera.c"
PannerList * pannerlist_new (Point goal , float speed , int immediate );

#line 96 "src/camera.c"
PannerList * pannerlist_free (PannerList * self );

#line 104 "src/camera.c"
PannerList * pannerlist_freeall (PannerList * self );

#line 118 "src/camera.c"
Lockin * lockin_init (Lockin * self , float x , float y , float w , float h );

#line 126 "src/camera.c"
Lockin * lockin_done (Lockin * self );

#line 134 "src/camera.c"
LockinList * lockinlist_new (float x , float y , float w , float h );

#line 143 "src/camera.c"
LockinList * lockinlist_free (LockinList * self );

#line 151 "src/camera.c"
LockinList * lockinlist_freeall (LockinList * self );

#line 164 "src/camera.c"
int camera_setflag (Camera * self , int flag );

#line 169 "src/camera.c"
int camera_unsetflag (Camera * self , int flag );

#line 176 "src/camera.c"
int camera_flag_ (Camera * self , int flag , int set );

#line 181 "src/camera.c"
int camera_flag (Camera * self , int flag );

#line 185 "src/camera.c"
#line 1 "/usr/local/share/cfunctions/c-extensions.h"
/* Macro definitions for C extensions for Cfunctions. */

/* Own version of the cfunctions.h . */

#ifndef C_EXTENSIONS_H
#define C_EXTENSIONS_H

#ifndef X_NO_RETURN
#define X_NO_RETURN 
#endif

#ifndef X_PRINT_FORMAT
#define X_PRINT_FORMAT(a,b) 
#endif

#ifndef X_CONST
#define X_CONST
#endif

#ifndef INLINE
#define INLINE
#endif

#ifndef NO_RETURN
#define NO_RETURN void
#endif

#ifndef NO_SIDE_FX
#define NO_SIDE_FX
#endif

#ifndef PRINT_FORMAT
#define PRINT_FORMAT(a,b)
#endif

#ifndef LOCAL
#define LOCAL
#endif

#ifndef PROTO
#define PROTO(a) a
#endif 

#endif /* ifndef C_EXTENSIONS_H */
Camera * camera_alloc PROTO ((void));

#line 191 "src/camera.c"
Camera * camera_done (Camera * self );

#line 199 "src/camera.c"
Camera * camera_free (Camera * self );

#line 207 "src/camera.c"
Camera * camera_init (Camera * self , float x , float y , float w , float h );

#line 219 "src/camera.c"
Camera * camera_new (float x , float y , float w , float h );

#line 226 "src/camera.c"
Thing * camera_track_ (Camera * self , Thing * track );

#line 234 "src/camera.c"
int camera_panning_p (Camera * self );

#line 244 "src/camera.c"
int camera_lockin_p (Camera * self );

#line 252 "src/camera.c"
int camera_tracking_p (Camera * self );

#line 260 "src/camera.c"
int camera_applylockedtracking (Camera *self );

#line 269 "src/camera.c"
int camera_applynormaltracking (Camera *self );

#line 281 "src/camera.c"
int camera_applytracking (Camera *self );

#line 291 "src/camera.c"
int camera_applypanners (Camera * self );

#line 338 "src/camera.c"
int camera_applylockins (Camera * self );

#line 358 "src/camera.c"
Camera * camera_update (Camera * self );

#line 372 "src/camera.c"
Point camera_at (Camera * self );

#line 377 "src/camera.c"
Point camera_at_x_ (Camera * self , float x );

#line 382 "src/camera.c"
Point camera_at_y_ (Camera * self , float y );

#line 388 "src/camera.c"
Point camera_at_xy_ (Camera * self , float x , float y );

#line 393 "src/camera.c"
Point camera_at_ (Camera * self , Point at );

#line 399 "src/camera.c"
float camera_at_x (Camera * self );

#line 405 "src/camera.c"
float camera_at_y (Camera * self );

#line 410 "src/camera.c"
float camera_w (Camera * self );

#line 415 "src/camera.c"
float camera_h (Camera * self );

#line 420 "src/camera.c"
float camera_br_x (Camera * self );

#line 425 "src/camera.c"
float camera_br_y (Camera * self );

#line 430 "src/camera.c"
float camera_center_x (Camera * self );

#line 435 "src/camera.c"
float camera_center_y (Camera * self );

#line 440 "src/camera.c"
Point camera_center (Camera * self );

#line 446 "src/camera.c"
Point camera_center_ (Camera * self , Point center );

#line 453 "src/camera.c"
Point camera_centerdelta_ (Camera * self , Point newcenter , float deltax , float deltay );

#line 475 "src/camera.c"
Point camera_speed_deltaxy (Camera * self , float dx , float dy );

#line 482 "src/camera.c"
Point camera_speed_xy_ (Camera * self , float x , float y );

#line 489 "src/camera.c"
Point camera_speed (Camera * self );

#line 494 "src/camera.c"
Point camera_speed_ (Camera * self , Point speed );

#line 500 "src/camera.c"
Camera * camera_debugprint (Camera * self );

#line 510 "src/camera.c"
Panner * camera_newpanner (Camera * self , Point goal , float speed , int immediate );

#line 526 "src/camera.c"
Panner * camera_freetoppanner (Camera * self );

#line 539 "src/camera.c"
void camera_freepanners (Camera * self );

#line 546 "src/camera.c"
Lockin * camera_newlockin (Camera * self , float x , float y , float w , float h );

#line 562 "src/camera.c"
Lockin * camera_freetoplockin (Camera * self );

#line 575 "src/camera.c"
void camera_freelockins (Camera * self );

#line 583 "src/camera.c"
int camera_cansee (Camera * self , int x , int y , int w , int h );

#endif /* CFH_CAMERA_PROTO */
