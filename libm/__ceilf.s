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
