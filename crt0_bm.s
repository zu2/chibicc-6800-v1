;
;	crt0 for BASIC MASTER
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
;
	.export _putchar
	.export _getchar
	.export _exit
	.export _abort
	.code
start:
	sts exitsp
	lds <$08
	ldx #__bss_size
	beq call_main
	ldx #__bss
;
	ldab #<__bss
	ldaa #>__bss
	addb #<__bss_size
	adca #>__bss_size
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
	rts
;
;	minimal I/O routine
;
_putchar:
        tba
        jmp $002b
_getchar:
        ldaa #'_'
        jsr $0028
__kbhit_ret:
        tab
        clra
        rts
_kbhit:
        jsr $f00f
        tab
        bcc __kbhit_ret
        clrb
        clra
        rts
;
        .data
exitsp:
	.word 0
	.bss

