/* This file was generated with:
'cfunctions -c -aoff -n -w rebox_proto src/rebox.c' */
#ifndef CFH_REBOX_PROTO
#define CFH_REBOX_PROTO

/* From 'src/rebox.c': */

#line 7 "src/rebox.c"
Rebox rebox_new (cpFloat x , cpFloat y , cpFloat w , cpFloat h );

#line 19 "src/rebox.c"
Point rebox_at (Rebox * self );

#line 24 "src/rebox.c"
Point rebox_x_ (Rebox * self , float x );

#line 30 "src/rebox.c"
Point rebox_y_ (Rebox * self , float y );

#line 37 "src/rebox.c"
Point rebox_xy_ (Rebox * self , float x , float y );

#line 44 "src/rebox.c"
Point rebox_at_ (Rebox * self , Point at );

#line 50 "src/rebox.c"
float rebox_x (Rebox * self );

#line 55 "src/rebox.c"
float rebox_y (Rebox * self );

#line 60 "src/rebox.c"
float rebox_w (Rebox * self );

#line 65 "src/rebox.c"
float rebox_h (Rebox * self );

#line 70 "src/rebox.c"
float rebox_br_x (Rebox * self );

#line 75 "src/rebox.c"
float rebox_br_y (Rebox * self );

#line 80 "src/rebox.c"
float rebox_center_x (Rebox * self );

#line 85 "src/rebox.c"
float rebox_center_y (Rebox * self );

#line 90 "src/rebox.c"
Point rebox_center (Rebox * self );

#line 96 "src/rebox.c"
Point rebox_center_ (Rebox * self , Point center );

#line 106 "src/rebox.c"
int rebox_inside_p (Rebox * self , Rebox * other );

#line 118 "src/rebox.c"
double rebox_delta_x (Rebox * self , Rebox * other );

#line 130 "src/rebox.c"
double rebox_delta_y (Rebox * self , Rebox * other );

#endif /* CFH_REBOX_PROTO */
