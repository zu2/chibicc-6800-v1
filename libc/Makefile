#
#
#
all: libc.a

OBJ =	__mul16x16.o __div16x16s.o __div16x16u.o __div16x16.o \
	__cmp16.o __shift16.o \
	__load.o \
	__long.o __mul32x32.o __div32x32.o __cmp32.o __shift32.o \
	__float.o __floatlib.o \
	__adx.o __popx.o __copy_struct.o \
	__memset.o __memcpy.o __memchr.o __memmove.o __memcmp.o \
	__strcmp.o __strncmp.o __strcpy.o __strncpy.o \
	__strlen.o __strcat.o __strchr.o __strrchr.o \
	__ctype.o \
	__itoa.o __ltoa.o \
	__struct.o \
	__atoi.o __abs.o __rand.o \
	qsort.o \
	__setjmp.o \
	__memswap.o \

.s.o:
	as6800 $<

.c.o:
	../chibicc -c $<

libc.a: $(OBJ)
	rm -f libc.a
	ar qc libc.a `lorderz80 $(OBJ) | tsort`

clean:
	rm -f *.o *.a *~ makeops libc.a
