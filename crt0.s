;
;	A dummy minimal crt0.s for now
;
	.zp
	.export bp
	.export tmp1
	.export tmp2
	.export tmp3
	.export tmp4
	.export long
bp:	.word	0
tmp1:	.word	0
tmp2:	.word	0
tmp3:	.word	0
tmp4:	.word	0
long:	.word	0
	.word	0
	.export	rdi
rdi:	.word	0
;
	.export _putchar
	.export _getchar
	.export _print
	.export _cpu_counter
	.code
start:
	sts exitsp
	lds #$7fff
	ldx #__bss_size
	beq call_main
	ldx #__bss
;
	ldab #<__bss
	ldaa #>__bss
	addb #>__bss_size
	adca #<__bss_size
	stab @tmp1+1
	staa @tmp1
	clrb
bss_clear:
	stab 0,x
	inx
	cpx @tmp1
	bne bss_clear
;
call_main:
	clra
	clrb
	pshb
	psha
	jsr _main
	ins
	ins
;
;	No atexit support yet
;
_exit:
_abort:
doexit:
	lds exitsp
	stab $feff
	rts
;
;	minimal I/O routine
;
_putchar:
	stab $fefe;
	rts
_getchar:
	ldab #'_'
	stab $fefe
	rts
_print:
	staa $fefc;
	stab $fefd;
	rts
_cpu_counter:
	staa $fefb;
	rts
;
	.bss
exitsp:
	.word 0

