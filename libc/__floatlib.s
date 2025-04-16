;
;	MC6800 floating point arithmetic library
;
;	Copyright (c) 2025 by ZUKERAN, shin
;	Released under the MIT license
;
;	https://github.com/zu2/chibicc-6800-v1?tab=License-1-ov-file#readme
;
;	Note: his program was created for testing chibicc-6800-v1, 
;	and does not pay attention to speed, accuracy, or exception handling.
;
;
	.export	_fabs
	.data
	.code
;
;	@long = fabs(@long)
;	parameter passed by @long
;
_fabs:
	asl	@long		; sign bit = 0
	lsr	@long
	rts
