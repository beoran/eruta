/* This file was generated with:
'cfunctions -c -aoff -n -w rebox_proto src/rebox.c' */
#ifndef CFH_REBOX_PROTO
#define CFH_REBOX_PROTO

/* From 'src/rebox.c': */

#line 6 "src/rebox.c"
Rebox rebox_new (cpFloat x , cpFloat y , cpFloat w , cpFloat h );

#line 18 "src/rebox.c"
Point rebox_at (Rebox * self );

#line 23 "src/rebox.c"
Point rebox_x_ (Rebox * self , float x );

#line 29 "src/rebox.c"
Point rebox_y_ (Rebox * self , float y );

#line 36 "src/rebox.c"
Point rebox_xy_ (Rebox * self , float x , float y );

#line 43 "src/rebox.c"
Point rebox_at_ (Rebox * self , Point at );

#line 49 "src/rebox.c"
float rebox_x (Rebox * self );

#line 54 "src/rebox.c"
float rebox_y (Rebox * self );

#line 59 "src/rebox.c"
float rebox_w (Rebox * self );

#line 64 "src/rebox.c"
float rebox_h (Rebox * self );

#line 69 "src/rebox.c"
float rebox_br_x (Rebox * self );

#line 74 "src/rebox.c"
float rebox_br_y (Rebox * self );

#line 79 "src/rebox.c"
float rebox_center_x (Rebox * self );

#line 84 "src/rebox.c"
float rebox_center_y (Rebox * self );

#line 89 "src/rebox.c"
Point rebox_center (Rebox * self );

#line 95 "src/rebox.c"
Point rebox_center_ (Rebox * self , Point center );

#line 105 "src/rebox.c"
int rebox_inside_p (Rebox * self , Rebox * other );

#endif /* CFH_REBOX_PROTO */
