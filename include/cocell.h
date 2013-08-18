#ifndef cocell_H_INCLUDED
#define cocell_H_INCLUDED

typedef union CocelValue_ CocelValue;
typedef struct Cocel_ Cocel;

typedef Cocel * CocelFunction(Cocel * env, Cocel * args);

#define COCEL_NCHARS (sizeof(double))

union CocelValue_ {
  int             integer;
  double          number;
  void  *         pointer;
  CocelFunction * args;
  char            chars[COCEL_NCHARS];
  Cocel *         cell;
};



#endif




