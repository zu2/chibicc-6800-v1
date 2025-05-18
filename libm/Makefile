#
#  Math library
#
all: libm.a

OBJ =	__floorf.o __fmaxf.o __fmodf.o __modff.o __roundf.o \
	__sqrtf.o \
	atanf.o cosf.o sinf.o tanf.o  \
	expf.o logf.o powf.o log1pf.o \
	__copysignf.o

.s.o:
	as6800 $<

.c.o:
	../chibicc -c $<

libm.a: $(OBJ)
	rm -f libm.a
	ar qc libm.a `lorderz80 $(OBJ) | tsort`

clean:
	rm -f *.o *.a *~ makeops libm.a
