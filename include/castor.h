#ifndef castor_H_INCLUDED
#define castor_H_INCLUDED

typedef struct Castor_ Castor;

enum CastorType {
  CASTOR_NONE           = 0,
  CASTOR_IMAGE          = 1,
  CASTOR_FONT           = 2,
  CASTOR_SOUND          = 3,
  CASTOR_MUSIC          = 4,
  CASTOR_XML            = 5,
  CASTOR_LASTTYPE       = 6,
};


typedef struct CastorType_ CastorType;
typedef struct CastorNode_ CastorNode;

/* Load and unload callback functionality. */
typedef void * CastorLoader(const char * name, int type, int flags, void * extra);
typedef void * CastorUnloader(void * data, void * extra);

struct CastorType_ { 
  int index;
  CastorLoader   * loader;
  CastorUnloader * unloader;
};


Castor * castor_done(Castor * self) ;
Castor * castor_init(Castor * self);
Castor * castor_free(Castor * self);
Castor * castor_new();

CastorType * castor_addtype(Castor * self, int index, CastorType * type);
CastorType * castor_gettype(Castor * self, int type);
Castor * castor_sort(Castor * self);
void * castor_getptr(Castor * self, const char * name);
void * castor_loadex
(Castor * self, const char * name, int typeno, int flags, void * extra);
void * castor_load(Castor * self, const char * name, int typeno);






#endif




