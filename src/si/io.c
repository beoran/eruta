#include "si.h"
#include "si_mem.h"
#include "si_macro.h"
#include "si_wire.h"
#include "si_io.h"


#include <stdio.h>


SiIoClass * siioclass_init( SiIoClass * klass, 
                            SiIoReader * read, SiIoWriter * write,
                            SiIoSelfer * close, SiIoSelfer * end_p) {
  if(!klass) return NULL;
  klass->read  = read;
  klass->write = write;
  klass->close = close;
  klass->end_p = end_p;
  return klass;
}

struct SiIo_ {
  SiIoClass * klass;
  void      * data; 
  size_t      index;
};


SiIo * siio_alloc() {
  return SI_ALLOCATE(sizeof (SiIo));
}

SiIo * siio_initgeneric(SiIo * self, void * data, SiIoClass * klass) {
  self->data  = data;
  self->klass = klass;
  self->index = 0;
  return self;
}


SiIo* siio_close(SiIo * self) {
  if(!self)       return NULL;
  if(!self->data) return NULL;
  if(self->klass && self->klass->close) self->klass->close(self->data);
  return self;
}

SiIo * siio_free(SiIo * self) {
  siio_close(self);
  return si_free(self);
}

SiIo * siio_newgeneric(void * data, SiIoClass * klass) {
  SiIo * res = siio_alloc();
  if(!res) return res;
  siio_initgeneric(res, data, klass);
  return res;
} 


int siio_read(SiIo * self, void * data, size_t size) {
  int res;
  if(!self) return -1;
  if(!data) return -1;
  if(!size) return -1;
  if(!self->klass) return -1;
  if(!self->klass->read) return -1;
  res = self->klass->read(self, data, size);
  if(res > 0) self->index += res;
  return res;
}

int siio_write(SiIo * self, void * data, size_t size) {
  int res;
  if(!self) return -1;
  if(!data) return -1;
  if(!size) return -1;
  if(!self->klass) return -1;
  if(!self->klass->write) return -1;
  res = self->klass->write(self, data, size);
  if(res > 0) self->index += res;
  return res;
}

int siio_end_p(SiIo * self) {
  if(!self) return TRUE;
  if(!self->klass) return TRUE;
  if(!self->klass->end_p) return TRUE;
  return self->klass->end_p(self);
}


FILE * siio_file(SiIo * self) {
  return (FILE *)self->data;
}

int siio_fclose(SiIo * self) {
  return fclose(siio_file(self)); 
}

int siio_fread(SiIo * self, void * data, size_t size) {
  return fread(data, size, 1, siio_file(self));
}

int siio_fwrite(SiIo * self, void * data, size_t size) {
  return fwrite(data, size, 1, siio_file(self));
}

int siio_feof(SiIo * self) {
  return feof(siio_file(self));
}


static SiIoClass SiIoFileClass = { 
  siio_fread, siio_fwrite, siio_fclose, siio_feof
};


SiIo * siio_fopen(char * filename, char * flags) {
  FILE * data = fopen(filename, flags);
  if(!data) return NULL;
  return siio_newgeneric(data, &SiIoFileClass);
}


SiWire * siio_wire(SiIo * self) {
  return (SiWire *)self->data;
}

int siio_wclose(SiIo * self) {
  if(siwire_free(siio_wire(self))) return 0;
  return -1;
}

int siio_wread(SiIo * self, void * data, size_t size) {
  siio_wire(self);
  return -1;
}















