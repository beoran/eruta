#ifndef RH_PROTO_H
#define RH_PROTO_H
/*
This file was autogenerated from src/rh.c
by bin/genproto
Please do not hand edit.
*/

/**
* helper macros 
*/

#define RH_WRAP(RUBY, TYPE, OBJECT)                         \
mrb_obj_value(Data_Wrap_Struct(RUBY,                        \
              RUBY->object_class, &toruby_##TYPE, OBJECT));



/*
* RH contains helper functions for the mruby ruby interpreter.
*/


void toruby_Font_free(mrb_state * state, void * ptr);

/** Allocates and initialzes a new ruby state. */
Ruby * rh_new();

/** Frees a ruby state. */
Ruby * rh_free(Ruby * self);

/** Returns an mrb_value that contains the value of object.inspect. 
* m
*/
mrb_value rh_inspect(mrb_state *mrb, mrb_value obj);

/**
* Executes a ruby file in Eruta's data/script directory.
* Returns -2 if the file was not found.
*/
int rh_dofile(Ruby * self, const char * filename);

/** Runs a ruby string in the console, logging results and errors back to it.
*/
int rh_dostring_console(Console * console, char * command, void * extra);

#endif // RH_PROTO_H

