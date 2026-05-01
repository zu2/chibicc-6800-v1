	.export _puts
	.code
_puts:
	stab	@tmp1+1
	staa	@tmp1
	ldx	@tmp1
        bra     start
loop:   jsr     _putchar        ; _putchar must keep IX
        inx
start:	ldab    0,x
        bne     loop
        ldab    #$0a
        jmp     _putchar
