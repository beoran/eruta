
LIBNAME=objc_tr
LIBFILENAME=lib${LIBNAME}.so


SOURCES=\
		sarray.c\
		selector.c\
		object_tags.c\
		objc_object.c\
		objc_class.c


OBJECTS=${SOURCES:.c=.o}

CFLAGS   +=-std=c99 -g -Wall -march=i686 -fPIC -O2 -D _XOPEN_SOURCE=500
CPPFLAGS +=-DBUILD_TESTS 
LDFLAGS  += -g -lpthread

CFLAGS+=${CPPFLAGS}

SHAREDLIBFLAG = `if [ \`uname\` = Darwin ] ; then echo -n '-dynamiclib' ; else echo -n '-shared' ; fi`

all: test example benchmark

example: example.c ${LIBFILENAME}
	${CC} example.c ${CFLAGS} ${LDFLAGS} -o example -l${LIBNAME} -L.

benchmark: benchmark.c ${LIBFILENAME}
	${CC} benchmark.c ${CFLAGS} ${LDFLAGS} ${OBJECTS} -o benchmark
	
benchmarkobjc: benchmark.m
	${CC} benchmark.m ${CFLAGS} -lobjc -o benchmarkobjc

test: test.c ${LIBFILENAME}
	${CC} test.c ${CFLAGS} ${LDFLAGS} -o test -l${LIBNAME} -L.

${LIBFILENAME}: ${OBJECTS}
	${CC} ${LDFLAGS} ${OBJECTS} ${SHAREDLIBFLAG} -o ${LIBFILENAME}

doc:
	mkdir doc

docs: doc
	cd doc && autogsdoc -MakeFrames YES ../*.h
	
clean:
	@rm -f *.o
	@rm -f test
	@rm -f test.core
	@rm -f benchmark
	@rm -f benchmarkobjc
	@rm -f ${LIBFILENAME}


benchmark.o: benchmark.c capi.h objc_object.h sarray.h selector.h \
  object_tags.h objc_class.h
example.o: example.c capi.h objc_object.h sarray.h selector.h \
  object_tags.h objc_class.h
objc_class.o: objc_class.c objc_class.h objc_object.h sarray.h selector.h \
  object_tags.h lock.h
objc_object.o: objc_object.c objc_object.h sarray.h selector.h \
  object_tags.h atomic.h
object_tags.o: object_tags.c
sarray.o: sarray.c sarray.h
selector.o: selector.c selector.h sarray.h lock.h
test.o: test.c sarray.h selector.h objc_object.h object_tags.h \
  objc_class.h
