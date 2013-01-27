#ifndef scegra_H_INCLUDED
#define scegra_H_INCLUDED


typedef struct Scegra_          Scegra;
typedef struct ScegraNode_      ScegraNode;
typedef struct ScegraStyle_     ScegraStyle;
typedef struct ScegraPoints_    ScegraPoints;


typedef void ScegraDraw  (ScegraNode * node);
typedef void ScegraUpdate(ScegraNode * node, double dt);


/* A very simple scene graph, mainly for drawing the UI that will be managed from 
 * the scripting side of things. 
 */

Scegra * scegra_alloc();
Scegra * scegra_free(Scegra * self);
Scegra * scegra_init(Scegra * self);
Scegra * scegra_done(Scegra * self);








#endif




