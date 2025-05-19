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
__nexp:	.byte	0	; new exp
__maskp:.word	0
__mask:	.word	0
	.word	0
__frac:	.word	0	; frac_part
	.word	0
__save:	.word	0
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
	jsr	__f32isNaNorInf		; Handle NaN/Inf cases, mess IX
	bcs	__modff_NaN		; if NaN, int_part=@long, return @long
	beq	__modff_ret_long	; if Inf, int_part=Inf, return 0.0
;
__modff_10:
	jsr	__get_lexp_sign		; __lexp = @long's exp, __sign = sign
	subb	#127			; exp < 0 ?  ( x<1.0 ?)
	bcc	__modff_mask
;
	tsx
	ldx	2,x
	jmp	__f32zerox		; *int_part = 0.0, and return
;
__modff_NaN:				; @long is NaN, *int_part = NaN
	tsx
	ldx	2,x
	jmp	__store32x		; (0-3,x) = @long and return
;
__modff_mask:
	cmpb	#23
	bcs	__modff_make_mask
;
__modff_ret_long:			; int_part = @long, return +0.0 or -0.0
	tsx
	ldx	2,x
	jsr	__store32x		; (0-3,x) = @long
;
__modff_ret_zero:
	jmp	__f32zeros		; return +0.0 or -0.0
;
__modff_make_mask:
	ldx	@long+2			; save @long
	stx	__save+2
	ldx	@long
	stx	__save
;
	jsr	__fracmask		; mask = (0x800000>>AccB)-1
	stx	__maskp
	jsr	__bit_fmask		; (@long & mask) == 0?
	jeq	__modff_ret_long	; no frac, int_part = @long, ret 0.0
;
;					; extract frac part
	jsr	__and_fmask
	ldx	@long+2			; save frac
	stx	__frac+2
	ldx	@long
	stx	__frac
;
;	Extract integer part by masking out the fraction bits
;
	ldx	__save+2
	stx	@long+2
	ldx	__save
	stx	@long
;
	ldx	__maskp
	jsr	__and_not_fmask		; @long &= ~mask
;
	tsx
	ldx	2,x			; IX = &int_part
	ldaa	__lexp
	ldab	@long+1
	aslb
	pshb
	ldab	__sign
	aslb
	rora
	pulb
	rorb
	staa	0,x
	stab	1,x
	ldab	@long+2
	stab	2,x
	ldab	@long+3
	stab	3,x
;
__modff_need_norm:
	ldaa	__lexp
;
__modff_norm_loop:
	deca			; Use a negative value to simplify later calc.
	asl	__frac+3
	rol	__frac+2
	rol	__frac+1
	bpl	__modff_norm_loop
;
	cmpa	#127
	bne	__modff_normal
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
	pshb
	ldab	__sign
	aslb
	rora
	pulb
	rorb
	stab	@long+1
	staa	@long;
	rts
