/**
* This is a tiny program to wrap Ruby and the Eruta main Ruby file in a C executable.
*/

#include "ruby.h"

#ifndef ERUTA_MAIN_FILE
#define ERUTA_MAIN_FILE "main.rb"
#endif

int main(int argc, char **argv)
{
    int status;
    ruby_init();
    ruby_script(argv[0]);
    ruby_init_loadpath();
    rb_protect((VALUE (*)())rb_load_file, (VALUE) ERUTA_MAIN_FILE, &status);
    if (status == 0) {
	 int i;
	 for (i = 0; i < 3; i++)
	     if ((status = ruby_exec()) != 0) break;
    }
    return ruby_cleanup(status);
}
