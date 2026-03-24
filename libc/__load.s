;
;   AccAB = (AccAB)
;
	.export __load8
	.export __load16
	.code
__load8:
	stab	@tmp1+1
	staa	@tmp1
	ldx	@tmp1
	ldab 0,x
	rts
__load16:
	stab	@tmp1+1
	staa	@tmp1
	ldx	@tmp1
	ldab 1,x
	ldaa 0,x
	rts
