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
        .export	_nearbyintf
        .code
;
;       @long = nearbyintf(@long)
;               parameter passed by @long
;
;
_nearbyintf:
	jsr	__get_lexp_sign		; AccB = exp (biased)
	cmpb	#150			; exp>=23+127? NaN and Inf are exp==255
	bcc	__nbintf_ret		; return @long
	subb	#127
	bcc	__nbintf_ge1		; exp>=127 (fabsf(@long)>=1.0f)
	cmpb	#$FF			; exp==126? (0.5<=fabsf(@long)<1.0f)
	bne	__nbintf_zeros
	ldab	@long+1			; 0.5f is a tie, so only >0.5f gives 1.0f
	orab	@long+2
	orab	@long+3
	beq	__nbintf_zeros
	jmp	__f32ones
__nbintf_zeros:
	jmp	__f32zeros
__nbintf_ret:
	rts
;
__nbintf_ge1:				; fabsf(@long) >= 1.0f
	jsr	__fracmask		; make mantissa mask
	ldaa	__lexp
	jmp	__add_even_trunc	; round to the nearest even integer
