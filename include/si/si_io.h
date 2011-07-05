#ifndef SI_IO_H
#define SI_IO_H

/** Generic input and output functionality. Will elable read from and 
write to files, strings, etc.  */

struct SiIo_;
typedef struct SiIo_ SiIo;


typedef int (SiIoReader)(SiIo * self, void * data, size_t size);
typedef int (SiIoWriter)(SiIo * self, void * data, size_t size);
typedef int (SiIoCloser)(SiIo * self);
typedef int (SiIoSelfer)(SiIo * self);

struct SiIoClass_ {
  SiIoReader * read;
  SiIoWriter * write;
  SiIoSelfer * close;
  SiIoSelfer * end_p;
};

typedef struct SiIoClass_ SiIoClass;







#endif




