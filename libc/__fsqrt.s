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
	.export _fsqrt
	.data
__exp:	.byte	0		; exp
__q:	.byte	0,0,0,0		; sqrt_mantissa
__r:	.byte	0,0,0,0		; remainder
__s:	.byte	0,0,0,0		; partial_sqrt
__t:	.byte	0,0,0,0		; trial
;
	.code
;
;	@long = fsqrt(@long)
;
_fsqrt:
	jsr	__f32isNaNorInf	; @long is NaN or Inf?
	bcs	__fsqrt_ret	; @long is NaN, return @long
	bne	__fsqrt_01
	ldab	@long		; @long is Inf, sign?
	bpl	__fsqrt_ret	;   +Inf, return @long
	;			;   -Inf, return NaN
__retNaN:
	ldx	#$7FFF
	stx	@long
	ldx	#$FFFF
	stx	@long+2
__fsqrt_ret:
	rts
__fsqrt_01:			; @long is not NaN and Inf
	jsr	__f32iszero	; @long == 0.0?
	beq	__fsqrt_ret	; Yes, return @long
	ldab	@long		; @long < 0.0?
	bmi	__retNaN	; Yes, return NaN
;
	ldaa	@long+1
	asla
	ldaa	#0
	rolb			; now, AccB is exp
;
	bne	__fsqrt_05	; not subnormal
;
	incb			; subnormal's exp = -126
__fsqrt_03:
	subb	#1
	sbca	#0
        asl     @long+3
        rol     @long+2
        rol     @long+1         ; mant <<= 1
	bpl	__fsqrt_03	; loop until hidden bit==1
;
__fsqrt_05:
	asl	@long+1		; mant |= 0x00800000
	sec			; Set the bit without corrupting Acc
	ror	@long+1
	clr	@long
;
	subb	#127
	sbca	#0
;
__fsqrt_06:			; √(m*2^e) = √m * 2^(e/2) = √(m*2)* 2^((e-1)/2)
	bitb	#1
	beq	__fsqrt_10
	asl	@long+3
	rol	@long+2
	rol	@long+1
	rol	@long
	subb	#1
	sbca	#0
__fsqrt_10:
	asra
	rorb
	stab	__exp		; After a right shift, result fits in one byte
;
	ldab	@long+3		; mant<<=2
	aslb
	stab	@long+3
	ldab	@long+2
	rolb
	stab	@long+2
	ldab	@long+1
	rolb
	stab	@long+1
	ldab	@long
	rolb
	stab	@long
;
	ldx	#$0100
	stx	__r
	ldx	#0
	stx	__r+2
	stx	__q
	stx	__q+2
	stx	__s
	stx	__s+2
;
__fsqrt_20:
	ldx	__r
	bne	__fsqrt_21
	ldx	__r+2
	jeq	__fsqrt_29
__fsqrt_21:
	ldab	__r+3
	addb	__s+3
	stab	__t+3
	ldab	__r+2
	adcb	__s+2
	stab	__t+2
	ldab	__r+1
	adcb	__s+1
	stab	__t+1
	ldab	__r
	adcb	__s
	stab	__t
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
	bmi	__fsqrt_25
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
	ldab	__r+1
	adcb	__t+1
	stab	__s+1
	ldab	__r
	adcb	__t
	stab	__s
;
	ldab	__q+3
	addb	__r+3
	stab	__q+3
	ldab	__q+2
	adcb	__r+2
	stab	__q+2
	ldab	__q+1
	adcb	__r+1
	stab	__q+1
	ldab	__q
	adcb	__r
	stab	__q
;
	bra	__fsqrt_26
;
__fsqrt_25:
	ins
	ins
;
__fsqrt_26:
	asl	@long+3	
	rol	@long+2
	rol	@long+1
	rol	@long
;
	lsr	__r	
	ror	__r+1
	ror	__r+2
	ror	__r+3
;
	jmp	__fsqrt_20
;
__fsqrt_29:
;
	ldx	@long
	bne	__fsqrt_31
	ldx	@long+2
	beq	__fsqrt_32
__fsqrt_31:
	ldab	__q+3
	bitb	#1
	beq	__fsqrt_32
	addb	#1
	stab	__q+3
	ldab	__q+2
	adcb	#0
	stab	__q+2
	ldab	__q+1
	adcb	#0
	stab	__q+1
	ldab	__q
	adcb	#0
	stab	__q
;
__fsqrt_32:
	lsr	__q
	ror	__q+1
	ror	__q+2
	ror	__q+3
;
	ldab	__q+3
	stab	@long+3
	ldab	__q+2
	stab	@long+2
;
	clra
	ldab	__exp
	lsrb
	rora
;
	adda	__q+1
	staa	@long+1
	adcb	__q
	addb	#$3F
	andb	#$7F
	stab	@long
;
	rts
