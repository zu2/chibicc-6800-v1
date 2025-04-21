;
;	ABX: IX = IX+AccB
;	ADX: IX = IX+AccAB
;
	.export __abx
	.export __adx

	.code
__abx:
	clra
__adx:
	stx	@tmp1
	addb	@tmp1+1
	adca	@tmp1
	stab	@tmp1+1
	staa	@tmp1
	ldx	@tmp1
	rts
