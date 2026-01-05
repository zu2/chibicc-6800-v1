9019:
	chibicc -tbm -vv -O2 -o 9019.bin 9019-mandelbrot.c
	dd if=9019.bin of=tmp.bin bs=1 skip=4096
	objcopy -I binary -O srec --adjust-vma=0x1000 tmp.bin 9019.srec
	objcopy -I binary -O ihex --adjust-vma=0x1000 tmp.bin 9019.hex

9017:
	chibicc -tbm -vv -O2 -o 9017.bin 9017-asciiart.c
	dd if=9017.bin of=tmp.bin bs=1 skip=4096
	objcopy -I binary -O srec --adjust-vma=0x1000 tmp.bin 9017.srec
	objcopy -I binary -O ihex --adjust-vma=0x1000 tmp.bin 9017.hex

9301:
	chibicc -tbm -vv -O2 -o 9301.bin 9301-graphics-1.c
	dd if=9301.bin of=tmp.bin bs=1 skip=4096
	objcopy -I binary -O srec --adjust-vma=0x1000 tmp.bin 9301.srec
	objcopy -I binary -O ihex --adjust-vma=0x1000 tmp.bin 9301.hex

9302:
	chibicc -tbm -vv -O2 -o 9302.bin 9302-graphics-2.c
	dd if=9302.bin of=tmp.bin bs=1 skip=4096
	objcopy -I binary -O srec --adjust-vma=0x1000 tmp.bin 9302.srec
	objcopy -I binary -O ihex --adjust-vma=0x1000 tmp.bin 9302.hex
