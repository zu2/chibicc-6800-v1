#
#
#
all: clibs.a

OBJ =	qsort.o printf.o assert.o

.s.o:
	as6800 $<

.c.o:
	../chibicc -O2 -c $<

clibs.a: $(OBJ)
	rm -f clibs.a
	ar qc clibs.a `lorderz80 $(OBJ) | tsort`

clean:
	rm -f *.o *.a *~ makeops clibs.a
