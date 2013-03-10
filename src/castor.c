#include <string.h>
#include "bad.h"
#include "dynar.h"
#include "castor.h"


/* Castor is a CAche and in-memory STOrage for Resources.  
* It's used to keep long-lived resources such as images, etc in memory
* and reference count them. The resources can be looked up by name. 
* Permanent resorces are never unloaded.  
* Every type of data needs to be registered though a CastorType.
*/

typedef struct CastorType_ CastorType;
typedef struct CastorNode_ CastorNode;

/* Load and unload callback functionality. */
typedef void * CastorLoader(char * name, void * extra);
typedef void * CastorUnloader(void * data, void * extra);

struct CastorType_ { 
  int index;
  CastorLoader   * loader;
  CastorUnloader * unloader;
};


struct CastorNode_ {
  char        * name;
  void        * data;
  int           type;
  bool          permanent;
  int           refcount;
  /* BadAatree     tree; */
};

struct Castor_ {
  Dynar * types;
  Dynar * nodes;
};




CastorNode * castornode_done(CastorNode * self,  Castor * castor) {
  free(self->name);
  self->name = NULL;
  /* XXX: free data correctly!!! */
  self->data      = NULL;
  self->permanent = FALSE;
  self->refcount  = 0;
  return self;
}

CastorNode * 
castornode_initall(CastorNode * self, const char * name, void * data, 
                   bool permanent) {
  self->name      = strdup(name);
  self->data      = data;
  self->permanent = FALSE;
  self->refcount  = 0;
}

int castornode_compare(CastorNode * one, CastorNode * two) {
  return strcmp(one->name, two->name);
}













