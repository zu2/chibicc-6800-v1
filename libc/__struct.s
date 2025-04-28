;
;   Push AccA,B bytes from 0,x.
;
	.export __push_struct_x
	.code
__push_struct_x:
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
	ldaa	0,x
	psha
	inx
	decb
	bne	__loop
	dec	@tmp2
	bne	__loop
	ldx	@tmp1
	jmp	0,x
