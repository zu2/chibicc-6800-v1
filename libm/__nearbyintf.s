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
        .export	_nearbyintf
        .data
__fracmask_p:
	.word	0
__exp:
	.byte	0
        .code
;
;	if (mask & @long) == 0 , return Z=1, else Z=0
;
__nbintf_maskcheck:
	ldaa	0,x
	anda	@long+1
	bne	__nbintf_ret2
	ldaa	1,x
	anda	@long+2
	bne	__nbintf_ret2
	ldaa	2,x
	anda	@long+3
__nbintf_ret2:
	rts
;
;       @long = nearbyintf(@long)
;               parameter passed by @long
;
;
_nearbyintf:
	jsr	__get_lexp_sign
	cmpb	#150			; if exp>=150 return @long
	bcc	__nbintf_ret2
	cmpb	#126			; if exp<126 return +0.0 -0.0
	jcs	__f32zeros
;
	subb	#126
	pshb
	jsr	__fracmask
	stx	__fracmask_p
;
	pulb
	pshb
	jsr	__fmsbmask		; make (1<<(23-B))
;
	pulb
	tstb				; if exp==126, 0.5bit always 1 (hidden)
	beq	__nbintf_check
;
	bsr	__nbintf_maskcheck	; the 0.5 bit is 1 ?
	beq	__nbintf_trunc		; No, do truncate
;
__nbintf_check:				; the 0.5 bit is 1, check fracpart
	ldx	__fracmask_p
	bsr	__nbintf_maskcheck
	bne	__nbintf_roundup	; fracpart != 0, do roundup
;
					; only the 0.5 bit is 1, check LSB
	ldab	__lexp
	cmpb	#126			; if exp==126, LSB always 0
	jeq	__f32zeros		;    return 0.0f
	cmpb	#127			; if exp==127, LSB always 1 (hidden)
	beq	__nbintf_roundup
;
	subb	#127			; here, exp==128 to 149
	jsr	__fmsbmask		; LSB on?
	jsr	__nbintf_maskcheck
	bne	__nbintf_roundup
;
__nbintf_trunc:				; Truncate
	ldab	__lexp
	subb	#127
	jsr	__fracmask
	jmp	__and_not_fmask
;
__nbintf_roundup:
	ldab	__lexp
	cmpb	#126
	jeq	__f32ones		; return 1.0f
	bsr	__nbintf_trunc
	ldab	__lexp
	subb	#127
	jsr	__fmsbmask		; '1' bit
	ldab	@long+1
	orab	#$80			; hidden bit on
	stab	@long+1
	ldab	2,x
	addb	@long+3
	stab	@long+3
	ldab	1,x
	adcb	@long+2
	stab	@long+2
	ldab	0,x
	adcb	@long+1
;
	ldaa	__lexp
;
	bcc	__nbintf_rup_1
;
	inca
	cmpa	#$FF			; new exp==$FF ?
	jeq	__f32Infs		; Return Inf
;
__nbintf_rup_1:
	aslb
	asl	__sign
	rora
	rorb
	stab	@long+1
	staa	@long
	rts

