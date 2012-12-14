#ifndef GLH_H
#define GLH_H
#include <allegro5/allegro.h>
#include <allegro5/allegro_opengl.h>

/* This file was generated with:
'cfunctions -c -aoff -n -w glh_proto src/glh.c' */
#ifndef CFH_GLH_PROTO
#define CFH_GLH_PROTO

/* From 'src/glh.c': */

GLuint glh_shader_make (GLenum type , char * data );

GLuint gl_program_make (GLuint * shaders , int amount );

#endif /* CFH_GLH_PROTO */


#endif
