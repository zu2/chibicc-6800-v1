		.code
		.export	__add_frac_trunc
		.export	__add_half_trunc
;
;	@long = (@long + addend) & ~fmask
;	AccA: exp
;	IX:   fmask
;		__add_frac_trunc: addend = fmask	 (floorf)
;		__add_half_trunc: addend = (fmask+1)/2	 (roundf)
;
__add_half_trunc:
		ldab	2,x
		addb	#1			; addb sets C, incb does not
		stab	__work+2
		ldab	1,x
		adcb	#0
		stab	__work+1
		ldab	0,x
		adcb	#0
		lsrb
		ror	__work+1
		ror	__work+2
		stab	__work
		bra	__add_trunc
__add_frac_trunc:
		ldab	0,x
		stab	__work
		ldab	1,x
		stab	__work+1
		ldab	2,x
		stab	__work+2
__add_trunc:
		ldab	__work+2
		addb	@long+3
		stab	@long+3
		ldab	__work+1
		adcb	@long+2
		stab	@long+2
		ldab	@long+1			; set hidden bit
		orab	#$80
		adcb	__work
		stab	@long+1
		bcs	__trunc_carry
		jsr	__and_not_fmask
		bra	__trunc_build
__trunc_carry:					; the result is 2^(exp+1)
		inca
		ldab	#$80
		stab	@long+1
		clr	@long+2
		clr	@long+3
__trunc_build:
		rol	@long+1
		asl	__sign
		rora
		ror	@long+1
		staa	@long
		rts
