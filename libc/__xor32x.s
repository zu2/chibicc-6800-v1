;
;	long basic library xor32x
;
	.export __xor32x

	.code
;
__xor32x:
	ldab    3,x
	eorb	@long+3
	stab	@long+3
	ldab    2,x
	eorb	@long+2
	stab	@long+2
	ldab    1,x
	eorb	@long+1
	stab	@long+1
	ldab    0,x
	eorb	@long
	stab	@long
        rts
