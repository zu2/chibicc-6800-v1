OPT=-vv -O -DPRINTF=1 -Ddouble_t=float "-Dscanf(x,y)={y=20000;}"

benchmark: all


all: dhry_1.c dhry_2.c dhry.h my_printf.c
	chibicc ${OPT} -c dhry_1.c
	chibicc ${OPT} -c dhry_2.c
	chibicc ${OPT} -c my_printf.c
	chibicc ${OPT} -o dhry.bin dhry_1.o dhry_2.o my_printf.o
	emu6800 6800 dhry.bin dhry.map

setup:	dhry_1.c dhry_2.c dhry.h

dhry_1.c:
	wget -nc https://raw.githubusercontent.com/z88dk/z88dk/refs/heads/master/libsrc/_DEVELOPMENT/EXAMPLES/benchmarks/dhrystone21/dhry_1.c

dhry_2.c:
	wget -nc https://raw.githubusercontent.com/z88dk/z88dk/refs/heads/master/libsrc/_DEVELOPMENT/EXAMPLES/benchmarks/dhrystone21/dhry_2.c


dhry.h:
	wget -nc https://raw.githubusercontent.com/z88dk/z88dk/refs/heads/master/libsrc/_DEVELOPMENT/EXAMPLES/benchmarks/dhrystone21/dhry.h
