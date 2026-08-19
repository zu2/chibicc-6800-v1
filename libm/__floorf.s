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
	jsr	__get_lexp_sign		; AccB = exp (biased)
	cmpb	#150			; exp>=23+127? NaN and Inf are exp==255
	bcc	__floor_ret		; return @long
	subb	#127
	bcc	__floor_ge1		; exp>126? (fabsf(@long)>=1.0f)
	ldab	__sign			; exp<=126, check sign
	jpl	__f32zeros		; if @long>=0.0f return +0.0f
	jsr	__f32iszero
	beq	__floor_ret		; -0.0f
	jmp	__f32ones		; if @long<0.0f  return -1.0f
__floor_ret:
	rts
;
__floor_ge1:				; fabsf(@long) >= 1.0f
	jsr	__fracmask		; make mantissa mask
	ldab	__sign
	bmi	__floor_neg
	jmp	__and_not_fmask		; @long>0: truncate fractional
__floor_neg:
	ldaa	__lexp
	jmp	__add_frac_trunc	; @long<0: round away from zero
