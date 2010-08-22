#include "gari.h"

#include "SDL_mixer.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"  

/** Ruby interface for Gari (and vice versa). 
* It seems that it's difficult to embed ruby 1.9.x into C;  
* in stead I'll do it like this: 
*
* 1) Use the standard C foreign function interface for Ruby.
* 2) Write a small program that wraps the Ruby interpreter 
* wich loads the first script,,.
* 3) Which in turn will load the Gari library.
*/

#include "ruby.h"
#include "ruby/intern.h"

struct GariRuby_;

/*typedef struct GariRuby_ GariRuby; */

struct GariRuby_ {
  int wasinit;
  int error;
  int argc;
  char  *argv[32];
};

GariRuby * gari_ruby_init(GariRuby * ruby) {
  if(!ruby)         return NULL;
  if(ruby->wasinit) return ruby;
  // ruby->argc    = 0;
  // ruby->argv[0] = "gari";
  
  // ruby_sysinit(&ruby->argc, &ruby->argv);
  
  
  ruby_init();
  ruby_init_loadpath();
  ruby->wasinit = TRUE;
  
  
  return ruby;
} 

GariRuby * gari_ruby_new() {
  GariRuby * ruby;
  ruby = GARI_ALLOCATE(GariRuby);
  if(!ruby) return NULL;  
  if(!gari_ruby_init(ruby)) {
    GARI_FREE(ruby);
    return NULL;
  }
  return ruby;  
}



GariRuby * gari_ruby_showerror(GariRuby * ruby) {
  VALUE lasterr, m;
  if (!ruby)   	  	{ return NULL; }
  if (!ruby->error)	{ return ruby; }
  lasterr = rb_gv_get("$!");
  /* message */
  m = rb_obj_as_string(lasterr);
  printf("Ruby error: %s\n", RSTRING_PTR(m));
}


GariRuby * gari_ruby_done(GariRuby * ruby) {
  if(!ruby)           return NULL;
  if(!ruby->wasinit)  return NULL;
  ruby_finalize();
  ruby->wasinit = FALSE;
}

void gari_ruby_free(GariRuby * ruby) {
  if(!ruby) return NULL;
  gari_ruby_done(ruby);
}


GariRuby * gari_ruby_eval(GariRuby * ruby, char * str) {
 int result;
 rb_eval_string(str); 
 // rb_eval_string_protect(str, &result); 
 // ruby->error = result; 
 return ruby;
}

int gari_ruby_exec(GariRuby * ruby, char * filename) {
 rb_load_file(filename);
 // return ruby_exec();
}


/*
RUBY_GLOBAL_SETUP
 
int main(int argc, char *argv[])
{
    ruby_sysinit(&argc, &argv);
    {
        RUBY_INIT_STACK;
        ruby_init();
        return ruby_run_node(ruby_options(argc, argv));
    }
}
*/



Init_gari() {
  VALUE GariModule;
  GariModule = rb_define_module("Gari");
}
















