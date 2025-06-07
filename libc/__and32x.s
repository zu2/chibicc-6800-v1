;
;	long basic library and32
;
	.export __and32x

	.code
;
;
;
__and32x:
	ldab    3,x
	andb	@long+3
	stab	@long+3
	ldab    2,x
	andb	@long+2
	stab	@long+2
	ldab    1,x
	andb	@long+1
	stab	@long+1
	ldab    0,x
	andb	@long
	stab	@long
        rts
