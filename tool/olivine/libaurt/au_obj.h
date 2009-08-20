#ifndef OI_OBJ_H
#define OI_OBJ_H

#include "oi.h"
#include "oi_mem.h"  
  
struct oi_selectors_;
struct oi_data_     ;
struct oi_obj_      ;
enum OI_EXCEPTION_  ;

typedef struct oi_selectors_   oi_selectors;
typedef struct oi_data_        oi_data;
typedef struct oi_obj_         oi_obj;
typedef enum OI_FLAGS_        OI_FLAGS;  

enum OI_FLAGS_ { OK, ERROR, FATAL };

struct oi_obj_ {
  oi_mem          memory;
  OI_FLAGS        flags;
  oi_selectors  * selectors;
  oi_data       * data;
};

OI_API(oi_obj *) oi_obj_sendres(oi_obj * self, int argc     , oi_obj *argv[]);
OI_API(oi_obj *) oi_data_get(oi_obj * self, oi_obj * key);
OI_API(oi_obj *) oi_data_set(oi_obj * self, oi_obj * key , oi_obj * value);
OI_API(void *)   oi_obj_unwrap(oi_obj * self);
OI_API(OI_FLAGS) oi_obj_send(oi_obj * result, oi_obj * self, int argc, oi_obj *argv[]);





#endif
