#
#
#
all: libc.a

OBJ =	__mul16x16.o __div16x16s.o __div16x16.o \
	__cmp16.o __shift16.o \
	__load.o

.s.o:
	as6800 $<

.c.o:
	../chibicc -c $<

libc.a: $(OBJ)
	rm -f libc.a
	ar qc libc.a `lorderz80 $(OBJ) | tsort`

clean:
	rm -f *.o *.a *~ makeops libc.a
