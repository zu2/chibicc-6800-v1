;
;   AccAB = (AccAB)
;
	.export __load8u
	.export __load8s
	.export __load16
	.code
__load8u:
	stab	@tmp1+1
	staa	@tmp1
	ldx	@tmp1
	ldab 0,x
	clra
	rts
__load8s:
	stab	@tmp1+1
	staa	@tmp1
	ldx	@tmp1
	clra
	ldab 0,x
	bpl __load8s2
	coma
__load8s2:
	rts
__load16:
	stab	@tmp1+1
	staa	@tmp1
	ldx	@tmp1
	ldab 1,x
	ldaa 0,x
	rts
