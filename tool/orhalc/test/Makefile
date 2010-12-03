ASSEMBLER=as
LINKER=ld

.PHONY : run
 
run : test1
	./test1

test1 : test1.o
	${LINKER} test1.o -o test1

test1.o : test1.s 
	${ASSEMBLER} test1.s -o test1.o
