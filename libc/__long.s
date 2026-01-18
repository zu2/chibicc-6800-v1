;
;	long basic library
;
	.export	__32to8
	.export	__32to16
	.export	__u8to32
	.export	__u16to32
	.export	__s8to32
	.export	__s16to32
	.export	__iszero32

	.code
;
;	convert value
;
__32to8:
	ldab	@long+3
	rts
__32to16:
	ldab	@long+3
	ldaa	@long+2
	rts
__u8to32:
	stab	@long+3
	clra
	staa	@long+2
	staa	@long+1
	staa	@long
	rts
__u16to32:
	stab	@long+3
	staa	@long+2
	clra
	staa	@long+1
	staa	@long
	rts
__s8to32:
	clra
	stab	@long+3
	bpl	__s8to32_1
	deca
__s8to32_1:
	staa	@long+2
	staa	@long+1
	staa	@long
	rts
__s16to32:
	ldx	#0
	stab	@long+3
	staa	@long+2
	bpl	__s16to32_1
	dex
__s16to32_1:
	stx	@long
	rts
;
;	if (@long == zero)
;		Z=1, AccB==0
;       else
;               Z=0, AccB!=0
;
__iszero32:
        ldab    @long+3
        orab    @long+2
        orab    @long+1
        orab    @long
	rts

 
