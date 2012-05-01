
#include "eruta.h"
#include "image.h"
#include "rh.h"
#include "fifi.h"


/**
* helper macros 
*/

#define RH_WRAP(RUBY, TYPE, OBJECT)                         \
mrb_obj_value(Data_Wrap_Struct(RUBY,                        \
              RUBY->object_class, &toruby_##TYPE, OBJECT));



/*
* Helper functions for the mruby ruby interpreter
*/


void toruby_Font_free(mrb_state * state, void * ptr) {
  printf("Freeing font %p\n", ptr);
  font_free((Font*)ptr);
}

struct mrb_data_type toruby_Font_type = { "Font", toruby_Font_free };

/*
mrb_value toruby_Font(mrb_state* mrb, mrb_value sel, mrb_value rname) {
  Fonr * font = RSTRING(name);
}
*/

void toruby_Path_free(mrb_state * state, void * ptr) {
  printf("Freeing path %p\n", ptr);
  al_destroy_path((Path*)ptr);
}

struct mrb_data_type toruby_Path = { "Path", toruby_Path_free };

/* gets a data path based on a vpath. */
mrb_value tr_Path(Ruby * ruby, mrb_value self) {
  ALLEGRO_PATH * path = NULL;
  const char * vpath = NULL; size_t vlen = 0; 
  mrb_get_args(ruby, "s", &vpath, &vlen);
  printf("Making path for : %s, %d", vpath, vlen);
  path = fifi_data_vpath(vpath);
  if(!path) return mrb_nil_value();
  return mrb_obj_value(Data_Wrap_Struct(ruby, ruby->object_class, &toruby_Path, path));
}

/** Allocates and initialzes a new ruby state. */
Ruby * rh_new() {
   Ruby * self = mrb_open();
   mrb_define_method(self, self->kernel_module, 
                     "path", tr_Path, ARGS_REQ(1));
   return self;
}

/** Frees a ruby state. */
Ruby * rh_free(Ruby * self) {
  mrb_close(self);
  return NULL;
}


int rh_runfile(Ruby * self, FILE * file) {
  int n;
  struct mrb_parser_state * parser;
  parser = mrb_parse_file(self, file);
  if (!parser || !parser->tree || parser->nerr) {
    printf("Parse error!\n");
    return -1;
  }
  n = mrb_generate_code(self, parser->tree);
  mrb_pool_close(parser->pool);
  if (n >= 0) {
    mrb_run(self, mrb_proc_new(self, self->irep[n]), mrb_top_self(self));
    if (self->exc) {
      mrb_p(self, mrb_obj_value(self->exc));
    }
  }
  return n;
}

int rh_runfilename(Ruby * self, const char * filename) {
  FILE * file = fopen(filename, "rt");
  int res;
  if(!file) { 
    printf("No such ruby file: %s\n", filename);
    return -2;
  }
  res = rh_runfile(self, file);
  fclose(file);
  return res;
}


/**
* Executes a ruby file in Eruta's data/script directory.
* Returns -2 if the file ws not found.
*/
int rh_dofile(Ruby * self, const char * filename) {
  int runres;
  ALLEGRO_PATH * path = fifi_data_pathargs(filename, "script", NULL);
  if(!path) return -1;
  runres = rh_runfilename(self, PATH_CSTR(path));
  al_destroy_path(path);
  return runres;
}










