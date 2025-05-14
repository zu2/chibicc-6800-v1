#
#  Math library
#
all: libm.a

OBJ =	__floor.o __fmaxf.o __fmodf.o __modff.o __round.o \
	__sqrtf.o \
	atanf.o cosf.o sinf.o tanf.o  \
	expf.o logf.o powf.o log1pf.o

.s.o:
	as6800 $<

.c.o:
	../chibicc -c $<

libc.a: $(OBJ)
	rm -f libm.a
	ar qc libm.a `lorderz80 $(OBJ) | tsort`

clean:
	rm -f *.o *.a *~ makeops libm.a
