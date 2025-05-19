	.code
	.export	__add_msb
;
;	@long += 1.0 (or 0.5)
;	AccA: exp
;	IX:   fmsbmask
;
__add_msb:
	ldab	@long+1			; set hidden bit
	orab	#$80
	stab	@long+1
	ldab	2,x
	addb	@long+3
	stab	@long+3
	ldab	1,x
	adcb	@long+2
	stab	@long+2
	ldab	0,x
	adcb	@long+1
	stab	@long+1
	bcc	__msb_build
	lsr	@long+1
	ror	@long+2
	ror	@long+3
	inca
__msb_build:
	rol	@long+1
	asl	__sign
	rora
	ror	@long+1
	staa	@long
	rts
