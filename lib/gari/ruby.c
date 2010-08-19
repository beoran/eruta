#include "gari.h"

#include "SDL_mixer.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"  

/** Ruby interface for Gari (and vice versa). */

#include "ruby.h"

struct GariRuby_;

typedef struct GariRuby_ GariRuby; 

struct GariRuby_ {
  int wasinit;
};

GariRuby * gari_ruby_init(GariRuby * ruby) {
  if(!ruby)         return NULL;
  if(ruby->wasinit) return ruby;
  ruby_init();
  ruby_init_loadpath();
  ruby->wasinit = TRUE;
  return ruby;
} 


GariRuby * gari_ruby_done(GariRuby * ruby) {
  if(!ruby)           return NULL;
  if(!ruby->wasinit)  return NULL;
  ruby_finalize();
  ruby->wasinit = FALSE;
}

int gari_ruby_eval(GariRuby * ruby, char * str) {
 int result;
 rb_eval_string_safe(str, &result); 
 return result;
}

int gari_ruby_exec(GariRuby * ruby, char * filename) {
 rb_load_file(filename);
 return ruby_exec();
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




















