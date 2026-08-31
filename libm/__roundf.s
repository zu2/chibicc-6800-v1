;
;	MC6800 floating point arithmetic library
;
;	Copyright (c) 2025 by ZUKERAN, shin
;	Released under the MIT license
;
;	https://github.com/zu2/chibicc-6800-v1?tab=License-1-ov-file#readme
;
;	Note: written for chibicc-6800-v1.
;	The code favors small size and speed on the MC6800.
;	Results are exact.
;	Inf, NaN, +0.0, -0.0, and subnormals are handled correctly.
;	IEEE 754 exceptions are not handled.
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
	jsr	__get_lexp_sign		; AccB = exp (biased)
	cmpb	#150			; exp>=23+127? NaN and Inf are exp==255
	bcc	__round_ret		; return @long
	subb	#127
	bcc	__round_ge1		; exp>126? (fabsf(@long)>=1.0f)
	ldab	__lexp
	cmpb	#126			; fabsf(@long)>=0.5f?
	bcs	__round_zero
	jmp	__f32ones		; return +-1.0f
__round_zero:
	jmp	__f32zeros		; return +-0.0f, keeps the sign
__round_ret:
	rts
;
__round_ge1:				; fabsf(@long) >= 1.0f
	ldab	__lexp
	subb	#127
	jsr	__fracmask		; make mantissa mask
	ldaa	__lexp
	jmp	__add_half_trunc	; +0.5, then truncate
