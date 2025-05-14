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
;	@long = roundf(@long)
;		parameter passed by @long
;
;	float myround(float x) {
;	    if (x >= 0.0f)
;	        return floorf(x + 0.5f);
;	    else
;	        return ceilf(x - 0.5f);
;	}
;
	.export	_roundf
_roundf:
	ldaa	@long
	anda	#$80
	oraa	#$3F		; +0.5 or -0.5
	psha
	clrb
	pshb
	pshb
	pshb
	psha
	jsr	__addf32tos
	pula
	tsta	
	jpl	_floorf
	jmp	_ceilf
