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
int castor_errno(Castor * self);
const char * castor_strerror(Castor * self);

Castor * castor_new();

CastorType * castor_add_type(Castor * self, int index, CastorType * type);
CastorType * castor_get_type(Castor * self, int type);
Castor * castor_sort(Castor * self);
void * castor_get(Castor * self, const char * name);
void * castor_abandon(Castor * self, const char * name);
void * castor_get_abandon(Castor * self, const char * name);
void * castor_put_ex
(Castor * self, const char * name, void * value);
void * castor_put
(Castor * self, const char * name, void * value);


void * 
castor_load_ex(Castor * self, const char * name, int typeno, int flags, void * extra);
void * castor_load(Castor * self, const char * name, int typeno);
int castor_remove(Castor * self, const char * name);

enum StorageErrno {
  STORAGE_OK     = 0, 
  STORAGE_ENOENT = 1,
  STORAGE_EMEM   = 2, 
};


int storage_init(); 
int storage_done();
void * storage_get(const char * name);
void * storage_load(const char * name, int typeno);
void * storage_load_ex(const char * name, int typeno, int flags, void * extra, int * err); 
int storage_errno();
const char * storage_errstr(); 







#endif




