#
#  Math library
#
all: libm.a

OBJ =	__float.o __floatlib.o __floatconst.o \
	__floorf.o __ceilf.o __truncf.o __roundf.o \
	__fmaxf.o __fmodf.o __modff.o \
	__frexpf.o __ldexpf.o \
	__sqrtf.o \
	atanf.o sincosf.o tanf.o  \
	sinhf.o coshf.o tanhf.o \
	expf.o logf.o powf.o log1pf.o \
	__copysignf.o __nearbyintf.o __nextafterf.o \
	__fracmask.o __get_lexp_sign.o __add_msb.o

.s.o:
	as6800 $<

.c.o:
	../chibicc -O2 -c $<

libm.a: $(OBJ)
	rm -f libm.a
	ar qc libm.a `lorderz80 $(OBJ) | tsort`

clean:
	rm -f *.o *.a *~ makeops libm.a
