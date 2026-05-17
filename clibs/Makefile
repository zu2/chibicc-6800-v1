#
#
#
all: clibs.a

OBJ =	qsort.o printf.o assert.o malloc.o

.s.o:
	as6800 $<

.c.o:	../chibicc
	../chibicc -O2 -c $<
	../chibicc -O2 -S $<

clibs.a: $(OBJ)
	rm -f clibs.a
	ar qc clibs.a `lorderz80 $(OBJ) | tsort`


clean:
	rm -f *.o *.a *~ makeops clibs.a
