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
        .export	_floorf
        .data
__fracmask_p:
	.word	0
;
        .code
;
;       @long = floor(@long)
;               parameter passed by @long
;
_floorf:
	jsr	__f32isNaNorInf
	bls	__floor_ret		; C=1 or Z=1 : C:NaN, Z:Inf
	jsr	__f32iszero
	bne	__floor_non_zero
__floor_ret:
	rts
__floor_non_zero:
	jsr	__get_lexp_sign		; AccB = exp (biased)
	cmpb	#150			; exp>=23+127?
	bcc	__floor_ret		; return @long
	subb	#127
	bcc	__floor_ge1		; exp>126? (fabsf(@long)>=1.0f)
	ldab	__sign			; exp<=126, check sign
	jpl	__f32zeros		; if @long>=0.0f return +0.0f
	jmp	__f32ones		; if @long<0.0f  return -1.0f
;
__floor_ge1:				; fabsf(@long) >= 1.0f
	jsr	__fracmask		; make mantissa mask
	jsr	__bit_fmask		; fac & mask == 0 ?
	beq	__floor_ret		; @long is already an integer
	jsr	__and_not_fmask		; truncate fractional
	tst	__sign
	bpl	__floor_ret
	ldab	__lexp
	subb	#127
	jsr	__fmsbmask		; '1' bit
	ldaa	__lexp
	jmp	__add_msb		; +1.0
