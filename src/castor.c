#include <string.h>
#include "bad.h"
#include "castor.h"


/* Castor is a CAche and in-memory STOrage for Resources.  
* It's used to keep long-lived resources such as images, etc in memory. 
*/

typedef struct CastorNode_ CastorNode;

struct CastorNode_ {
  char        * name; 
  void        * data;
  bool          permanent;
  int           refcount;
  BadAatree     tree;
};

CastorNode * castornode_done(CastorNode * self) {
  free(self->name);
  self->name = NULL;
  /* XXX: free data correctly!!! */
  self->data      = NULL;
  self->permanent = FALSE;
  self->refcount  = 0;
  badaatree_initall(&self->tree, NULL, NULL, NULL, 0);
  return self;
}

CastorNode * 
castornode_initall(CastorNode * self, char * name, void * data, 
                   bool permanent) {
  self->name      = strdup(name);
  self->data      = data;
  self->permanent = FALSE;
  self->refcount  = 0;
  badaatree_initall(&self->tree, NULL, NULL, NULL, 0);                     
}

CastorNode * badaatree_castornode(BadAatree * tree) {
  return bad_container(tree, CastorNode, tree);
}

int castornode_compare(CastorNode * one, CastorNode * two) {
  return strcmp(one->name, two->name);
}

int castornode_setvalue(CastorNode * one, CastorNode * two) {
  castornode_done(one);
  castornode_initall(one, two->name, two->data, two->permanent);
  one->tree = two->tree;  
  return strcmp(one->name, two->name);
}


int castornode_badaatreecompare(BadAatree * one, BadAatree * two) {
  return castornode_compare(badaatree_castornode(one), badaatree_castornode(two));
}

int castornode_badaatreesetvalue(BadAatree * one, BadAatree * two) {
  return castornode_setvalue(badaatree_castornode(one), badaatree_castornode(two));
}













