;
;	MC6800 floating point arithmetic library
;
;	Copyright (c) 2025 by ZUKERAN, shin
;	Released under the MIT license
;
;	https://github.com/zu2/chibicc-6800-v1?tab=License-1-ov-file#readme
;
;	Note: This program was created for testing chibicc-6800-v1, 
;	and does not pay attention to speed, accuracy, or exception handling.
;
;	TODO: NaN, Inf
;
	.export	_modff
	.data
__exp:	.byte	0
__nexp:	.byte	0	; new exp
__mask:	.word	0
	.word	0
__frac:	.word	0
	.word	0
;
;	@long = modff(@long,&int_part)
;		parameter passed by @long
;
;	stack:
;	+2 &int_part
;
	.code
_modff:
	jsr	__f32isNaNorInf	; Handle NaN/Inf cases
	tsx
	bcs	__modff_NaN
	beq	__modff_ret_long ; @long is Inf,return @long and 0.0
;
__modff_10:
	ldx	2,x		; get int_part
	ldab	@long
	ldaa	@long+1
	asla
	rolb
	stab	__exp
	subb	#127		; exp < 0 ?  ( x<1.0 ?)
	bcc	__modff_mask
;
	ldab	@long		; *int_part = 0.0
	andb	#$80
	stab	0,x
	clrb
	stab	1,x
	stab	2,x
	stab	3,x
	rts			; return @long
;
__modff_NaN:			; @long is NaN
	ldx	2,x
	jmp	__store32x	; (0-3,x) = @long and return
;
__modff_mask:
	subb	#23
	bcs	__modff_make_mask
;
__modff_ret_long:		; int_part = @long, return +0.0 or -0.0
	ldx	2,x		; get int_part
	jsr	__store32x	; (0-3,x) = @long
;
__modff_ret_zero:
	ldab	@long
	stab	__sign
	jmp	__f32zeros	; return +0.0 or -0.0
;
;	Create a mask to separate the integer and fractional parts
;	  of a floating-point number
;
;	mask = (1u << (23 - exp)) - 1
;
__modff_make_mask:
	negb			; B = 23 - exp,  1 < B < 23
;
	ldx	#__mask
	bsr	__mask_make	; __mask = (1U<<AccB)-1
;
	ldab	__mask+3
	andb	@long+3
	stab	__frac+3
	ldab	__mask+2
	andb	@long+2
	stab	__frac+2
	ldab	__mask+1
	andb	@long+1
	stab	__frac+1
;
	bne	__modff_mask_done	; @long & __mask == 0 ?
	tsta
	bne     __modff_mask_done 
	tst	__frac+3
	
	beq	__modff_ret_zero	; int_part = @long, return ±0.0
;
;	Extract integer part by masking out the fraction bits
;
__modff_mask_done:
	tsx
	ldx	2,x
	ldab	__mask+3
	comb
	andb	@long+3
	stab	3,x
	ldab	__mask+2
	comb
	andb	@long+2
	stab	2,x
	ldab	__mask+1
	comb
	andb	@long+1
	stab	1,x
	ldab	@long
	stab	0,x		; __mask always $00
;
;	Normalize the fractional part using shift loop
;
	clra			; shift count
__modff_norm_loop:
	deca			; Use a negative value to simplify later calc.
	asl	__frac+3
	rol	__frac+2
	rol	__frac+1
	bpl	__modff_norm_loop
;
	adda	__exp		; biased __exp - shift, A has new exp
	bcs	__modff_normal
;
;	Handle denormalized numbers
;
__modff_denormal_loop:
	lsr	__frac+1
	ror	__frac+2
	ror	__frac+3
	inca
	bne	__modff_denormal_loop
;
;	Now, AccA is new exp (if denormal, AccA==0)
;
;	Reconstruct fractional float
;
__modff_normal:
	ldab	__frac+3
	stab	@long+3
	ldab	__frac+2
	stab	@long+2
	ldab	__frac+1
	aslb
	asl	@long
	lsra
	rorb
	stab	@long+1
	staa	@long;
	rts
;
;	make mask (0-3,x) == (1U<<AccB)-1
;		1 <= B <= 32
;		destroy A, B, IX
;
__mask_make:
	clra
	staa	3,x
	staa	2,x
	staa	1,x
	staa	0,x
	inx
	inx
	inx
	andb	#$1F
	bra	__mask_make_start
__mask_skip_loop:
	com	0,x
	dex
__mask_make_start:
	subb	#8
	bcc	__mask_skip_loop
__mask_make_done:	; here, -8 <= AccB <= -1 (loop count 1-8)
	addb	#8
	beq	__make_mask_end	; here, 0 < AccB < 7
	clra
__make_mask_loop:		; 10cyc/loop
	sec
	rola
	decb
	bne	__make_mask_loop
	staa	0,x
__make_mask_end:
	rts



	




















