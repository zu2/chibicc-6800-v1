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
        .export	_ceilf
        .code
;
;       @long = ceil(@long)
;               parameter passed by @long
;               ceil(x) == -floor(-x)
;
_ceilf:
	ldaa	@long
	eora	#$80
	staa	@long
	jsr	_floorf
	ldaa	@long
	eora	#$80
	staa	@long
	rts
