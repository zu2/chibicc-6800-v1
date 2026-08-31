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
        .export	_truncf
        .data
        .code
;
;       @long = truncf(@long)
;               parameter passed by @long
;
;
_truncf:
	jsr	__get_lexp_sign
	cmpb	#150
	bcc	__truncf_ret		; if exp>=150 return @long itself
	subb	#127
	jcs	__f32zeros		; if exp<=126 return ±0.0f
	jsr	__fracmask
	jmp	__and_not_fmask
__truncf_ret:
	rts
