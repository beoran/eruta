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

#line 105 "src/camera.c"
int camera_setflag (Camera * self , int flag );

#line 110 "src/camera.c"
int camera_unsetflag (Camera * self , int flag );

#line 117 "src/camera.c"
int camera_flag_ (Camera * self , int flag , int set );

#line 122 "src/camera.c"
int camera_flag (Camera * self , int flag );

#line 126 "src/camera.c"
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

#line 132 "src/camera.c"
Camera * camera_done (Camera * self );

#line 140 "src/camera.c"
Camera * camera_free (Camera * self );

#line 148 "src/camera.c"
Camera * camera_init (Camera * self , float x , float y , float w , float h );

#line 158 "src/camera.c"
Camera * camera_new (float x , float y , float w , float h );

#line 165 "src/camera.c"
int camera_panning_p (Camera * self );

#line 180 "src/camera.c"
int camera_applypanners (Camera * self );

#line 227 "src/camera.c"
Camera * camera_update (Camera * self );

#line 237 "src/camera.c"
Point camera_at (Camera * self );

#line 242 "src/camera.c"
Point camera_at_x_ (Camera * self , float x );

#line 247 "src/camera.c"
Point camera_at_y_ (Camera * self , float y );

#line 253 "src/camera.c"
Point camera_at_xy_ (Camera * self , float x , float y );

#line 258 "src/camera.c"
Point camera_at_ (Camera * self , Point at );

#line 264 "src/camera.c"
float camera_at_x (Camera * self );

#line 270 "src/camera.c"
float camera_at_y (Camera * self );

#line 275 "src/camera.c"
float camera_w (Camera * self );

#line 280 "src/camera.c"
float camera_h (Camera * self );

#line 285 "src/camera.c"
float camera_br_x (Camera * self );

#line 290 "src/camera.c"
float camera_br_y (Camera * self );

#line 295 "src/camera.c"
float camera_center_x (Camera * self );

#line 300 "src/camera.c"
float camera_center_y (Camera * self );

#line 305 "src/camera.c"
Point camera_center (Camera * self );

#line 311 "src/camera.c"
Point camera_center_ (Camera * self , Point center );

#line 317 "src/camera.c"
Point camera_centerdelta_ (Camera * self , Point newcenter , float delta );

#line 333 "src/camera.c"
Point camera_speed_deltaxy (Camera * self , float dx , float dy );

#line 340 "src/camera.c"
Point camera_speed_xy_ (Camera * self , float x , float y );

#line 347 "src/camera.c"
Point camera_speed (Camera * self );

#line 352 "src/camera.c"
Point camera_speed_ (Camera * self , Point speed );

#line 358 "src/camera.c"
Camera * camera_debugprint (Camera * self );

#line 368 "src/camera.c"
Panner * camera_newpanner (Camera * self , Point goal , float speed , int immediate );

#line 384 "src/camera.c"
Panner * camera_freetoppanner (Camera * self );

#line 397 "src/camera.c"
void camera_freepanners (Camera * self );

#line 403 "src/camera.c"
int camera_cansee (Camera * self , int x , int y , int w , int h );

#endif /* CFH_CAMERA_PROTO */
