
/*
Compile with: 
gcc -idirafter/usr/local/include/ruby-1.9.1 \
-idirafter/usr/local/include/ruby-1.9.1/i686-linux \
-c

gcc -idirafter/usr/local/include/ruby-1.9.1 
-idirafter/usr/local/include/ruby-1.9.1/i686-linux -L /usr/local/lib -lruby 
-lm eruta.c -o eruta ; ./eruta

*/

// #define RUBY_LIB_PREFIX

#include <ruby.h>
#include <ruby/encoding.h>

VALUE eruta_raw_require(VALUE arg) {
    return rb_require("./local.rb");
}

VALUE eruta_require() {
    int error;
    VALUE result = rb_protect(eruta_raw_require, 0, &error);
    if(error) return Qnil;
    //    throw;
    return result;
}


VALUE shelter;

int eruta_ruby_init() {
  VALUE options = Qnil, sublet = Qnil;

  void Init_prelude(void);

  RUBY_INIT_STACK;
  ruby_init();
  ruby_init_loadpath();
  ruby_script("eruta");

#ifdef HAVE_RB_ENC_SET_DEFAULT_INTERNAL
  {
    VALUE encoding = Qnil;

    /* FIXME: Fix for ruby 1.9.2p429 borrowed from ruby? */
    (void)rb_filesystem_encoding();

    /* Set encoding */
    encoding = rb_enc_from_encoding(rb_locale_encoding());
    rb_enc_set_default_external(encoding);
  }
#endif /* HAVE_RB_ENC_SET_DEFAULT_INTERNAL */

  /* FIXME: Fake ruby_init_gems(Qtrue) */
  rb_define_module("Gem");
  Init_prelude();
  
  
  /* Bypassing garbage collection. Why? */
  /* shelter = rb_ary_new();
  rb_gc_register_address(&shelter);
  */
  
}

int eruta_ruby_quit() {
  ruby_finalize();
}




int main(void) {
  int res;
  eruta_ruby_init();
  rb_eval_string_protect("$: < '.'", &res);
  rb_eval_string_protect("$: < '.' ; require 'local.rb'", &res);
  rb_eval_string_protect("puts 'Hello 1 µµ!'", &res);
  rb_eval_string_protect("exit(1)", &res);
  rb_eval_string_protect("puts 'Hello 2!'", &res);
  printf("Result: %d\n", res);
  rb_eval_string_protect("frozcicks", &res);
  printf("Result: %d\n", res);
  rb_eval_string_protect("puts 'Hello 3!'", &res);
  
  eruta_ruby_quit();
  return 0;
}



