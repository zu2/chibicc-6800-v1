;
;   Push AccA,B bytes from 0,x, from the last byte to the first,
;   so the first byte lands at the lowest address.
;
	.export __push_struct_x
	.code
__push_struct_x:
	stab	@tmp2+1
	staa	@tmp2
	stx	@tmp1
	addb	@tmp1+1
	adca	@tmp1
	stab	@tmp1+1
	staa	@tmp1
	ldx	@tmp1
	ldab	@tmp2+1
	ldaa	@tmp2
	cmpb	#1		; if b!=0, a++
	sbca	#0
	inca
	staa	@tmp2
;
	pula
	staa	@tmp1
	pula
	staa	@tmp1+1
__loop:
	dex
	ldaa	0,x
	psha
	decb
	bne	__loop
	dec	@tmp2
	bne	__loop
	ldx	@tmp1
	jmp	0,x
