all:
	chibicc -tbm -vv -DBM=1 9017-asciiart.c
	dd if=a.bin of=temp.bin bs=1 skip=4096
	objcopy -I binary -O srec --adjust-vma=0x1000 temp.bin a.srec

