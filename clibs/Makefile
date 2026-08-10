#
#
#
all: clibs.a

OBJ =	qsort.o printf.o _printf_float.o assert.o malloc.o

.s.o:
	as6800 $<

.c.o:	../chibicc
	../chibicc -O2 -B../ -I../include -c $<
	../chibicc -O2 -B../ -I../include -S $<

clibs.a: $(OBJ)
	rm -f clibs.a
	ar qc clibs.a `lorder6800 $(OBJ) | tsort`


clean:
	rm -f *.o *.a *~ makeops clibs.a
