CC:=chibicc
START:=0x1000
OPT:=-tbm -O2 -C$(START)
all: bm-graphics.o 9017 9019 9301 9302

.c.o:
	$(CC) $(OPT) -c $<

9017:
	chibicc $(OPT) -o 9017.bin 9017-asciiart.c
	dd if=9017.bin of=tmp.bin bs=1 skip=`printf "%d\n" $(START)`
	objcopy -I binary -O srec --adjust-vma=$(START) tmp.bin 9017.mot
	objcopy -I binary -O ihex --adjust-vma=$(START) tmp.bin 9017.hex

9019:
	chibicc $(OPT) -o 9019.bin 9019-mandelbrot.c bm-graphics.o
	dd if=9019.bin of=tmp.bin bs=1 skip=`printf "%d\n" $(START)`
	objcopy -I binary -O srec --adjust-vma=$(START) tmp.bin 9019.mot
	objcopy -I binary -O ihex --adjust-vma=$(START) tmp.bin 9019.hex

9301:
	chibicc $(OPT) -o 9301.bin 9301-graphics-1.c bm-graphics.o
	dd if=9301.bin of=tmp.bin bs=1 skip=`printf "%d\n" $(START)`
	objcopy -I binary -O srec --adjust-vma=$(START) tmp.bin 9301.mot
	objcopy -I binary -O ihex --adjust-vma=$(START) tmp.bin 9301.hex

9302:
	chibicc $(OPT) -o 9302.bin 9302-graphics-2.c bm-graphics.o
	dd if=9302.bin of=tmp.bin bs=1 skip=`printf "%d\n" $(START)`
	objcopy -I binary -O srec --adjust-vma=$(START) tmp.bin 9302.mot
	objcopy -I binary -O ihex --adjust-vma=$(START) tmp.bin 9302.hex
