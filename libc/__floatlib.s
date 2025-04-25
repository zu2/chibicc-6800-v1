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
	.export	_fabs
	.data
	.code
;
;	@long = fabs(@long)
;		parameter passed by @long
;
_fabs:
	asl	@long		; sign bit = 0
	lsr	@long
	rts

;
;	AccAB = isinf(@long)
;		parameter passed by @long
;
	.export	_isinf
_isinf:
	jsr	__f32isNaNorInf
	bne	__isinf_not_inf
	ldaa	@long
	bpl	____isinf_pInf
	ldab	#$FF		; if -Inf, return -1
	tba
	rts
__isinf_not_inf:
__isnan_not_nan:
	clrb
	clra
	rts

;
;	AccAB = isnan(@long)
;		parameter passed by @long
;
	.export	_isnan
_isnan:
	jsr	__f32isNaNorInf
	bcc	__isnan_not_nan
____isinf_pInf:			; if +Inf, return 1
	clra
	ldab	#1
	rts
