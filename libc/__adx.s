;
;	ABX: IX = IX+AccB
;	ADX: IX = IX+AccAB
;	TFR D,X: IX = AccDB
;
	.export __abx
	.export __adx
	.export __tfr_dx

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
;
__tfr_dx:
	stab	@tmp1+1
	staa	@tmp1
	ldx	@tmp1
	rts
;
