#
#
#
all: libc.a

OBJ =	__mul16x16.o __div16x16s.o __div16x16u.o __div16x16.o __mul8x8.o \
	__mul100.o \
	__cmp8.o __cmp16.o __shift16.o \
	__load.o __load32.o __pop32.o \
	__long.o __mul32x32.o __div32x32.o __cmp32.o __shift32.o \
	__add32.o __sub32.o __neg32.o \
	__and32.o __or32.o __xor32.o __not32.o \
	__and32x.o __or32x.o __xor32x.o \
	__adx.o __popx.o __copy_struct.o __ins.o \
	__memset.o __memcpy.o __memchr.o __memmove.o __memcmp.o \
	__strcmp.o __strncmp.o __strcpy.o __strncpy.o \
	__strlen.o __strcat.o __strchr.o __strrchr.o __strtok.o \
	__ctype.o \
	__itoa.o __ltoa.o \
	__struct.o \
	__atoi.o __abs.o __rand.o \
	__builtin_clz.o \
	qsort.o \
	__setjmp.o \
	__memswap.o \
	__func.o __jmp_d.o \
	__popx_stxbp.o __pulbp_rts.o

.s.o:
	as6800 $<

.c.o:
	../chibicc -c $<

libc.a: $(OBJ)
	rm -f libc.a
	ar qc libc.a `lorderz80 $(OBJ) | tsort`

clean:
	rm -f *.o *.a *~ makeops libc.a
