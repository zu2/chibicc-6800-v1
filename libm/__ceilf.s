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
;
        .export	_ceilf
        .data
__fracmask_p:
	.word	0
;
        .code
;
;       @long = ceil(@long)
;               parameter passed by @long
;
_ceilf:
	jsr	__f32isNaNorInf
	bls	__ceil_ret		; C=1 or Z=1 : C:NaN, Z:Inf
	jsr	__f32iszero
	bne	__ceil_non_zero
__ceil_ret:
	rts
__ceil_non_zero:
	jsr	__get_lexp_sign		; AccB = exp (biased)
	cmpb	#150			; exp>=23+127?
	bcc	__ceil_ret		; return @long
	subb	#127
	bcc	__ceil_ge1		; exp>126? (fabsf(@long)>=1.0f)
	ldab	__sign			; exp<=126, check sign
	jpl	__f32ones		; if @long>=0.0f return +1.0f
	clr	__sign
	jmp	__f32zeros		; if @long<0.0f  return +0.0f
;
__ceil_ge1:				; fabsf(@long) >= 1.0f
	jsr	__fracmask		; make mantissa mask
	jsr	__bit_fmask		; fac & mask == 0 ?
	beq	__ceil_ret		; @long is already an integer
	jsr	__and_not_fmask		; truncate fractional
	tst	__sign
	bmi	__ceil_ret
	ldab	__lexp
	subb	#127
	jsr	__fmsbmask		; '1' bit
	ldaa	__lexp
	jmp	__add_msb		; +1.0
