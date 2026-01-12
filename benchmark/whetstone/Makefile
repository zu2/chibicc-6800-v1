#
#
#
#OPT=-vv -O2 -DNOPTINTF -Dfloat_t=float -Ddouble_t=float -D__use_float_for_double_ -Datan=atanf -Dsin=sinf -Dcos=cosf -Dsqrt=sqrtf -Dexp=expf -Dlog=logf -I./ 
OPT=-vv -O2 -DNOPTINTF -DPRINTOUT=1 -Dfloat_t=float -Ddouble_t=float -D__use_float_for_double_ -Datan=atanf -Dsin=sinf -Dcos=cosf -Dsqrt=sqrtf -Dexp=expf -Dlog=logf -I./
#OPT=-vv -Os -DNOPTINTF -DPRINTOUT=1 -Dfloat_t=float -Ddouble_t=float -D__use_float_for_double_ -Datan=atanf -Dsin=sinf -Dcos=cosf -Dsqrt=sqrtf -Dexp=expf -Dlog=logf -I./ 

benchmark: all


all: whetstone.c my_printf.c
	chibicc ${OPT} -c whetstone.c
	chibicc ${OPT} -c my_printf.c
	chibicc ${OPT} -o whet.bin whetstone.o my_printf.o -lm
	emu6800 6800 whet.bin whet.map

setup:	whetstone.c

whetstone.c:
	wget -nc https://raw.githubusercontent.com/z88dk/z88dk/refs/heads/master/support/benchmarks/whetstone/whetstone.c

