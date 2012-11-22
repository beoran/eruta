/* This file was generated with:
'cfunctions -aoff -n -m -w main_proto src/main.c' */
#ifndef CFH_MAIN_PROTO
#define CFH_MAIN_PROTO

/* From 'src/main.c': */

#line 23 "src/main.c"
extern ALLEGRO_FONT * font;

#line 24 "src/main.c"
extern ALLEGRO_TRANSFORM identity;

#line 25 "src/main.c"
extern ALLEGRO_BITMAP * buffer;

#line 26 "src/main.c"
extern ALLEGRO_BITMAP * texture;

#line 27 "src/main.c"
extern ALLEGRO_COLOR solid_white;
/* Testing function for al_get_standard_path */

#line 49 "src/main.c"
void puts_standard_path (int path , char * name );

#line 61 "src/main.c"
React * main_react_key_down (React * self , ALLEGRO_KEYBOARD_EVENT * event );

#line 103 "src/main.c"
React * main_react_key_up (React * self , ALLEGRO_KEYBOARD_EVENT * event );

#line 140 "src/main.c"
int real_main (void);

#endif /* CFH_MAIN_PROTO */
