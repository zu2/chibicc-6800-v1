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
        .export	_roundf
        .data
;
        .code
;
;       @long = roundf(@long)
;               parameter passed by @long
;
_roundf:
	jsr	__f32isNaNorInf
	bls	__round_ret		; C=1 or Z=1 : C:NaN, Z:Inf
	jsr	__f32iszero
	bne	__round_non_zero
__round_ret:
	rts
__round_non_zero:
	jsr	__get_lexp_sign		; AccB = exp (biased)
	cmpb	#150			; exp>=23+127?
	bcc	__round_ret		; return @long
	subb	#127
	bcc	__round_ge1		; exp>126? (fabsf(@long)>=1.0f)
	ldab	__lexp
	cmpb	#126			; fabsf(@long)>=0.5f?
	bcs	__round_zero
	jmp	__f32ones		; return +-1.0f
__round_zero:
	jmp	__f32zeros		; return +-0.0f
;
__round_ge1:				; fabsf(@long) >= 1.0f
	ldab	__lexp
	subb	#126			; '0.5' bit position
	jsr	__fmsbmask
	jsr	__bit_fmask		; fac & '0.5' bit == 0 ?
	psha
	ldab	__lexp
	subb	#127
	jsr	__fracmask		; make mantissa mask
	jsr	__and_not_fmask		; truncate fractional
	pula
	tsta
	beq	__round_ret
	ldab	__lexp
	subb	#127
	jsr	__fmsbmask		; '1' bit
	ldaa	__lexp
	jmp	__add_msb		; +1.0
