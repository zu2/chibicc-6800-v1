;
;	MC6800 floating point arithmetic library
;
;	Copyright (c) 2025 by ZUKERAN, shin
;	Released under the MIT license
;
;	https://github.com/zu2/chibicc-6800-v1?tab=License-1-ov-file#readme
;

;
;	Note: This program was created for testing chibicc-6800-v1, 
;	and does not pay attention to speed, accuracy, or exception handling.
;

;
;	TODO:
;		Changed variable names in division routines
;		Improved accuracy of division routines
;
;		more test
;		refactoring
;		Exception handling
;		Speed up
;

	.zp
	.data
	.export	_fmaxf
	.export	_fminf
;
	.data
__zin:	.byte	0	; TOS & @long are Zero? Inf? NaN?
__max_or_min:
	.byte	0	; max:$7F, min:$80
	.word	0
	.word	0
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
	bne	__fmaxmin_ret_tos
	rts				; return @long as it is.
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
	rts
;
__fmaxmin_not_NaN:
	ldab	__zin
	andb	#$C0			; x or y is Inf ?
	bne	__fmaxmin_not_Inf
	cmpb	#$C0			; x and y is Inf ?
	bne	__fmaxmin_Inf_2
	ldaa	@long			; check sign
	eora	2,x
	bpl	__fmaxmin_ret		; x and y have the same sign, rts
;
	ldab	__max_or_min
	bmi	__fmin_mInf
;
	andb	@long			; clear sign (→ +Inf)
	stab	@long
	rts
__fmin_mInf:
	orb	@long			; set sign (→ -Inf)
	stab	@long
__fmaxmin_ret:
	rts
;
__fmaxmin_Inf_2:				; Either x or y is Inf.
	ldaa	__max_or_min
	bmi	__fmin_Inf_2
;					; fmaxf(x,y)
	cmpb	#$04			; @long is Inf?
	bne	__fmax_Inf_3
	tst	@long			; check sign
	bpl	__fmaxmin_ret		; @long==+Inf, return it
	bra	__fmaxmin_ret_tos	; @long==-Inf, return TOS
__fmax_Inf_3:
	tst	2,x			; TOS==Inf, check sign
	bpl	__fmaxmin_ret_tos	; TOS==+Inf, return it.
	bra	__fmaxmin_ret		; return @long
;
__fmin_Inf_2:
	cmpb	#$04			; @long is Inf ?
	bne	__fmin_Inf_3
	tst	@long
	bmi	__fmaxmin_ret		; @lomg == -Inf, return it
	bra	__fmaxmin_ret_tos	; @long == +Inf, return TOS
;
__fmin_Inf_3:
	tst	2,x			; TOS == Inf, check sign
	bmi	__fmaxmin_ret_tos	; TOS == -Inf, return it
	bra	__fmaxmin_ret		; return @long
;
__fmaxmin_not_Inf:			; Neither x nor y is NaN or Inf.
	ldaa	__max_or_min
	bmi	__fmin_comp
__fmax_comp:
	jsr	__cmpf32x		; compare @long and TOS
;	bcs	XXX?			; C==1 if NaN; shouldn't occur
	bmi	__fmaxmin_ret_tos	; @long < TOS (N=1)
	bra	__fmaxmin_ret	
__fmin_comp:
	jsr	__cmpf32x
;	bcs	XXX?			; C==1 if NaN; shouldn't occur
	bpl	__fmaxmin_ret_tos	; @long >= TOS (N=0)
	bra	__fmaxmin_ret
