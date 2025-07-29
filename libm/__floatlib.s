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
	.export	_fabsf
	.export	_isinf
	.export	_isnan
	.export	_isfinite
        .export _signbit
	.data
	.code
;
;	@long = fabs(@long)
;		parameter passed by @long
;
_fabsf:
	asl	@long		; sign bit = 0
	lsr	@long
	rts

;
;	AccAB = isinf(@long)
;		parameter passed by @long
;
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
__isfinite_non:
	clrb
	clra
	rts

;
;	AccAB = isnan(@long)
;		parameter passed by @long
;
_isnan:
	jsr	__f32isNaNorInf
	bcc	__isnan_not_nan
____isinf_pInf:			; if +Inf, return 1
__isfinite_yes:
	clra
	ldab	#1
	rts

;
;	AccAB = isfinite(@long)
;		parameter passed by @long
;
_isfinite:
	jsr	__f32isNaNorInf
	bcs	__isfinite_non
	beq	__isfinite_non
	bra	__isfinite_yes
;
;       AccAB = signbit(@long)
;		parameter passed by @long
;
_signbit:
        clra
        ldab    @long
        aslb
        rolb
        andb    #1
        rts
