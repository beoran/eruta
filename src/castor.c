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


struct CastorNode_ {
  char        * name;
  void        * data;
  void        * extra;
  CastorType  * type;
  int           flags;
  bool          permanent;
  int           refcount;
  /* BadAatree     tree; */
};

struct Castor_ {
  Dynar * types;
  Dynar * nodes;
  int used_types;
  int used_nodes;
  int sorted;
};



CastorNode * castornode_done(CastorNode * self) {
  free(self->name);
  self->name = NULL;
  self->type->unloader(self->data, self->extra); 
  /* XXX: free data correctly!!! */
  self->data      = NULL;
  self->permanent = FALSE;
  self->refcount  = 0;
  self->type      = NULL;
  self->extra     = NULL;
  return self;
}

CastorNode * castornode_free(CastorNode * self) {
  if(!castornode_done(self)) return NULL;
  return mem_free(self);
}

CastorNode * 
castornode_initall(CastorNode * self, const char * name, CastorType * type, int flags,
                   void * data, void * extra, bool permanent) {
  if(!self)         return NULL;
  self->name      = strdup(name);
  self->flags     = flags;
  self->type      = type; 
  self->data      = data;
  self->extra     = extra;
  self->permanent = permanent;
  self->refcount  = 0;
  return self;
}

CastorNode * castornode_newall(const char * name, CastorType * type, int flags, 
                               void * data, void * extra, bool permanent) {
  CastorNode * self = STRUCT_ALLOC(CastorNode);
  return castornode_initall(self, name, type, flags, data, extra, permanent);
}


int castornode_compare(const CastorNode * one, const CastorNode * two) {
  return strcmp(one->name, two->name);
}

int castornode_compare_qsort(const void * one, const void * two) {
  if(!one) { 
    if(!two) return 0;
    return 1;
  } else if(!two) { 
    return -1;
  }
  return castornode_compare((const CastorNode *)one, (const CastorNode *)two);
}

int castornode_compare_bsearch_cstr(const void * one, const void * two) {
  const char * str        = one;
  const CastorNode * node = two;
  return strcmp(str, node->name);
}


Castor * castor_alloc() {
  return STRUCT_ALLOC(Castor);
}

#define CASTOR_INIT_TYPES 32
#define CASTOR_INIT_NODES 1024

void * castor_node_destructor(void * data) {
  CastorNode * node = data;
  return castornode_free(node);
}

Castor * castor_done(Castor * self) {
  if (!self) return NULL;
  /* No cleanup for types, they are not owned (probably in static structs) */
  self->types      = mem_free(self->types);
  /* Clean up nodes */
  dynar_free_destroy(self->nodes, castor_node_destructor);
  self->nodes      = mem_free(self->nodes);
  self->used_types = -1;
  self->used_nodes = -1;
  return self;
}


Castor * castor_init(Castor * self) {
  if (!self) return NULL;
  self->types = dynar_newptr(CASTOR_INIT_TYPES);
  self->nodes = dynar_newptr(CASTOR_INIT_NODES);
  if ((!self->types) || (self->nodes)) { 
    castor_done(self);
    return NULL;
  }
  dynar_putnullall(self->types);
  dynar_putnullall(self->nodes);
  self->used_types = 0;
  self->used_nodes = 0;
  self->sorted     = FALSE;
  return self;
}

Castor * castor_free(Castor * self) {
  if (!castor_done(self)) return NULL;
  return mem_free(self);
}

Castor * castor_new() {
  Castor * self = castor_alloc();
  return castor_init(self);
}

CastorType * castor_addtype(Castor * self, int index, CastorType * type) {
  if (!self) return NULL;
  if (!dynar_putptr(self->nodes, index, type)) return NULL;
  return type;
}

CastorType * castor_gettype(Castor * self, int type) {
  if (!self) return NULL;
  return dynar_getptr(self->types, type);
}

CastorNode * castor_appendnode(Castor * self, CastorNode * node) {
  if (!self) return NULL;
  if (!dynar_putptr(self->nodes, self->used_nodes, node)) return NULL;
  self->used_nodes++;
  return node;  
}

CastorNode * castor_loadnode
(Castor * self, const char * name, int typeno, int flags, void * extra, 
 bool permanent) {
  CastorNode * node;
  CastorType * type; 
  void * data;
  type = castor_gettype(self, typeno);
  if(!type) return NULL;
  data = type->loader(name, typeno, flags, extra); 
  if (!data) return NULL;
  node = castornode_newall(name, type, flags, data, extra, permanent);
  if(!node) { 
    type->unloader(node, extra);
    return NULL;
  }
  if(!castor_appendnode(self, node)) {
    return castornode_free(node);
  } 
  self->sorted = false;
  return node;
}

Castor * castor_sort(Castor * self) {
  if(!self) return NULL;
  dynar_qsort(self->nodes, castornode_compare_qsort);
  self->sorted = TRUE;
  return self;
}

CastorNode * castor_getnode(Castor * self, const char * name) {
  CastorNode * result;
  if (!self) return NULL;
  if(!name) return NULL;
  if (!self->sorted) {
    castor_sort(self);
  } 
  result = dynar_bsearch(self->nodes, name, castornode_compare_bsearch_cstr) ;
  return result;
}

void * castor_getptr(Castor * self, const char * name) {
  CastorNode * node;
  node = castor_getnode(self, name);
  if(!node) return NULL;
  return node->data;
}


void * castor_loadex(Castor * self, const char * name, int typeno, int flags, void * extra) {
  CastorNode * load;
  void * result = castor_getnode(self, name);
  if (result) return result;
  
  load = castor_loadnode(self, name, typeno, flags, extra, TRUE);
  if(!load) return NULL;
  return load->data;
}

void * castor_load(Castor * self, const char * name, int typeno) {
  return castor_loadex(self, name, typeno, 0, NULL);
}
















