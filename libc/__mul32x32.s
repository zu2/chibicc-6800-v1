;
;	@long = TOS*@long
;
;	it's very slow
;
	.export __mul32tos
	.code
shift8:	ldab	@long+1
	stab	@long
	ldab	@long+2
	stab	@long+1
	ldab	@long+3
	stab	@long+2
	clr	long+3
	ldab	@tmp3
	subb	#8
	stab	@tmp3
	rts
;
__mul32tos:
	ldab #32
	stab @tmp3
	tsx
	; 0-1,x return address
	; 2-5,x TOS
	clra
	clrb
	stab	@tmp2+1
	staa	@tmp2
;
	tst	@long
	bne	first
;
	tst	@long+1
	bne	skip8	; @long[0] == 0, skip 8bit
;
	tst	@long+2
	bne	skip16	; @long[0..1] ==0, skip 16bit
;
	tst	@long+3
	bne	skip24	; @long[0..2] ==0, skip 24bit
;	; all 0! return 0
	bra	ret
;
skip24:	bsr	shift8
skip16:	bsr	shift8
skip8:	bsr	shift8
	clrb
	bra	first
;
;	Rotate through the number
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
	; The result of 32bit*32bit is 64bit. It is stored in 2-5,x and @long.
	stab	@long+3
	staa	@long+2
	ldab	@tmp2+1
	stab	@long+1
	ldab	@tmp2
	stab	@long
ret:
	ldx	0,x
	ins
	ins
	ins
	ins
	ins
	ins
	jmp	0,x
