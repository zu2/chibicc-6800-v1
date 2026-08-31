;
;	MC6800 floating point arithmetic library
;
;	Copyright (c) 2025 by ZUKERAN, shin
;	Released under the MIT license
;
;	https://github.com/zu2/chibicc-6800-v1?tab=License-1-ov-file#readme
;

;
;	Note: written for chibicc-6800-v1.
;	The code favors small size and speed on the MC6800.
;	Results are exact.
;	Inf, NaN, +0.0, -0.0, and subnormals are handled correctly.
;	IEEE 754 exceptions are not handled.
;

;
;	TODO:
;		more test
;		refactoring
;		Exception handling
;

	.export	_fmaxf
	.export	_fminf
;
	.data
__max_or_min:
	.byte	0	; max:$7F, min:$80
;
	.code
;
;	fmaxf(x,y)
;	  return (x>y)? x: y;
_fminf:
	ldab	#$80
	stab	__max_or_min
	bra	__fmaxmin_common
_fmaxf:
	ldab	#$7F
	stab	__max_or_min
__fmaxmin_common:
	tsx
	jsr	__setup_zin		; TOS & @long is zero/Inf/NaN?
	andb	#$03			; x or y is NaN ?
	beq	__fmaxmin_not_NaN	; not NaN
	cmpb	#$03			; x and y is NaN ?
	jeq	__f32NaN		; return NaN
	cmpb	#$01			; @long is NaN ?
	beq	__fmaxmin_ret_tos	; yes, return TOS
	rts				; TOS is NaN, return @long as it is.
;
__fmaxmin_ret_tos:			; return TOS
	tsx
	ldab	5,x
	stab	@long+3
	ldab	4,x
	stab	@long+2
	ldab	3,x
	stab	@long+1
	ldab	2,x
	stab	@long
__fmaxmin_ret:
	rts
;
;	__cmpf32_x2 orders Inf like any other float, so Inf needs no special case.
;
__fmaxmin_not_NaN:
	tsx				; __setup_zin leaves IX on long
	ldaa	__max_or_min
	bmi	__fmin_comp
__fmax_comp:
	jsr	__cmpf32_x2		; compare @long and TOS
	bmi	__fmaxmin_ret_tos	; @long < TOS (N=1)
	bra	__fmaxmin_ret
__fmin_comp:
	jsr	__cmpf32_x2
;	bcs	XXX?			; C==1 if NaN; shouldn't occur
	bpl	__fmaxmin_ret_tos	; @long >= TOS (N=0)
	bra	__fmaxmin_ret
