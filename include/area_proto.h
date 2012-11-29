/* This file was generated with:
'cfunctions -c -aoff -n -w area_proto src/area.c' */
#ifndef CFH_AREA_PROTO
#define CFH_AREA_PROTO

/* From 'src/area.c': */

#line 49 "src/area.c"
int thing_id (Thing * thing );

#line 57 "src/area.c"
int thing_id_ (Thing * thing , int newid );

#line 73 "src/area.c"
Thing * thing_z_ (Thing * self , int z );

#line 83 "src/area.c"
int thing_setflag (Thing * self , int flag );

#line 88 "src/area.c"
int thing_unsetflag (Thing * self , int flag );

#line 95 "src/area.c"
int thing_flag_ (Thing * self , int flag , int set );

#line 100 "src/area.c"
int thing_flag (Thing * self , int flag );

#line 105 "src/area.c"
Thing * thing_done (Thing * self );

#line 122 "src/area.c"
Thing * thing_free (Thing * self );

#line 130 "src/area.c"
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
Thing * thing_alloc PROTO ((void));

#line 139 "src/area.c"
cpShape * shape_rectnew (cpBody * body , cpFloat x , cpFloat y , cpFloat w , cpFloat h , cpFloat xoff , cpFloat yoff );

#line 148 "src/area.c"
int thing_static_p (Thing * self );

#line 162 "src/area.c"
Thing * thing_initgeneric (Thing * self , Area * area , int kind , int z , cpBody * body , cpShape * shape );

#line 196 "src/area.c"
Thing * thing_initstatic (Thing * self , Area * area , int kind , int x , int y , int z , int w , int h );

#line 218 "src/area.c"
Thing * thing_initdynamic (Thing * self , Area * area , int kind , int x , int y , int z , int w , int h );

#line 247 "src/area.c"
Thing * thing_newstatic (Area * area , int kind , int x , int y , int z , int w , int h );

#line 258 "src/area.c"
Thing * thing_newdynamic (Area * area , int kind , int x , int y , int z , int w , int h );

#line 271 "src/area.c"
Point thing_p (Thing * self );

#line 276 "src/area.c"
int thing_x (Thing * self );

#line 281 "src/area.c"
int thing_y (Thing * self );

#line 286 "src/area.c"
int thing_z (Thing * self );

#line 291 "src/area.c"
Point thing_v (Thing * self );

#line 296 "src/area.c"
int thing_vx (Thing * self );

#line 301 "src/area.c"
int thing_vy (Thing * self );

#line 306 "src/area.c"
void thing_v_ (Thing * self , Point v );

#line 311 "src/area.c"
void thing_vxy_ (Thing * self , int vx , int vy );

#line 317 "src/area.c"
void thing_vx_ (Thing * self , int vx );

#line 324 "src/area.c"
void thing_vy_ (Thing * self , int vy );

#line 331 "src/area.c"
void thing_p_ (Thing * self , Point p );

#line 336 "src/area.c"
void thing_deltap (Thing * self , Point delta );

#line 343 "src/area.c"
void thing_pxy_ (Thing * self , int x , int y );

#line 349 "src/area.c"
void thing_x_ (Thing * self , int x );

#line 356 "src/area.c"
void thing_y_ (Thing * self , int y );

#line 363 "src/area.c"
void thing_applyforce (Thing * thing , const Point f );

#line 368 "src/area.c"
void thing_applyimpulse (Thing * thing , const Point f );

#line 373 "src/area.c"
void thing_resetforces (Thing * thing );

#line 381 "src/area.c"
void thing_draw (Thing * self , Camera * camera );

#line 454 "src/area.c"
int area_maxthings (Area * area );

#line 459 "src/area.c"
Thing * area_thing (Area * area , int index );

#line 467 "src/area.c"
Thing * area_thing_ (Area * area , int index , Thing * set );

#line 477 "src/area.c"
int area_thingid (Area * self );

#line 496 "src/area.c"
Thing * area_addthing (Area * area , Thing * thing );

#line 512 "src/area.c"
cpBody * area_staticbody (Area * area );

#line 520 "src/area.c"
int area_deletething (Area * area , int index );

#line 531 "src/area.c"
Area * area_cleanupthings (Area * self );

#line 542 "src/area.c"
Area * area_emptythings (Area * self );

#line 553 "src/area.c"
Area * area_done (Area * self );

#line 567 "src/area.c"
Area * area_free (Area * self );

#line 575 "src/area.c"
Area * area_alloc PROTO ((void));

#line 580 "src/area.c"
Area * area_init (Area * self );

#line 595 "src/area.c"
Area * area_new PROTO ((void));

#line 601 "src/area.c"
cpSpace * area_space (Area * self );

#line 609 "src/area.c"
Thing * area_newstatic (Area * self , int kind , int x , int y , int z , int w , int h );

#line 621 "src/area.c"
Thing * area_newdynamic (Area * self , int kind , int x , int y , int z , int w , int h );

#line 633 "src/area.c"
void area_draw (Area * self , Camera * camera );

#line 643 "src/area.c"
void area_update (Area * self , double dt );

#line 653 "src/area.c"

#ifdef COMMENT_
int thing_track (Tracker * tracker , void * data );

#endif

#endif /* CFH_AREA_PROTO */
