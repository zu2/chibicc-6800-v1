;
;	@long = TOS*@long
;
;	it's very slow
;
	.export __mul32tos
	.code
__mul32tos:
	ldab #32
	stab @tmp3
	tsx
	; 0-1,x return address
	; 2-5,x TOS
	clra
	clrb
	clr	@tmp2+1
	clr	@tmp2
	bra	first
	; Rotate through the number
nextbit:
	aslb
	rola
	rol	@tmp2+1
	rol	@tmp2
first:	rol	@long+3
	rol	@long+2
	rol	@long+1
	rol	@long
	bcc noadd
	addb	5,x
	adca	4,x
	psha
	ldaa	3,x
	adca	@tmp2+1
	staa	@tmp2+1
	ldaa	2,x
	adca	@tmp2
	staa	@tmp2
	pula
noadd:
	dec @tmp3
	bne nextbit
	; For a 32x32 to 64bit just store 2-5,x into sreg
	stab	@long+3
	staa	@long+2
	ldab	@tmp2+1
	stab	@long+1
	ldab	@tmp2
	stab	@long
	ldx	0,x
	ins
	ins
	ins
	ins
	jmp	0,x
