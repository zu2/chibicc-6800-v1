#CC=gcc
#CC=clang
CFLAGS=-std=c2x -g -fno-common -Wall -Wno-switch -g3 -pedantic # --coverage

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
INCS=$(wildcard include/*.h)

TEST_SRCS=$(wildcard test/*.c)
TESTS=$(TEST_SRCS:.c=.bin)


all: dirs chibicc lib crt0.o dummyfloat.o crt0_mikbug.o crt0_bm.o crt0_jr100.o crt0_jr200.o

lib:
	(cd libc ; make)
	(cd libm ; make)
	(cd clibs ; make)

crt0.o: crt0.s
	as6800 $^

crt0_mikbug.o: crt0_mikbug.s
	as6800 $^

crt0_bm.o: crt0_bm.s
	as6800 $^

crt0_jr100.o: crt0_jr100.s
	as6800 $^

crt0_jr200.o: crt0_jr200.s
	as6800 $^

dummyfloat.o: dummyfloat.s
	as6800 $^

dirs:
	mkdir -p /opt/chibicc/bin
	mkdir -p /opt/chibicc/lib
	mkdir -p /opt/chibicc/include

install: all
	install -c chibicc /opt/chibicc/bin
	install -c -m 644 crt0.o /opt/chibicc/lib
	install -c -m 644 crt0_mikbug.o /opt/chibicc/lib
	install -c -m 644 crt0_bm.o /opt/chibicc/lib
	install -c -m 644 crt0_jr100.o /opt/chibicc/lib
	install -c -m 644 crt0_jr200.o /opt/chibicc/lib
	install -c -m 644 dummyfloat.o /opt/chibicc/lib
	install -c -m 644 libc/libc.a /opt/chibicc/lib
	install -c -m 644 libm/libm.a /opt/chibicc/lib
	install -c -m 644 clibs/clibs.a /opt/chibicc/lib
	install -c -m 644 $(INCS)  /opt/chibicc/include
	install -c -m 644 copt.rules  /opt/chibicc/lib
	install -c -m 644 copt_O2.rules  /opt/chibicc/lib
	install -c -m 644 copt_O3.rules  /opt/chibicc/lib

chibicc: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJS): chibicc.h


wtests:
	(cd wtest/ ; ./runtests)

ctests:
	(cd c-compiler/ ; ././test_cases.sh )

ztests:
	(cd ztest/ ; ./runall)

rtests:
	(cd rtest/; ./runall)

btests:
	(cd benchmark; for d in */; do $(MAKE) -C $$d; done)

test: install wtests ctests ztests rtests btests
#test: install ztests btests


# Misc.

clean:
	rm -rf chibicc tmp* $(TESTS) test/*.s test/*.exe 
	find * -type f '(' -name '*~' -o -name '*.o' ')' -exec rm {} ';'
	rm -f *.gcov *.gcda *.gcno
	(cd libc ; make clean)
	(cd libm ; make clean)
	(cd clibs ; make clean)
	(cd ztest ; make clean)

.PHONY: test clean
