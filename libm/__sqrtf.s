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
	.export _sqrtf
	.data
__exp:	.byte	0		; exp
__r:	.byte	0,0,0,0		; remainder
__s:	.byte	0,0,0,0		; twice the partial sqrt_mantissa
__t:	.byte	0,0,0,0		; trial
;
	.code
;
;	@long = sqrtf(@long)
;
_sqrtf:
	jsr	__f32isNaNorInf	; @long is NaN or Inf?
	bcs	__sqrtf_ret	; @long is NaN, return @long
	bne	__sqrtf_01
	ldab	@long		; @long is Inf, sign?
        bpl     __sqrtf_ret     ;   +Inf, return @long
__sqrtf_NaN:
	jmp	__f32NaN	;   -Inf, return NaN
__sqrtf_ret:
	rts

__sqrtf_01:			; @long is not NaN and Inf
	jsr	__f32iszero	; @long == 0.0?
	beq	__sqrtf_ret	; Yes, return @long
	ldab	@long		; @long < 0.0?
	bmi	__sqrtf_NaN	; Yes, return NaN
;
	ldaa	@long+1
	asla
	ldaa	#0
	rolb			; now, AccB is exp
;
	bne	__sqrtf_05	; not subnormal
;
	incb			; subnormal's exp = -126
__sqrtf_03:
	subb	#1
	sbca	#0
        asl     @long+3
        rol     @long+2
        rol     @long+1         ; mant <<= 1
	bpl	__sqrtf_03	; loop until hidden bit==1
;
__sqrtf_05:
	asl	@long+1		; mant |= 0x00800000
	sec			; Set the bit without corrupting Acc
	ror	@long+1
	clr	@long
;
	subb	#127
	sbca	#0
;
__sqrtf_06:			; √(m*2^e) = √m * 2^(e/2) = √(m*2)* 2^((e-1)/2)
	bitb	#1
	beq	__sqrtf_10
	asl	@long+3
	rol	@long+2
	rol	@long+1
	rol	@long
	subb	#1
	sbca	#0
__sqrtf_10:
	asra
	rorb
	stab	__exp		; After a right shift, result fits in one byte
;
	asl	@long+3		; mant <<= 2
	rol	@long+2
	rol	@long+1
	rol	@long
;
	ldx	#$0100
	stx	__r
	ldx	#0
	stx	__r+2
	stx	__s
	stx	__s+2
;
	ldx	#9		; 1st loop. __t+2,__t+3 always 0
__sqrtf_12:
	ldab	__r+1
	addb	__s+1
	stab	__t+1
	ldab	__r
	adcb	__s
	stab	__t
;
	ldab	@long+1
	subb	__t+1
	ldaa	@long
	sbca	__t
	bmi	__sqrtf_15
;
	stab	@long+1
	staa	@long
;
	ldab	__r+1
	addb	__t+1
	stab	__s+1
	ldab	__r
	adcb	__t
	stab	__s
;
__sqrtf_15:
	asl	@long+3		; @long << 1
	rol	@long+2
	rol	@long+1
	rol	@long
;
	lsr	__r		; __r>>1
	ror	__r+1
	ror	__r+2
;
	dex
	bne	__sqrtf_12
;
	ldab	__s		; r+1,r are 0, move here.
	stab	__t
;
	ldx	#16		; 2nd loop
__sqrtf_20:
	ldab	__r+3
	addb	__s+3
	stab	__t+3
	ldab	__r+2
	adcb	__s+2
	stab	__t+2
	ldab	__s+1
	adcb	#0
	stab	__t+1
;	ldab	__r+1		; r+1,r are 0, move out of loop. ↑
;	adcb	__s+1
;	stab	__t+1
;	ldab	__r
;	adcb	__s		; __t < 2^26, so no carry reaches
;	stab	__t
;
	ldab	@long+3
	subb	__t+3
	ldaa	@long+2
	sbca	__t+2
	pshb
	psha
	ldab	@long+1
	sbcb	__t+1
	ldaa	@long
	sbca	__t
	bmi	__sqrtf_25
;
	stab	@long+1
	staa	@long
	pula
	pulb
	stab	@long+3
	staa	@long+2
;
	ldab	__r+3
	addb	__t+3
	stab	__s+3
	ldab	__r+2
	adcb	__t+2
	stab	__s+2
	ldab	__t+1
	adcb	#0
	stab	__s+1
;
	bra	__sqrtf_26
;
__sqrtf_25:
	ins
	ins
;
__sqrtf_26:
	asl	@long+3	
	rol	@long+2
	rol	@long+1
	rol	@long
;
	lsr	__r+2
	ror	__r+3
;
	dex
	bne	__sqrtf_20
;
__sqrtf_29:
;
	ldab	__s+3
	ldaa	__s+2
;
	lsr	__s		; __s>>1
	ror	__s+1
	rora
	rorb
;
	lsr	__s		; __s>>1
	ror	__s+1
	rora
	rorb
	bcc	__sqrtf_32	; G==1?
;
	addb	#1		; yes, round up.
	adca	#0
	bcc	__sqrtf_32
	inc	__s+1
;
__sqrtf_32:
	stab	@long+3
	staa	@long+2
;
	clrb
	ldaa	__exp
	lsra
	rorb
;
	addb	__s+1
	stab	@long+1
	adca	#$3F
	anda	#$7F
	staa	@long
;
	rts
