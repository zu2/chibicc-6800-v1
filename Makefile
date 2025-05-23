CFLAGS=-std=c11 -g -fno-common -Wall -Wno-switch -g3 -std=c99

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
INCS=$(wildcard include/*.h)

TEST_SRCS=$(wildcard test/*.c)
TESTS=$(TEST_SRCS:.c=.bin)


all: chibicc lib crt0.o

lib:
	(cd libc ; make)
	(cd libm ; make)

crt0.o: crt0.s
	as6800 $^

install:
	mkdir -p /opt/chibicc/bin
	mkdir -p /opt/chibicc/lib
	mkdir -p /opt/chibicc/include
	install -c chibicc /opt/chibicc/bin
	install -c crt0.o /opt/chibicc/lib
	install -c libc/libc.a /opt/chibicc/lib
	install -c libm/libm.a /opt/chibicc/lib
	install -c copt.rules  /opt/chibicc/lib
	install -c $(INCS)  /opt/chibicc/include
	

# Stage 1

chibicc: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJS): chibicc.h

test/%.bin: chibicc test/%.c
	(cd test; ../chibicc -vvv -Ddouble=float -Iinclude -I.. -I. -o $@ $*.c  ../benchmark/pi/my_printf.c -xc common  )

test: $(TESTS)
	(cd test; for i in $^; do echo $$i; emu6800 6800 ../test/$$i.bin ../test/$$i.map || exit 1; echo; done
	test/driver.sh ./chibicc

test-all: test test-stage2

# Stage 2

stage2/chibicc: $(OBJS:%=stage2/%)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

stage2/%.o: chibicc %.c
	mkdir -p stage2/test
	./chibicc -c -o $(@D)/$*.o $*.c

stage2/test/%.exe: stage2/chibicc test/%.c
	mkdir -p stage2/test
	./stage2/chibicc -Iinclude -Itest -c -o stage2/test/$*.o test/$*.c
	$(CC) -o $@ stage2/test/$*.o -xc test/common

test-stage2: $(TESTS:test/%=stage2/test/%)
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done
	test/driver.sh ./stage2/chibicc

# Misc.

clean:
	rm -rf chibicc tmp* $(TESTS) test/*.s test/*.exe stage2
	find * -type f '(' -name '*~' -o -name '*.o' ')' -exec rm {} ';'
	(cd libc ; make clean)
	(cd libm ; make clean)
	(cd ztest ; make clean)

.PHONY: test clean test-stage2
