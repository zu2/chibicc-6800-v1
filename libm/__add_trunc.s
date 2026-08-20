		.code
		.export	__add_frac_trunc
		.export	__add_half_trunc
		.export	__add_even_trunc
;
;	@long = (@long + addend) & ~fmask
;	AccA: exp
;	IX:   fmask
;		__add_frac_trunc: addend = fmask	 (floorf)
;		__add_half_trunc: addend = (fmask+1)/2	 (roundf)
;		__add_even_trunc: addend = fmask/2 + integer LSB (nearbyintf)
;
__add_half_trunc:
		ldab	2,x
		addb	#1			; addb sets C, incb does not
		stab	__fp_work+2
		ldab	1,x
		adcb	#0
		stab	__fp_work+1
		ldab	0,x
		adcb	#0
		lsrb
		ror	__fp_work+1
		ror	__fp_work+2
		stab	__fp_work
		bra	__add_trunc
__add_even_trunc:
		ldab	@long+1			; the LSB test needs the hidden bit
		orab	#$80
		stab	@long+1
		ldab	0,x			; __fp_work = fmask/2
		lsrb
		stab	__fp_work
		ldab	1,x
		rorb
		stab	__fp_work+1
		ldab	2,x
		rorb
		stab	__fp_work+2
		ldab	2,x			; (fmask+1) picks the integer LSB
		addb	#1
		andb	@long+3			; ANDB keeps C
		bne	__add_odd
		ldab	1,x
		adcb	#0
		andb	@long+2
		bne	__add_odd
		ldab	0,x
		adcb	#0
		andb	@long+1
		beq	__add_trunc
__add_odd:
		sec				; a tie rounds up to the even integer
		ldab	__fp_work+2
		adcb	@long+3
		bra	__trunc_lo
;
__add_frac_trunc:
		ldab	0,x
		stab	__fp_work
		ldab	1,x
		stab	__fp_work+1
		ldab	2,x
		stab	__fp_work+2
__add_trunc:
		ldab	__fp_work+2
		addb	@long+3
__trunc_lo:
		stab	@long+3
		ldab	__fp_work+1
		adcb	@long+2
		stab	@long+2
		ldab	@long+1			; set hidden bit
		orab	#$80
		adcb	__fp_work
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
