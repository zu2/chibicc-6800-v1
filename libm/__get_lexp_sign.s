	.data
;
	.code
	.export	__get_lexp_sign
;
__get_lexp_sign:
	ldaa	@long+1
	ldab	@long
	stab	__sign
	asla
	rolb
	stab	__lexp
	rts
