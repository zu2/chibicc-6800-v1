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
        bcs     __isinf_not_inf ; NaN: return 0
	bne	__isinf_not_inf ; not Inf: return 0
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
	jsr	__f32isNaNorInf ; if NaN, C=1
        rolb
        clra
        andb    #1
        rts
;
;	AccAB = isfinite(@long)
;		parameter passed by @long
;               return (NaN or Inf)? 0: non-zero;
;
_isfinite:
	jsr	__f32isNaNorInf ; NaN: C=1, Inf: Z=1
	bls	__isfinite_non  ; C=1 or Z=1
____isinf_pInf:			; if +Inf, return 1
__isfinite_yes:
	clra
	ldab	#1
	rts
;
;       AccAB = signbit(@long)
;		parameter passed by @long
;               return (@long<0.0f)? non-zero: 0;
;
_signbit:
        clra
        ldab    @long
        andb    #$80
        rts
