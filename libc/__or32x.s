;
;	long basic library or32
;
	.export __or32x

	.code
;
__or32x:
        ldab    3,x
	orab	@long+3
	stab	@long+3
        ldab    2,x
	orab	@long+2
	stab	@long+2
        ldab    1,x
	orab	@long+1
	stab	@long+1
        ldab    0,x
	orab	@long
	stab	@long
        rts
