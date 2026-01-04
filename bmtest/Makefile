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

