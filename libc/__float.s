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
	.export	__i16tof32
	.export	__u16tof32
	.export	__i32tof32
	.export	__u32tof32
	.export	__f32tou32
	.export	__f32toi32
	.export	__f32tou16
	.export	__f32toi16
	.export	__addf32tos
	.export __subf32tos
	.export __mulf32tos
	.export __divf32tos
	.export __cmpf32tos
	.export __cmpf32x
	.export __f32iszero
;	.export __f32iszerox
	.export __f32isNaNorInf
;	.export __f32isNaNorInfx
	.export __fdiv32x32
	.export	__setup_long
	.export __f32retpZero
;
	.export __sign
	.export __work
	.data
__zin:	.byte	0	; TOS & @long are Zero? Inf? NaN?
__sign:	.byte	0	; sign (TOS & @long sign are different? 1:differ,0:same)
__texp:	.byte	0	; TOS's exp
__lexp:	.byte	0	; @long's exp
__expdiff:.byte	0	; TOS's exp - @long's exp
__exp2: .word	0	; exp work. subnormal use 2byte (127 to -149)
__work: .word	0	; working area 48bit
	.word	0
	.word	0
	.code
;
;	(0-3,x) is NaN? (exp==255)
;		NaN: 7F800000-7FFFFFFF
;		Inf: 7F800000 or FF800000
;	C=1: NaN
;	Z=1: Inf
;
__f32isNaNorInf:
	ldx	#long
__f32isNaNorInfx:
	ldab	1,x
	ldaa	0,x
	aslb
	rola
	adda	#1		; Use adda to update the carry flag (not inca).
	bne	__f32isNaN_1	; if exp!=$FF, not NaN and Inf. C=0,Z=0
	;			; exp == 255, check mantissa
	;			; Hera, a is zero
	sba			; a-b → 0-b
	bne	__f32isNaN_1	; if b==0 then Z=1,C=0 else Z=0,C=1
	cmpa	2,x		; cmpa is faster than tst
	bne	__f32isNaN_1	; ditto
	cmpa	3,x		
__f32isNaN_1:			; Z=0, C=0 not NaN,Inf
	rts			; Z=1, C=0 Inf
				; Z=0, C=1 NaN
;
;	(0-3,x) == 0.0 ?
;		== 0x0000 0000 or 0x8000 0000	
;	if 0.0 then Z=1 else Z=0
;
__f32iszero:
	ldx	#long
__f32iszerox:
	ldab	0,x		; exponent on the MSB side, to check from the top is faster
	andb	#$7F
	bne	iszerox_ret
				; Here, b is zero
	cmpb	1,x		; cmpb is faster than tst
	bne	iszerox_ret
	cmpb	2,x
	bne	iszerox_ret
	cmpb	3,x
iszerox_ret:
	rts
;
;	negate 32bit long
;
__negi32:
	ldx	#long
__negi32x:
	com	0,x	
	com	1,x	
	com	2,x	
	neg	3,x	
	bne	__negi32xret
	inc	2,x
	bne	__negi32xret
	inc	1,x
	bne	__negi32xret
	inc	0,x
__negi32xret:
	rts
;
;	int16 to float32
;
__i16tof32:
	tstb
	bne	__i16tof32_1
	tsta
	jeq	__f32zero	; load 0.0 and return
	;
__i16tof32_1:
	psha
	anda	#$80
	staa	__sign
	pula
	bpl	__i16tof32_10
	nega
	negb
	sbca	#0
	;
__i16tof32_10:
	stab	@long+2
	ldab	#$8f		; exp ($8e+1), if i32>=32768 then exp=$8E
__i16tof32_20:			; Shift left until the most significant bit becomes 1
	decb
	asl	@long+2
	rola
	bcc	__i16tof32_20	; loop until C=1 (hidden bit check)
__i16tof32_21:
	lsrb
	rora			; shifted over 1bit, fix it.
	ror	@long+2
	staa	@long+1
	orab	__sign
	stab	@long
	clr	@long+3
	rts
;
;	uint16 to float32
;
__u16tof32:
	tstb
	bne	__u16tof32_1
	tsta
	jeq	__f32zero	; load 0.0 and return
	;
__u16tof32_1:
	stab	@long+2
	ldab	#$8f
__u16tof32_20:			; Shift left until most significant bit to 1
	decb
	asl	@long+2
	rola
	bcc	__u16tof32_20	; loop until C=1 (hidden bit check)
__u16tof32_21:
	lsrb
	rora			; shifted over 1bit, fix it.
	ror	@long+2
	staa	@long+1
	stab	@long
	clr	@long+3
	rts
;
;
;
__f32minint:			; -2147483648 (0x8000 0000) = CF00 0000
	ldx	#0
	stx	@long+2
	ldx	#$CF00
	stx	long
	rts
;
;	int32 to float32
;		TODO: round up/down
;
__i32tof32:
	ldx	#long
__i32tof32x:
	ldab	0,x
	orab	1,x
	orab	2,x
	orab	3,x
	jeq	__f32zero	;   return +0.0
__i32tof32_1:
	ldab	0,x
	andb	#$80
	stab	__sign		; b7:sign bit
	bpl	__i32tof32_2
	jsr	__negi32x	; negate (0-3,x)
__i32tof32_2:
	ldaa	0,x
	beq	__i32tof32_left	; need left shift
;
	ldab	#$96		; exp.
	clr	__work
__i32tof32_right:		; right shift is required until the MSB byte becomes 0
	incb
	lsra
	ror	1,x
	ror	2,x
	ror	3,x
	ror	__work		; save R/S bit
	tsta
	bne	__i32tof32_right
	ldaa	__work
	bpl	__i32tof32_done	; if R==0 no round up.
	anda	#$7F		; get sticky
	bne	__i32tof32_rup	; if S==1 do round up.
	ldaa	3,x
	lsra
	bcc	__i32tof32_done	; LSB==0?
;				; R==1 && (sticy || LSB==1)
__i32tof32_rup:
	inc	3,x
	bne	__i32tof32_done
	inc	2,x
	bne	__i32tof32_done
	inc	1,x
	bne	__i32tof32_done
;				; carry occurred from rounding. Shift 1bit
;	sec			; MSB is hidden bit, it doesn't need to be set.
	ror	1,x
	ror	2,x
	ror	3,x
	incb			; exp++
;
__i32tof32_done:
	asl	1,x		; clear hidden bit and set exp's LSB
	lsrb
	ror	1,x
	orab	__sign
	stab	0,x
	rts
__i32tof32_left:		; left shift is required until hidden bit==1
	ldab	#$96
	tst	1,x		; MSB bit already set?
	bmi	__i32tof32_done
__i32tof32_left2:
	decb
	asl	3,x
	rol	2,x
	rol	1,x
	bpl	__i32tof32_left2
	bra	__i32tof32_done
;
;	uint32 to float32
;		TODO: round up/down
;
__u32tof32:
	ldx	#long
__u32tof32x:
	ldab	0,x
	orab	1,x
	orab	2,x
	orab	3,x
	jeq	__f32zero	;   return +0.0
;
	ldaa	0,x
	beq	__u32tof32_left	; need left shift
;
	ldab	#$96		; exp.
	clr	__work
__u32tof32_right:		; right shift is required until the MSB byte becomes 0
	incb
	lsra
	ror	1,x
	ror	2,x
	ror	3,x
	ror	__work		; save R/S bit
	tsta
	bne	__u32tof32_right
	ldaa	__work
	bpl	__u32tof32_done	; if R==0 no round up.
	anda	#$7F		; get sticky
	bne	__u32tof32_rup	; if S==1 do round up.
	ldaa	3,x
	lsra
	bcc	__u32tof32_done	; LSB==0?
;				; R==1 && (sticy || LSB==1)
__u32tof32_rup:
	inc	3,x
	bne	__u32tof32_done
	inc	2,x
	bne	__u32tof32_done
	inc	1,x
	bne	__u32tof32_done
;				; carry occurred from rounding. Shift 1bit
;	sec			; MSB is hidden bit, it doesn't need to be set.
	ror	1,x
	ror	2,x
	ror	3,x
	incb			; exp++
;
__u32tof32_done:
	asl	1,x		; clear hidden bit and set exp's LSB
	lsrb
	ror	1,x
	stab	0,x
	rts
__u32tof32_left:		; left shift is required until hidden bit==1
	ldab	#$96
	tst	1,x		; MSB bit already set?
	bmi	__u32tof32_done
__u32tof32_left2:
	decb
	asl	3,x
	rol	2,x
	rol	1,x
	bpl	__u32tof32_left2
	bra	__u32tof32_done
;
;	float to unsigned long
;		@long -> @long
;	
__f32tou32:
	ldx	#long
	jsr	__f32iszerox
	beq	__u32zero
	ldab	0,x
	bmi	__u32zero	; if x<0 then return 0
	ldaa	1,x
	asla
	rolb			; B = exp
	cmpb	#$3f		; if exp<=$3e (x < 0.5) then return 0;
	bcc	__f32tou32_1
	jmp	__u32zero
__f32tou32_1:
	cmpb	#$9f		; if exp>=$9f (x >= 4,294,967,295)
	bcs	__f32tou32_2
	jmp	__u32ffffffff	; return 4,294,967,295
__f32tou32_2:
	clr	0,x
	ldaa	1,x		; recover hidden bit
	ora	#$80
	staa	1,x
	subb	#$96		; TODO:
	beq	__f32tou32_ret
	bcs	__f32tou32_4
__f32tou32_3:
	asl	3,x
	rol	2,x
	rol	1,x
	rol	0,x
	decb
	bne	__f32tou32_3
__f32tou32_ret:
	rts
__f32tou32_4:
	lsr	0,x
	ror	1,x
	ror	2,x
	ror	3,x
	incb
	bne	__f32tou32_4
	rts
;
__f32zero:
__i32zero:
__u32zero:
	ldx	#0
	stx	@long+2
	stx	@long
	rts
__f32ffffffff:
__i32ffffffff:
__u32ffffffff:
	ldx	#$ffff
	stx	@long+2
	stx	@long
	rts
__i327fffffff:
	ldx	#$ffff
	stx	@long+2
	ldx	#$7fff
	stx	@long
	rts
__i3280000000:
	ldx	#0
	stx	@long+2
	ldx	#$8000
	stx	@long
	rts
;
;	load plus/minus Inf into @long
;
__f32retInfs:
	ldab	__sign
__f32retInf:		; AccB(Sign)+7f80 0000
	tstb
	bmi	__f32retmInf
__f32retpInf:		; 7f80 0000
	ldab	#$7f
	stab	@long
	incb
	stab	@long+1
	clrb
	stab	@long+2
	stab	@long+3
	jmp     __pullret
__f32retmInf:		; ff80 0000
	ldab	#$80
	stab	@long+1
	clrb
	stab	@long+2
	stab	@long+3
	decb
	stab	@long
	jmp     __pullret
;
;	load plus/minus qNaN into @long
;
__f32retNaN:
	ldab	__sign
	orab	#$7F
	stab	@long
	ldab	#$FF
	stab	@long+1
	stab	@long+2
	stab	@long+3
	jmp	__pullret	
;
__f32retZerox:
	ldab	2,x
	bra	__f32retZero
__f32retZerol:
	ldab	@long
	bra	__f32retZero
__f32retmZero:
	ldab	#$80
	bra	__f32retZero
__f32retpZero:
	clrb
	bra	__f32retZero
__f32retZeros:
	ldab	__sign
__f32retZero:
	andb	#$80
	stab	@long
	clrb
	stab	@long+1
	stab	@long+2
	stab	@long+3
	jmp	__pullret
;
;	pull TOS into @long, and return
;
__f32retTOS:
	tsx
	ldx	0,x
	ins
	ins
	pulb
	stab	@long
	pulb
	stab	@long+1
	pulb
	stab	@long+2
	pulb
	stab	@long+3
	jmp	0,x
;
;	float to signed long
;		@long -> @long
;	
__f32toi32:
	ldx	#long
__f32toi32x:
	jsr	__f32iszerox
	jeq	__u32zero
	ldab	0,x
	stab	__sign		; save sign
	ldaa	1,x
	asla
	rolb			; B = exp
	cmpb	#$3f		; if exp<=$3e (x < 0.5) then return 0;
	jcs	__u32zero
__f32toi32_1:
	cmpb	#$9E		; if exp>=$9E (x > 2,147,483,647)
	bcs	__f32toi32_2
	tst	__sign
	jpl	__i327fffffff	; return +2,147,483,647
	jmp	__i3280000000	; return -2,147,483,648
__f32toi32_2:
	clr	0,x
	sec			; recover hidden bit
	rora
	staa	1,x
	subb	#$96		; exp==2^23?
	beq	__f32toi32_ret	; 24bit mantissa can be interpreted as long.
	bcs	__f32toi32_4
__f32toi32_3:
	asl	3,x
	rol	2,x
	rol	1,x
	rol	0,x
	decb
	bne	__f32toi32_3
__f32toi32_ret:
	tst	__sign
	jmi	__negi32x	; negate (0-3,x) and return
__f32toi32_ret2:
	rts
__f32toi32_4:
	clra
__f32toi32_5:
	lsr	0,x
	ror	1,x
	ror	2,x
	ror	3,x
	rora			; G/S bit
	incb
	bne	__f32toi32_5
	tsta
	bpl	__f32toi32_ret	; G=0, no round up
	bitb	#$7F
	bne	__f32toi32_6	; round up
	ldaa	3,x
	lsra
	bcc	__f32toi32_ret	; S=0, LSB=0, no round up
;
__f32toi32_6:
	inc	3,x		; round up
	bne	__f32toi32_ret
	inc	2,x
	bne	__f32toi32_ret
	inc	1,x
	bne	__f32toi32_ret
	inc	0,x
	bra	__f32toi32_ret
;
;	float to unsigned int
;		@long -> AccA:B
;	
__f32tou16:
	ldx	#long
__f32tou16x:
	jsr	__f32iszerox
	beq	__u16zero
__f32tou16_0:
	ldab	0,x
	bmi	__u16zero	; if x<0 then return 0
	ldaa	1,x
	asla
	rolb			; B = exp
	sec
	rora			; A = MSB
	cmpb	#$3f		; if exp<=$3e (x < 0.5) then return 0;
	bcc	__f32tou16_1
__u16zero:
	clrb
	clra
	rts
;
__f32tou16_1:
	subb	#$8f		; if exp>=$8f (x >= 65536)
	jcc	__u16ffff	; return 65535
;
	incb			; exp==$8e ?
	beq	__f32tou16_ret
__f32tou16_3:
	lsra
	ror	2,x
	ror	3,x
	incb
	bne	__f32tou16_3
__f32tou16_ret:
	ldab	2,x
;	ldaa	1,x
	rts
;
;	float to signed short/int
;		@long -> AccA:B
;	
__f32toi16:
	ldx	#long
__f32toi16x:
	jsr	__f32iszerox
	beq	__s16zero
	ldab	0,x
	ldaa	1,x
	asla
	rolb			; B = exp
	sec			; set hidden bit
	rora			; A = MSB
	cmpb	#$3f		; if exp<=$3e (x < 0.5) then return 0;
	bcc	__f32toi16_1
__s16zero:
	clrb
	clra
	rts
;
__f32toi16_1:
	cmpb	#$8e		; if exp>=$8e (x > 32767)
	bcs	__f32toi16_2
	ldaa	0,x
	bmi	__s16_8000	; x <= -32768
__s16_7fff:			; x > 32767, return 32767
	ldab	#$FF
	ldaa	#$7F
	rts
__s16_8000:			; x <= -32768, return -32768
	clrb
	ldaa	#$80
	rts
;
__f32toi16_2:			; AccA:MSB, AccB:exp (biased)
	subb	#$8E
	beq	__f32toi16_ret
__f32toi16_4:
	lsra
	ror	2,x
	incb
	bne	__f32toi16_4
__f32toi16_ret:
	ldab	2,x
;	ldaa	1,x
	tst	0,x
	bpl	__f32toi16_ret2
	nega
	negb
	sbca	#0
__f32toi16_ret2:
	rts
;
__u16ffff:
	ldab	#$FF
	tba
	rts
;
;	@long	= @long - TOS
;	→	= @long + (-TOS)
;
__subf32tos:
	tsx
	ldab	2,x		; flip the sign of TOS
	eorb	#$80
	stab	2,x
;	jmp	addf32tos	; thru to __addf32tos
;
;	@long = TOS + @long
;	pull TOS
;
__addf32tos:
	tsx
	jsr	__setup_zin	; TOS & @long is zero/Inf/NaN?
;	ldab	__zin
	bitb	#$3F
	beq	__addf32_1	; No,  normal calculation
;
	bitb	#$03		; TOS or @long is NaN?
	bne	__addf32_retNaN ; Yes: return NaN
;
	andb	#$0C		; TOS or @long is Inf?
	beq	__addf32_s20
	cmpb	#$0C		; TOS and @long are Inf?
	bne	__addf32_s10
	ldab	__sign		; each sign are same?
	bpl	__addf32_s05	; 
__addf32_retNaN:
	jmp	__f32retNaN	; No,  return NaN
__addf32_s05:
	ldab	@long		; Yes, return Inf. sign is the same as @long
	jmp	__f32retInf
	;
__addf32_s10:
	ldab	__zin		; Either TOS or @long is Inf.
	cmpb	#$04		; @long is Inf?
	bne	__addf32_s05	; No, return Inf, sign is same as @long
	tsx
	ldab	2,x
	jmp	__f32retInf	; return Inf, The sign is the same as TOS
;
__addf32_s20:			; TOS and @long are not NaN,Inf.
	ldab	__zin
	andb	#$30		; TOS or @long == 0.0?
	beq	__addf32_1	; No
	cmpb	#$30		; TOS and @long == 0.0?
	jne	__addf32_s50
	tst	__sign		; Yes. same sign?
	jne	__f32retpZero	; Not same sign. return +0.0
	jmp	__f32retZerol	; return 0.0, sign is same as @long
;
__addf32_s50:			; TOS or @long == 0.0
	cmpb	#$20		; TOS == 0.0?
	beq	__addf32_s51	; Yes, return @long (do nothing)
	tsx			; No,  return TOS
	inx
	inx
	jsr	__load32x	; @long <= (0-3,x)
__addf32_s51:
	jmp	__pullret
;
__addf32_1:			; neither of @long and TOS was not 0.0, simply add them.
	tsx
	ldab	2,x		; get MSB of TOS
	eorb	@long
	andb	#$80		; sign equal?
	jne	__addf32_50
	;			; sign equal, add it
	ldab	@long		; save sign
	andb	#$80
	stab	__sign
	jsr	__setup_both	; get both exp to texp, lexp. AccB = texp - lexp
	beq	__addf32_11	; texp==lexp, Simply add it (AccA has __texp).
	bcc	__addf32_10	; jump if texp > lexp
	;			
	negb			; texp < lexp
	jsr	__lsr_tos	; shift TOS right by AccB, align the bit.
	ldaa	__lexp		; @long is larger, use its exponent.
	jmp	__addf32_11
__addf32_10:			; texp > lexp
	jsr	__lsr_long	; shift @long right by AccB, align the bit
	ldaa	__texp		; TOS is larger, use its exponent.
__addf32_11:
	ldab	@long+3		; @long = @long + TOS , 32bit version
	addb	5,x
	stab	@long+3
	ldab	@long+2
	adcb	4,x
	stab	@long+2
	ldab	@long+1
	adcb	3,x
	stab	@long+1
	ldab	@long
	adcb	2,x
	stab	@long
	bcc	__addf32_20	; over flow?
        ror     @long		; shift one bit with carry
        ror     @long+1
        ror     @long+2
        ldab	@long+3		; sticky
	rorb
	bitb	#$3F	
	beq	__addf32_12
	orab	#$20
__addf32_12:
	stab	@long+3
	inca			; exp++
	cmpa	#$FF		; biased exponent exceeds 254, so it is Inf.
	jeq	__f32retInfs
__addf32_20:			; even number rounding
	ldab	@long+2
	lsrb			; LSB -> Carry
	ldab	@long+3
	rorb			; b7:LSB, b6:G, b5:R, b4:S
	bitb	#$40		; b6:G==0?
	beq	__addf32_29	;   Yes, do nothng
	andb	#$F0
	cmpb	#$40		; 0100:only G is 1?
	beq	__addf32_29	;   Yes, do nothng
;
	inc	@long+2		; round up
	bne	__addf32_29
	inc	@long+1
	bne	__addf32_29
	inc	@long+0
	bne	__addf32_29
;
	inca
	cmpa	#$FF
	jeq	__f32retInfs
	lsr	@long+0
	ror	@long+1
	ror	@long+2
;
__addf32_29:
	cmpa	#1		; sub normal number?
	bne	__addf32_30
	tst	@long		; check hiden bit, when 1 convert to normal
	bmi	__addf32_30
	clra
__addf32_30:
	jsr	__lsr8_long	; Put it back in the right position
;
	asl	@long+1		; exp's LSB set to @long+1
	lsra
	ror	@long+1
	ora	__sign
	staa	@long		; set exp
	jmp	__pullret
;
;	TOS and @long have different signs, do subtract
;	(note: TOS:rhs, @long:lhs)
;
__addf32_50:
	jsr	__abscmp	; compare: abs(tos) - abs(@long)
	jeq	__f32retpZero	; different sign, but value equal, return +0.0
__addf32_51:
	jcs	__addf32_60	; jump if TOS<@long
	;
	ldab	2,x		; abs(TOS) > abs(@long), result sign is TOS's
	andb	#$80
	stab	__sign
	jsr	__setup_both	; AccB = TOS'exp - @long's exp
	beq	__addf32_52	; Both exp were equal,  simply substract
	jsr	__lsr_long	; shift @long right by AccB
__addf32_52:
	ldaa	__texp
	;			; substract @long = TOS - @long
	ldab	5,x
	subb	@long+3
	stab	@long+3
	ldab	4,x
	sbcb	@long+2
	stab	@long+2
	ldab	3,x
	sbcb	@long+1
	stab	@long+1
	ldab	2,x
	sbcb	@long
	stab	@long
__addf32_54:
	bmi	__addf32_543	; hidden bit on?
	;
__addf32_541:
	deca
	beq	__addf32_543	; subnormal number. stop shift
	ldab	@long+3
	bitb	#$1F
	beq	__addf32_542
	ora	#$10		; sticky
__addf32_542:
	aslb
	stab	@long+3
	rol	@long+2
	rol	@long+1
	rol	@long+0
	jpl	__addf32_541	; hidden bit become 1 ?
;
__addf32_543:
	ldab	@long+2
	lsrb			; LSB -> Carry
	ldab	@long+3
	rorb			; b7:LSB, b6:G, b5:R, b4:S
	bitb	#$40		; b6:G==0?
	beq	__addf32_55	;   Yes, do nothng
	andb	#$F0
	cmpb	#$40		; 0100:only G is 1?
	beq	__addf32_55	;   Yes, do nothng
;
	inc	@long+2		; round up
	bne	__addf32_55
	inc	@long+1
	bne	__addf32_55
	inc	@long+0
	bne	__addf32_55
;
	inca
	cmpa	#$FF
	jeq	__f32retInfs
	lsr	@long+0
	ror	@long+1
	ror	@long+2
;
__addf32_55:
	jsr	__lsr8_long
	asl	@long+1		; exp's LSB into @long+1
	lsra
	ror	@long+1
	ora	__sign		; recover sign bit
	staa	@long
	jmp	__pullret
;
__addf32_60:
;				; abs(TOS) < abs(@long), return @long's sign
	ldab	@long
	andb	#$80
	stab	__sign
	jsr	__setup_both	; AccB = TOS'exp - @long's exp
	beq	__addf32_61	; AccB == 0, simply substract
	negb
	jsr	__lsr_tos	; shift TOS right by AccB
__addf32_61:
	ldaa	__lexp
	;			; substract @long = @long - TOS
	ldab	@long+3
	subb	5,x
	stab	@long+3
	ldab	@long+2
	sbcb	4,x
	stab	@long+2
	ldab	@long+1
	sbcb	3,x
	stab	@long+1
	ldab	@long
	sbcb	2,x
	stab	@long
	;
	jmp	__addf32_54

;
;	compare: abs(tos) - abs(@long)
;
;	  TOS<@long:  return C=1 (BCS)
;	  TOS==@long: return Z=1 (BEQ)
;	  TOS>@long:  C=0,Z=0    (BHI)
;
__abscmp:
	ldaa	2,x
	anda	#$7f		; ignore sign bit
	ldab	@long
	andb	#$7f		; ignore sign bit
	sba
	bne	__abscmp_ret
	ldab	3,x
	subb	@long+1
	bne	__abscmp_ret
	ldab	4,x
	subb	@long+2
	bne	__abscmp_ret
	ldab	5,x
	subb	@long+3
__abscmp_ret:
	rts
;
;	check both Inf and NaN
;	2,x = TOS top
;
;	__sign: TOS and @long has different sign? same:b7=0, differ:b7=1
;
;	__zin and AccB:
;		b7	xor @long/TOS's sign. same as __sign
;		b6	(not used)
;		b5	TOS   is Zero
;		b4	@long is Zero
;		b3	TOS   is Inf?
;		b2	@long is Inf?
;		b1	TOS   is NaN?
;		b0	@long is NaN?
;
__setup_zin:
	ldab	2,x
	eorb	@long
	andb	#$80
	stab	__sign		; First, determine the sign
;
	inx
	inx
	pshb
	jsr	__f32iszerox	; TOS == 0.0?
	pulb
	bne	__setup_zin_10
	orab	#$20		; b5: TOS is 0.0
	bra	__setup_zin_50
__setup_zin_10:
	pshb
	jsr	__f32isNaNorInfx ; TOS == Inf or NaN?
	pulb
	bne	__setup_zin_20
	orab	#$08		; b3: TOS is Inf
__setup_zin_20:
	bcc	__setup_zin_50
	orab	#$02		; b1: TOS is NaN
;
__setup_zin_50:
	pshb
	jsr	__f32iszero	; @long == 0.0 ?
	pulb
	bne	__setup_zin_60
	orab	#$10		; b4: @long is 0.0
	bra	__setup_zin_99
__setup_zin_60:
	pshb
	jsr	__f32isNaNorInf	; @long == Inf or NaN
	pulb
	bne	__setup_zin_70
	orab	#$04		; b2: @long is Inf
__setup_zin_70:
	bcc	__setup_zin_99
	orab	#$01		; b0: @long is NaN
__setup_zin_99:
	stab	__zin
	rts
;
;	Change @long and (2-5,x) floating point number for easier calculations.
;	  Put the exponent in __lexp, __texp (1 byte, biased)
;	    If the biased exponent is 00 (subnormal), it becomes 01.
;	  Set a hidden bit for normal number (without subnormal).
;	  Shift the mantissa to the left by 8 bits
;	    and put 0 in the tail byte. // @long+3 and (5,x)
;
;	Note:   to use @long+0 or 2,x as the last byte is faster than 
;		to move the whole thing, but this is easier to understand.
;
;	Special numbers ( Inf, NaN ) cannot be handled here.
;
__setup_both:			; get both exp, set hidden bit
	jsr	__setup_long	; @long's exp->AccA, set hidden bit of @long
	jsr	__setup_tos	; TOS's   exp->AccA, set hidden bit of TOS
	jsr	__asl8_both
	ldab	__texp
	subb	__lexp		; AccB = TOS'exp - @long's exp
	stab	__expdiff	; save it
	rts			; (note: AccA has __texp from __setup_tos)
;
__setup_long:			; @long's exp->AccA, set hidden bit of @long
	ldab	@long+1		; get TOS's exp to a
	ldaa	@long
	clr	@long
	aslb
	rola
	sec			; set hidden bit of TOS
	bne	__setup_long_1	; subnormal number?
	inca
	clc			; if subnormal, hidden bit not set
__setup_long_1:
	rorb
	stab	@long+1
	staa	__lexp
	rts
;
__setup_tos:			; TOS's   exp->AccA, set hidden bit of TOS
	ldab	3,x		; get TOS's exp to a
	ldaa	2,x
	clr	2,x
	aslb
	rola
	sec			; set hidden bit of TOS
	bne	__setup_tos_1	; subnormal number?
	inca
	clc
__setup_tos_1:
	rorb
	stab	3,x
	staa	__texp
	rts
;
;	@long/TOS shift right 8bit
;	  messing up AccB
;
__lsr8_both:
	bsr	__lsr8_tos
;
;	@long shift right 8bit
;
__lsr8_long:
	ldab	@long+2
	stab	@long+3
	ldab	@long+1
	stab	@long+2
	ldab	@long
	stab	@long+1
	clr	@long
	rts
;
;	TOS:(2-5,x) shift right 8bit
;
__lsr8_tos:
	ldab	4,x
	stab	5,x
	ldab	3,x
	stab	4,x
	ldab	2,x
	stab	3,x
	clr	2,x
	rts
;
;	@long/TOS shift left 8bit
;	  messing up AccB
;
__asl8_both:
	bsr	__asl8_tos
__asl8_long:
	ldab	@long+1
	stab	@long
	ldab	@long+2
	stab	@long+1
	ldab	@long+3
	stab	@long+2
	clr	@long+3
	rts
;
__asl8_tos:
	ldab	3,x
	stab	2,x
	ldab	4,x
	stab	3,x
	ldab	5,x
	stab	4,x
	clr	5,x
	rts
;
;	shift right by AccB with G/R/S bit
;	mess AccA
;
;	TODO: If the shift>=24, This will result in unnecessary shifts.
;
;
__lsr_long:			; lsr @long by AccB
	ldaa	@long+3
__lsr_long_1:
	lsr	@long+0
	ror	@long+1
	ror	@long+2
	rora
	bita	#$3F		; check stick
	bne	__lsr_long_2
	oraa	#$20
__lsr_long_2:
	decb
	bne	__lsr_long_1
	anda	#$e0
	staa	@long+3
	rts
;
__lsr_tos:			; lsr TOS (2-5,x) by AccB
	ldaa	5,x
__lsr_tos_1:
	lsr	2,x
	ror	3,x
	ror	4,x
	rora
	bita	#$3F
	bne	__lsr_tos_2
	oraa	#$20
__lsr_tos_2:
	decb
	bne	__lsr_tos_1
	anda	#$e0
	staa	5,x
	rts
;
;	shift left by AccB
;
__asl_long:		; asl @long by AccB
	asl	@long+3
	rol	@long+2
	rol	@long+1
	rol	@long+0
	decb
	bne	__lsr_long
	rts
__asl_tos:		; asl TOS (2-5,x) by AccB
	asl	5,x
	rol	4,x
	rol	3,x
	rol	2,x
	decb
	bne	__lsr_tos
	rts
;
;
__pullret:
	tsx
	ldx	0,x
	ins
	ins
	ins
	ins
	ins
	ins
	jmp	0,x
;
;	@long = @long * TOS
;
;	No arithmetic is required when multiplying by 1, but simply multiply it now.
;
__mulf32tos:
	tsx
	jsr	__setup_zin	; TOS & @long is zero/Inf/NaN?
	;
;	ldab	__zin
	bitb	#$3F		;
	beq	__mulf32tos4	; No, nomal calculation
;
	bitb	#$03		; TOS or @long is NaN?
	jne	__f32retNaN	; Yes: return NaN
	andb	#$0C		; TOS or @long is Inf?
	beq	__mulf32_s10
	ldab	__zin
	andb	#$30		; TOS or @long is zero?
	jeq	__f32retInfs	; No, Inf * (not zero) returns Inf with __sign.
	jmp	__f32retNaN	; Inf*0.0 returns NaN
;
__mulf32_s10:			; TOS and @long is not Inf,NaN
	ldab	__zin
	andb	#$30
	jne	__f32retZeros	; TOS or @long is zero
;
__mulf32tos4:
	tsx
	inx
	inx
	jsr	__adj_subnormal	; if subnormal, adjust it. exp into AccAB.
	stab	__exp2+1
	staa	__exp2
;
	ldx	#long
	jsr	__adj_subnormal
	addb	__exp2+1
	adca	__exp2
	stab	__exp2+1
	staa	__exp2
;
	subb	#<128		; sum of exp>127? (>=128)
	sbca	#>128
	jge	__f32retInfs	; Overflow, returns Inf with __sign.
;
	ldab	__exp2+1
	ldaa	__exp2
	subb	#<-149		; sum of exp < -149?
	sbca	#>-149
	jlt	__f32retZeros	; Underflow, return zero with __sign.
;
__mulf32tos03:
;
	clr	__work+5	; clear working area 48bit
	clr	__work+4
	clr	__work+3
	clr	__work+2
	clr	__work+1
	clr	__work
;
	tsx
	ldab	#24		; loop count
	stab	@tmp3
	bra	__mulf32tos31
;
__mulf32tos30:
	asl	__work+5
	rol	__work+4
	rol	__work+3
	rol	__work+2
	rol	__work+1
	rol	__work
__mulf32tos31:
	asl	5,x
	rol	4,x
	rol	3,x
	bcc	__mulf32tos32
	ldab	@long+3
	addb	__work+5
	stab	__work+5
	ldab	@long+2
	adcb	__work+4
	stab	__work+4
	ldab	@long+1
	adcb	__work+3
	stab	__work+3
	bcc	__mulf32tos32
	inc	__work+2
	bne	__mulf32tos32
	inc	__work+1
	bne	__mulf32tos32
	inc	__work
__mulf32tos32:
	dec	@tmp3
	bne	__mulf32tos30
;
__mulf32tos65:			; end of mant*mant multiply
	ldab	__exp2+1
	ldaa	__exp2
	tst	__work		; carryover of the MSB bit?
	bpl	__mulf32tos70
__mulf32tos69:
	addb	#1
	adca	#0
	stab	__exp2+1
	staa	__exp2
	bra	__mulf32tos71
__mulf32tos70:
	asl	__work+5
	rol	__work+4
	rol	__work+3
	rol	__work+2
	rol	__work+1
	rol	__work
__mulf32tos71:
;
	subb	#<128		; sum of exp>127? (>=128?)
	sbca	#>128
	jge	__f32retInfs	; Overflow, returns Inf with __sign.
;
	ldab	__exp2+1
	ldaa	__exp2
	subb	#<-149		; sum of exp < -149?
	sbca	#>-149
	jlt	__f32retZeros	; Underflow, return zero with __sign.
;
;	even number rounding
;
	ldab	__work+3
	bpl	__mulf32tos72	; G=0, do nothing
	andb	#$3F		; make sticky bit
	pshb
	orab	__work+4
	orab	__work+5
	pulb
	beq	__mulf32tos721
	orab	#$20
__mulf32tos721:
	pshb
	ldab	__work+2	; check LSB
	lsrb			; LSB -> Carry
	pulb
	rorb			; b7:LSB, b6:G, b5:R, b4:S
	bitb	#$40		; b6:G==0?
	beq	__mulf32tos72	;   Yes, do nothng
	andb	#$F0
	cmpb	#$40		; 0100:only G is 1?
	beq	__mulf32tos72	;   Yes, do nothng
;
	inc	__work+2		; round up
	bne	__mulf32tos72
	inc	__work+1
	bne	__mulf32tos72
	inc	__work+0
	bne	__mulf32tos72
;
	inca
	cmpa	#$FF
	jeq	__f32retInfs
	lsr	__work+0
	ror	__work+1
	ror	__work+2
;
__mulf32tos72:
	ldab	__work+3
	orab	__work+2
	orab	__work+1
	orab	__work
	jeq	__f32retZeros	; The mantissa is all 0, so the value is 0.
;
	ldab	__exp2+1
	ldaa	__exp2
	subb	#<-126
	sbca	#>-126
	jge	__mulf32tos74
	;			; subnormal
__mulf32tos73:
	lsr	__work		; 1bit shift right
	ror	__work+1
	ror	__work+2
	incb
	bne	__mulf32tos73
	ldab	#-127
	stab	__exp2+1
__mulf32tos74:
	ldab	__work+2
	stab	@long+3
	ldab	__work+1
	stab	@long+2
	ldab	__work
	ldaa	__exp2+1
	adda	#127
	aslb
	lsra
	rorb
	stab	@long+1
	oraa	__sign
	staa	@long
	jmp	__pullret
;
;	@long = @long / TOS
;
__divf32tos:
	tsx
	jsr	__setup_zin	; TOS & @long is zero/Inf/NaN?
;	ldab	__zin
;
	bitb	#$3F
	beq	__divf32tos01	; No, normal calculation
;
	bitb	#$03
	jne	__f32retNaN	; TOS or @long is NaN, return NaN
;
	andb	#$0C		; TOS or @long is Inf?
	beq	__divf32_s20
	cmpb	#$0C		; TOS and @long is Inf?
	jeq	__f32retNaN	; Yes, Inf/Inf returns NaN
;
	ldab	__zin
	bitb	#$30		; TOS or @long is 0.0 ?
	jeq	__divf32_s10
	;			; One is Inf and the other is 0.0
	bitb	#$20		; TOS==0.0?
	jeq	__f32retNaN	; Yes, Inf/0.0 returns NaN
	jmp	__f32retZeros	; 0.0/Inf return 0.0 with __sign
;
__divf32_s10:			; TOS or @long is Inf and finite numbers 
	ldab	__zin
	bitb	#$08		; TOS == Inf?
	jne	__f32retZeros	; num/Inf returns 0.0 with __sign
	jmp	__f32retInfs	; Inf/num returns Inf with __sign
;
__divf32_s20:
	ldab	__zin
	andb	#$30
	beq	__divf32tos01
	cmpb	#$30		; 0.0/0.0?
	jeq	__f32retNaN
	cmpb	#$20		; num/0.0?
	jeq     __f32retInfs	; Yes, returns Inf with __sign
	cmpb	#$10		; 0.0/num?
	jeq	__f32retZeros	; Yes, returns 0.0 with __sign
;	
__divf32tos01:
;
	tsx
	inx
	inx
	jsr	__adj_subnormal	; do normalize,AccAB = unbiased exp
	stab	__expdiff+1
	staa	__expdiff
	ldx	#long
	jsr	__adj_subnormal
	subb	__expdiff+1
	sbca	__expdiff
	stab	__expdiff+1	; expdiff = long's exp - TOS's exp
	staa	__expdiff
;
	subb	#<128
	sbca	#>128
	jge	__f32retInfs	; overflow
;
	ldab	__expdiff+1
	ldaa	__expdiff
	subb	#<-149
	sbca	#>-149
	jlt	__f32retZeros	; underflow (can't expressed even in subnormal)
;
	tsx
        jsr     __asl8_both     
;
;	Since division 24bit is done in 32-bit,
;	  the result will never be 0 (Dividend 0 is already excluded)
;
__divf32tos03:
	jsr	__fdiv32x32	; @tmp3:@tmp3+1:@tmp4:@tmp4+1 = @long / TOS
	ldab	__expdiff+1
	ldaa	__expdiff
	tst	@tmp3
	bmi	__divf32tos04		; MSB==1 needn't shitft
;
__divf32_0301:
	subb	#1			; exp++
	sbca	#0
	asl	@tmp4+1
	rol	@tmp4
	rol	@tmp3+1
	rol	@tmp3
	bpl	__divf32tos20
	stab	__expdiff+1
	staa	__expdiff
;
__divf32tos04:
	subb	#<-126			; subnormal?
	sbca	#>-126
	jge	__divf32tos20		; no, it's normal number
;
__divf32tos05:
	lsr	@tmp3
	ror	@tmp3+1
	ror	@tmp4
	ror	@tmp4+1
	incb
	bne	__divf32tos05
;
	ldab	#<-127			; subnormal's exp
	ldaa	#>-127
;					; round up check (subnormal)
	bsr	__divf32_rup_check	; if C==1, need round up
	bcc	__divf32_done
;	
	inc	@tmp4
	bne	__divf32_done
	inc	@tmp3+1
	bne	__divf32_done
	inc	@tmp3
	bpl	__divf32_done		; Still subnormal
;
;	annoying thing here is:
;	  round up carry from the subnormal results in a normal number.
;
	ldab	#<-126
	ldaa	#>-126
	stab	__expdiff+1
	staa	__expdiff
	bra	__divf32_done
;
__divf32tos20:				; round up check (normal)
	ldab	__expdiff+1
	ldaa	__expdiff
	bsr	__divf32_rup_check	; C==1, need round up
	bcc	__divf32_done
;
__divf32_rup:
	inc	@tmp4
	bne	__divf32_done
	inc	@tmp3+1
	bne	__divf32_done
	inc	@tmp3
	bne	__divf32_done
;
	lsr	@tmp3
	ror	@tmp3+1
	ror	@tmp4
	addb	#1
	adca	#0
;
__divf32_done:
	addb	#127
	tba
	ldab	@tmp4
	stab	@long+3
	ldab	@tmp3+1
	stab	@long+2
	ldab	@tmp3
	aslb
	lsra
	rorb
	stab	@long+1
	oraa	__sign
	staa	@long
	jmp	__pullret
;
;	round up check, @tmp4 and @tmp4+1
;
__divf32_rup_check:
	pshb
	ldab	@tmp4+1	
	bpl	__divf32_rup_none	; G==0, no round up
	aslb				; (bitb #$7F)
	bne	__divf32_rup_occur	; R or S==1, do round up
	ldab	@tmp4
	asrb
	pulb
	rts				; if LSB==0, no round up
__divf32_rup_none:
	pulb
	clc
	rts
__divf32_rup_occur:
	pulb
	sec
	rts
;
;	@tmp3:@tmp4		= @long / TOS
;	@tmp1:AccA:AccB		= @long % TOS
;	@tmp2: loop counter
;	mess @long
;
__fdiv32x32:
	ldab #32
	stab @tmp2	; loop counter
	ldab @long
	stab @tmp1+1
	ldab @long+2
	ldaa @long+1
	clr  @tmp1
loop:
	asl  @tmp4+1
	rol  @tmp4
	rol  @tmp3+1
	rol  @tmp3
	subb 4,x
	sbca 3,x
	pshb
	psha
	ldab @tmp1+1
	sbcb 2,x
	ldaa @tmp1
	sbca #0
	bcs  skip
	stab @tmp1+1	; subtracted.
	staa @tmp1
	pula
	pulb
	inc @tmp4+1	; set the lower bit of the quotient
	bra next
skip:
	pula
	pulb		; can't substract. pull it back.
	addb 4,x
	adca 3,x
next:
	aslb
	rola
	rol @tmp1+1
	rol @tmp1
	dec @tmp2
	bne loop
	rts
;
;	@long cmp TOS
;
;	condition:	return AccAB and carry
;	C=1		unordered relation (NaN)
;	C=0:
;	  @long<TOS	-1
;	  @long==TOS	0
;	  @long>TOS	1
;
;
__cmpf32tos:
	jsr	__f32isNaNorInf
	jcs	__pullret
	tsx
	inx
	inx
	jsr	__f32isNaNorInfx
	jcs	__pullret
;
        jsr     __f32iszero	; @long == 0.0 ?
	bne	__cmpf32tos_10	; branch if @long!=0.0
	tsx
	inx
	inx
        jsr     __f32iszerox
	bne	__cmpf32tos_10
	clrb			; ±0.0==±0.0? return eq
	clra
	jmp	__pullret
__cmpf32tos_10:
	tsx
	bsr	__cmpf32x
	jmp	__pullret
__cmpf32x:			; TODO: name?
	ldab	@long
	cmpb	2,x
	bgt	__cmpf32x_gt
	blt	__cmpf32x_lt
	ldab	@long+1
	cmpb	3,x
	bhi	__cmpf32x_gt
	bcs	__cmpf32x_lt
	ldab	@long+2
	cmpb	4,x
	bhi	__cmpf32x_gt
	bcs	__cmpf32x_lt
	ldab	@long+3
	cmpb	5,x
	bhi	__cmpf32x_gt
	bcs	__cmpf32x_lt
__cmpf32x_eq:			; @long == TOS
	clrb
	clra			; Z=1,C=0
	rts
__cmpf32x_gt:			; @long > TOS
	clra			; C=0
	ldab	#1		; Z=0
	rts
__cmpf32x_lt:			; @long < TOS
	clrb			; C=0
	decb			; Z=0
	tba
	rts
;
;	if float is subnormal, mantissa into normal form.
;	unbiased exp is returned in AccAB.
;	bit 23 turn on (| 0x00800000)
;	parameter:
;	  (0,x) - (3,x): subnormal float
;
__adj_subnormal:
	ldaa	1,x
	ldab	0,x
	asla
	rolb		; get exp in b
	bne	__adj_subn_ret
	ldab	#<-126	; least minimum nomal number
	ldaa	#>-126
__adj_subn_01:
	subb	#1
	sbca	#0
	asl	3,x
	rol	2,x
	rol	1,x
	bpl	__adj_subn_01
	rts
__adj_subn_ret:
	ldaa	1,x
	ora	#$80
	staa	1,x
	clra
	subb	#127	; un bias
	sbca	#0
	rts
