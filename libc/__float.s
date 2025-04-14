;
;	MC6800 floating point arithmetic library
;
;	Copyright (c) 2025 by ZUKERAN, shin
;	Released under the MIT license
;
;	https://github.com/zu2/chibicc-6800-v1?tab=License-1-ov-file#readme
;


;
;	Note: his program was created for testing chibicc-6800-v1, 
;	and does not pay attention to speed, accuracy, or exception handling.
;

;
;	TODO:
;		more test
;		refactoring
;		Signed zero
;		Subnormal numbers (Gradual Underflow)
;		Infinities
;		NaNs
;		Exception handling
;		Calculation precision
;		Speed up
;

	.zp
	.data
	.export	__i16tof32
	.export	__i32tof32
	.export	__f32tou32
	.export	__f32toi32
	.export	__f32tou16
	.export	__addf32tos
	.export __subf32tos
	.export __mulf32tos
	.export __divf32tos
	.export __cmpf32tos
	.export __f32iszero
;	.export __f32iszerox
;	.export __f32isNaNorInf
;	.export __f32isNaNorInfx
	.export __fdiv32x32
	.data
	.export __sign
	.export __texp
	.export __lexp
	.export __expdiff
__zin:	.byte	0	; TOS & @long are Zero? Inf? NaN?
__sign:	.byte	0	; sign (TOS & @long sign are different? 1:differ,0:same)
__texp:	.byte	0	; TOS's exp
__lexp:	.byte	0	; @long's exp
__expdiff:
	.word	0	; texp - lexp
__work: .word	0	; working area
	.word	0
	.code
;
;	(0-3,x) is NaN? (exp==255)
;
__f32isNaNorInf:
	ldx	#long
__f32isNaNorInfx:
	ldab	1,x
	ldaa	0,x
	aslb
	rola
	adda	#1		; Use add to make the C=0. if usinh inc, C is result of rola
	bne	__f32isNaN_1
	;		; exp == 255, check mantissa
	tstb
	bne	__f32isNaN_2
	tst	2,x
	bne	__f32isNaN_2
	tst	3,x
	bne	__f32isNaN_2
	tst	4,x
	bne	__f32isNaN_2
__f32isNaN_1:			; Z=0, C=0 not NaN,Inf
	rts			; Z=1, C=0 Inf
__f32isNaN_2:
	sec
	rts			; Z=0, C=1 NaN
;
;	(0-3,x) == 0.0 ?
;		== 0x0000 0000 or 0x8000 0000	
;	if 0.0 then Z=1 else Z=0
;
__f32iszero:
	ldx	#long
__f32iszerox:
	ldab	0,x		; exponent on the MSB side, to check from the top is faster
	bitb	#$7F
	bne	iszerox_ret
	tst	1,x
	bne	iszerox_ret
	tst	2,x
	bne	iszerox_ret
	tst	3,x
iszerox_ret:
	rts
;
;	negate 32bit long
;
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
	clr	@long+3
	stab	@long+2
	staa	@long+1
	ldab	#$8f		; exp ($8e+1), if i32>=32768 then exp=$8E
__i16tof32_20:			; Shift left until the most significant bit becomes 1
	decb
	asl	@long+2
	rol	@long+1
	bcc	__i16tof32_20	; loop until C=1 (hidden bit check)
__i16tof32_21:
	lsrb
	ror	@long+1		; shifted one more bit, return 1bit right and set exp's LSB
	ror	@long+2
	orab	__sign
	stab	@long
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
	ldab	0,x
	bne	__i32tof32_1
	ldab	1,x
	bne	__i32tof32_1
	ldab	2,x
	bne	__i32tof32_1
	ldab	3,x
	jeq	__f32zero	;   return +0.0
__i32tof32_1:
	ldab	0,x
	andb	#$80
	stab	__sign		; b7:sign bit
	bpl	__i32tof32_2
	jsr	__negi32x	; negate @long
__i32tof32_2:
	ldaa	0,x
	beq	__i32tof32_left	; need left shift
;
	ldab	#$96		; exp.
__i32tof32_right:		; right shift is required until the MSB byte becomes 0
	incb
	lsra
	ror	1,x
	ror	2,x
	ror	3,x
	tsta
	bne	__i32tof32_right
__i32tof32_done:
	asl	1,x		; clear hidden bit and set exp's LSB
	lsrb
	ror	1,x
	orab	__sign
	stab	0,x
	rts
__i32tof32_left:		; left shift is required until hidden bit==1
	ldab	#$96
	tst	1,x		; hidden bit set?
	bmi	__i32tof32_done
__i32tof32_left2:
	decb
	asl	3,x
	rol	2,x
	rol	1,x
	bpl	__i32tof32_left2
	bra	__i32tof32_done
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
;	float to signed long
;		@long -> @long
;	
__f32toi32:
	ldx	#long
	jsr	__f32iszerox
	jeq	__u32zero
	ldab	0,x
	stab	__sign		; save sign
	ldaa	1,x
	asla
	rolb			; B = exp
	cmpb	#$3f		; if exp<=$3e (x < 0.5) then return 0;
	bcc	__f32toi32_1
	jmp	__u32zero
__f32toi32_1:
	cmpb	#$9f		; if exp>=$9f (x >= 4,294,967,295)
	bcs	__f32toi32_2
	tst	__sign
	jpl	__i327fffffff
	jmp	__i3280000000	; return 4,294,967,295
__f32toi32_2:
	clr	0,x
	ldaa	1,x		; recover hidden bit
	ora	#$80
	staa	1,x
	subb	#$96		; TODO:
	beq	__f32toi32_ret
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
	bpl	__f32toi32_ret2
	com	0,x
	com	1,x
	com	2,x
	neg	3,x
	bne	__f32toi32_ret2
	inc	2,x
	bne	__f32toi32_ret2
	inc	1,x
	bne	__f32toi32_ret2
	inc	0,x
__f32toi32_ret2:
	rts
__f32toi32_4:
	lsr	0,x
	ror	1,x
	ror	2,x
	ror	3,x
	incb
	bne	__f32toi32_4
	bra	__f32toi32_ret
;
;	float to unsigned int
;		@long -> @long
;	
__f32tou16:
	ldx	#long
	jsr	__f32iszerox
	beq	__u16zero
	ldab	0,x
	bmi	__u16zero	; if x<0 then return 0
	ldaa	1,x
	asla
	rolb			; B = exp
	cmpb	#$3f		; if exp<=$3e (x < 0.5) then return 0;
	bcc	__f32tou16_1
	jmp	__u16zero
__f32tou16_1:
	cmpb	#$8f		; if exp>=$8f (x >= 65536)
	bcs	__f32tou16_2
	jmp	__u16ffff	; return 65535
__f32tou16_2:
	clr	0,x
	ldaa	1,x		; recover hidden bit
	ora	#$80
	staa	1,x
	subb	#$8e		; TODO:
	beq	__f32tou16_ret
	bcs	__f32tou16_4
__f32tou16_3:
	asl	3,x
	rol	2,x
	rol	1,x
	rol	0,x
	decb
	bne	__f32tou16_3
__f32tou16_ret:
	ldab	2,x
	ldaa	1,x
	rts
__f32tou16_4:
	lsr	0,x
	ror	1,x
	ror	2,x
	ror	3,x
	incb
	bne	__f32tou16_4
	bra	__f32tou16_ret
__u16zero:
	clrb
	clra
	rts
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
	jsr	__is_zin	; TOS & @long is zero/Inf/NaN?
;	ldab	__zin
	andb	#$03		; TOS or @long is NaN?
	jne	__f32retNaN	; Yes: return NaN
;
	ldab	__zin
	andb	#$0C		; TOS or @long is Inf?
	jeq	__addftos_s20
	cmpb	#$0C		; TOS and @long are Inf?
	bne	__addftos_s10
	ldab	__sign		; each sign are same?
	jmi	__f32retNaN	; No: return NaN
__addftos_s05:
	ldab	@long		; return Inf, sign is the same as @long
	jmp	__f32retInf
	;
__addftos_s10:
	ldab	__zin		; Either TOS or @long is Inf.
	cmpb	#$04		; @long is Inf?
	bne	__addftos_s05	; No, return Inf, sign is same as @long
	tsx
	ldab	2,x
	jmp	__f32retInf	; return Inf, The sign is the same as TOS
;
__addftos_s20:			; TOS and @long is not NaN,Inf.
	ldab	__zin
	andb	#$30		; TOS or @long == 0.0?
	beq	__addf32tos1	; No
	cmpb	#$30		; TOS and @long == 0.0?
	jne	__addftos_s50
	tst	__sign		; Yes. same sign?
	jne	__f32retpZero	; Not same sign. return +0.0
	jmp	__f32retZerol	; return 0.0, sign is same as @long
;
__addftos_s50:			; TOS or @long == 0.0
	cmpb	#$20		; TOS == 0.0?
	beq	__addftos_s51	; Yes, return @long (do nothing)
	tsx			; No,  return TOS
	inx
	inx
	jsr	__load32x	; @long <= (0-3,x)
__addftos_s51:
	jmp	__pullret
;
__addf32tos1:			; neither of @long and TOS was not 0.0, simply add them.
	tsx
	ldab	2,x		; get MSB of TOS
	eorb	@long
	andb	#$80		; sign equal?
	jne	__addf32tos50
	;			; sign equal, add it
	ldab	@long		; save sign
	andb	#$80
	stab	__sign
	jsr	__setup_both	; get both exp to texp, lexp. AccB = texp - lexp
	bne	__addf32tos9
	;
	bra	__addf32tos11	; texp==lexp, Simply add it.
__addf32tos9:
	bcc	__addf32tos10
	;			; TOS < @long
	negb
	jsr	__lsr_tos	;   shift TOS right by AccB, align the bit.
	ldaa	__lexp
	jmp	__addf32tos11
__addf32tos10:			; TOS > @long
	jsr	__lsr_long	;   shift @long right by AccB, align the bit
	ldaa	__texp
__addf32tos11:
	ldab	@long+3		; @long = @long + TOS
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
	bcc	__addf32tos20	; over flow?
        ror     @long		; shift one bit
        ror     @long+1		; shift one bit
        ror     @long+2
        ror     @long+3
	inca			; exp++
__addf32tos20:			; even number rounding
	ldab	@long+3		; check guard bit
	bpl	__addf32tos29	;   G=0, do nothing
	andb	#$7f		; check sticky
	bne	__addf32tos21
	ldab	@long+2		; check LSB bit
	asrb
	bcc	__addf32tos29
__addf32tos21:
	inc	@long+2		; inc mant
	bne	__addf32tos29
	inc	@long+1
	bne	__addf32tos29
	inc	@long+0
	bne	__addf32tos29
	;			; over flow
        ror     @long		; shift one bit
        ror     @long+1		; shift one bit
        ror     @long+2
        ror     @long+3
	inca			; exp++
__addf32tos29:
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
__addf32tos50:
	jsr	__abscmp	; compare: abs(tos) - abs(@long)
	bne	__addf32tos51	; different sign, but value equal
	jsr	__f32zero	;   return +0.0
	jmp	__pullret
__addf32tos51:
	bcs	__addf32tos60	; jump if TOS<@long
	;
	ldab	2,x		; abs(TOS) > abs(@long), result sign=TOS's sign
	andb	#$80
	stab	__sign
	jsr	__setup_both	; AccB = TOS'exp - @long's exp
	beq	__addf32tos52	; AccB ==0,  simply substract
	jsr	__lsr_long	; shift @long right by AccB
__addf32tos52:
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
	;
__addf32tos54:
	bmi	__addf32tos542
__addf32tos541:
	deca			; while hidden bit ==0 do asl @long, AccA--
	beq	__addf32tos542	; subnormal number
	asl	@long+3
	rol	@long+2
	rol	@long+1
	rol	@long+0
	bpl	__addf32tos541
__addf32tos542:
;
;	TODO: This part is not strictly correct. fix it later.
;
	ldab	@long+3		; check guard bit
	bpl	__addf32tos55	;   G=0, do nothing
	andb	#$7f		; check sticky
	bne	__addf32tos543
	ldab	@long+2		; check LSB bit
	asrb
	bcc	__addf32tos55
;
__addf32tos543:
	inc	@long+2		; inc mant
	bne	__addf32tos55
	inc	@long+1
	bne	__addf32tos55
	inc	@long+0
	bne	__addf32tos55
	swi			; TODO:
__addf32tos55:
	jsr	__lsr8_long
	asl	@long+1		; exp's LSB into @long+1
	lsra
	ror	@long+1
	ora	__sign		; recover sign bit
	staa	@long
	jmp	__pullret
__addf32tos60:
	;			; abs(TOS) < abs(@long), result sign=@long's sign
	ldab	@long
	andb	#$80
	stab	__sign
	jsr	__setup_both	; AccB = TOS'exp - @long's exp
	beq	__addf32tos61	; AccB == 0, simply substract
	negb
	jsr	__lsr_tos	; shift TOS right by AccB
__addf32tos61:
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
	jmp	__addf32tos54
;
;
;
__abscmp:	; compare: abs(tos) - abs(@long)
	ldaa	2,x
	anda	#$7f
	ldab	@long
	andb	#$7f
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
	rts	; TOS<@long: C=1 (BCS), TOS==@long: Z=1 (BEQ), other TOS>@long: C=0,Z=0 (BHI)
;
;	check both Inf and NaN
;	2,x = TOS top
;
;	__sign: TOS and @long has different sign? same:b7=0, differ:b7=1
;
;	__zin and AccB:
;		b7	TOS's   sign
;		b6	@long's sign
;		b5	TOS   is Zero
;		b4	@long is Zero
;		b3	TOS   is Inf?
;		b2	@long is Inf?
;		b1	TOS   is NaN?
;		b0	@long is NaN?
;
__is_zin:
	ldab	2,x
	eorb	@long
	andb	#$80
	stab	__sign		; First, determine the sign
;
	ldab	2,x
	aslb
	ldab	@long
	rorb			; bit7: TOS's sign, bit6: @long's sign
	andb	#$C0		; mask 1100 0000
;
	pshb
	jsr	__f32iszerox	; TOS == 0.0?
	pulb
	bne	__is_zin_10
	orab	#$20		; b5: TOS is 0.0
	bra	__is_zin_50
__is_zin_10:
	pshb
	jsr	__f32isNaNorInfx ; TOS == Inf or NaN?
	pulb
	bne	__is_zin_20
	orab	#$08		; b3: TOS is Inf
__is_zin_20:
	bcc	__is_zin_50
	orab	#$02		; b1: TOS is NaN
;
__is_zin_50:
	pshb
	jsr	__f32iszero	; @long == 0.0 ?
	pulb
	bne	__is_zin_60
	orab	#$10		; b4: @long is 0.0
	bra	__is_zin_99
__is_zin_60:
	pshb
	jsr	__f32isNaNorInf	; @long == Inf or NaN
	pulb
	bne	__is_zin_70
	orab	#$04		; b2: @long is Inf
__is_zin_70:
	bcc	__is_zin_99
	orab	#$01		; b0: @long is NaN
__is_zin_99:
	stab	__zin
	rts
;
;
__setup_both:			; get both exp, set hidden bit
	jsr	__setup_long	; @long's exp->AccA, set hidden bit of @long
	jsr	__setup_tos	; TOS's   exp->AccA, set hidden bit of TOS
	jsr	__asl8_both
	ldab	__texp
	subb	__lexp		; AccB = TOS'exp - @long's exp
	rts
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
	clc
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
;
;
;
;	@long shift right 8bit
;
__lsr8_both:
	bsr	__lsr8_tos
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
;	shift right by AccB
;
__lsr_long:		; lsr @long by AccB
	lsr	@long+0
	ror	@long+1
	ror	@long+2
	ror	@long+3
	decb
	bne	__lsr_long
	rts
__lsr_tos:		; lsr TOS (2-5,x) by AccB
	lsr	2,x
	ror	3,x
	ror	4,x
	ror	5,x
	decb
	bne	__lsr_tos
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
__normalize_done:
	rts
;
__normalize_long:		; if hidden bit==0, lsr @long, AccA--
	tst	@long+3
	bne	__normalize_2
	tst	@long+2
	bne	__normalize_2
	tst	@long+1
	beq	__normalize_done	; all 0, return
__normalize_2:
	tst	@long+1		; hidden bit set?	TODO: 1bit lost...
	bmi	__normalize_done
	asl	@long+3
	rol	@long+2
	rol	@long+1
	deca
	bra	__normalize_2
__normalize_tos:		; if hidden bit==0, lsr @long, AccA--
	tst	5,x
	bne	__normalize_3
	tst	4,x
	bne	__normalize_3
	tst	3,x
	beq	__normalize_done	; all 0, return
__normalize_3:
	tst	3,x		; hidden bit set?	TODO: 1bit lost...
	bmi	__normalize_done
	asl	5,x
	rol	4,x
	rol	3,x
	deca
	bra	__normalize_3
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
	jsr	__is_zin	; TOS & @long is zero/Inf/NaN?
	;
;	ldab	__zin
	andb	#$03		; TOS or @long is NaN?
	jne	__f32retNaN	; Yes: return NaN
	ldab	__zin
	andb	#$0C		; TOS or @long is Inf?
	beq	__mulf32_s10
	ldab	__zin
	andb	#$30		; TOS or @long is zero?
	jeq	__f32retInf	; No, Inf x (not zero) returns Inf. 
	jmp	__f32retNaN
__mulf32_s10:			; TOS and @long is not Inf,NaN
	ldab	__zin
	andb	#$30
	jne	__f32retZeros	; TOS or @long is zero
;
__mulf32tos4:
	tsx
	jsr	__setup_both	; mant: 2-4,x and @long to @long+2
	clra
	ldab	__lexp
	addb	__texp
	adca	#0
	subb	#127
	sbca	#0
	; 			; TODO:	exception check
	stab	__expdiff+1	; saveit
	staa	__expdiff
;
	beq	__mulf32tos03
	bpl	__mulf32tos02
	cmpb	#$E8		; underflow
	bcc	__mulf32tos03
	jmp	__f32retZeros
__mulf32tos02:			; overflow
	jmp	__f32retInf
;
__mulf32tos03:
;
	clr	@tmp2+3		; clear working area 32bit
	clr	@tmp2+2
	clr	@tmp2+1
	clr	@tmp2
;	clr	__work+3	; clear working area 32bit for carry
	clr	__work+2
	clr	__work+1
	clr	__work
;
	ldab	@long+1
	ldaa	4,x
	jsr	__mul8x8
;	adda	@tmp2+3		; Just after clearing, so @tmp2+3==0
	staa	@tmp2+3
;
	ldab	@long+2
	ldaa	3,x
	jsr	__mul8x8
	adda	@tmp2+3
	staa	@tmp2+3
	bcc	__mulf32tos30
	inc	__work+2
;
__mulf32tos30:
	ldab	@long
	ldaa	4,x
	jsr	__mul8x8
	addb	@tmp2+3
	stab	@tmp2+3
	adca	@tmp2+2
	staa	@tmp2+2
	bcc	__mulf32tos31
	inc	__work+1
;
__mulf32tos31:
	ldab	@long+1
	ldaa	3,x
	jsr	__mul8x8
	addb	@tmp2+3
	stab	@tmp2+3
	adca	@tmp2+2
	staa	@tmp2+2
	bcc	__mulf32tos32
	inc	__work+1
;
__mulf32tos32:
	ldab	@long+2
	ldaa	2,x
	jsr	__mul8x8
	addb	@tmp2+3
	stab	@tmp2+3
	adca	@tmp2+2
	staa	@tmp2+2
	bcc	__mulf32tos40
	inc	__work+1
;
__mulf32tos40:
	ldab	@long
	ldaa	3,x
	jsr	__mul8x8
	addb	@tmp2+2
	stab	@tmp2+2
	adca	@tmp2+1
	staa	@tmp2+1
	bcc	__mulf32tos41
	inc	__work
;
__mulf32tos41:
;
	ldab	@long+1
	ldaa	2,x
	jsr	__mul8x8
	addb	@tmp2+2
	stab	@tmp2+2
	adca	@tmp2+1
	staa	@tmp2+1
	bcc	__mulf32tos50
	inc	__work

__mulf32tos50:
	ldab	@long
	ldaa	2,x
	jsr	__mul8x8
	addb	@tmp2+1
	stab	@tmp2+1		; TODO:
	adca	@tmp2
	staa	@tmp2
	bcc	__mulf32tos60
	swi	;  TODO: overflow check needed???
__mulf32tos60:
	ldab	__work+2
	addb	@tmp2+2
	stab	@tmp2+2
	ldab	__work+1
	adcb	@tmp2+1
	stab	@tmp2+1
	ldab	__work
	adcb	@tmp2
	stab	@tmp2
	bcc	__mulf32tos65
	swi	; TODO: overflow check needed???
__mulf32tos65:
	ldaa	__expdiff+1
	tst	__expdiff
	bmi	__mulf32tos69	; subnormal number
	jne	__f32retInf	; overflow
	cmpa	#$FF
	jeq	__f32retInf
	bra	__mulf32tos70
	;
__mulf32tos69:
	lsr	@tmp2
	ror	@tmp2+1
	ror	@tmp2+2
	ror	@tmp2+3
	inca
	bne	__mulf32tos69
	bra	__mulf32tos80
;
__mulf32tos70:
	tsta
	beq	__mulf32tos80	; subnormal number
	clrb
	tst	@tmp2		; hidden bit set?
	bpl	__mulf32tos72
	inca
	jeq	__f32retInf
	cmpa	#$ff
	jeq	__f32retInf
	bra	__mulf32tos80
__mulf32tos72:
	ldab	@tmp2+3
	orab	@tmp2+2
	orab	@tmp2+1
	orab	@tmp2
	bne	__mulf32tos73
;	;			; The mantissa is all 0, so the value is 0.
	clra
	bra	__mulf32tos90
__mulf32tos73:
	asl	@tmp2+3
	rol	@tmp2+2
	rol	@tmp2+1
	rol	@tmp2
	bpl	__mulf32tos72
;
__mulf32tos80:
	;	TODO: round
__mulf32tos90:
	ldab	@tmp2+2
	stab	@long+3
	ldab	@tmp2+1
	stab	@long+2
	ldab	@tmp2
	aslb
	lsra
	rorb
	stab	@long+1
	oraa	__sign
	staa	@long
	jmp	__pullret
;
;	Multiply 8x8
;		@tmp1:	work
;		@tmp1+1:loop counter
;
__mul8x8:
	tsta
	beq	__mul8x8_zero
	stab	@tmp1
	beq	__mul8x8_zero
	ldab	#8
	stab	@tmp1+1
	clrb
__mul8x8_1:
	aslb
	rola
	bcc	__mul8x8_2
	addb	@tmp1
	adca	#0
__mul8x8_2:
	dec	@tmp1+1
	bne	__mul8x8_1
	rts
__mul8x8_zero:
	clrb
	clra
	rts
;
;	@long = @long / TOS
;
__divf32tos:
	tsx
	ldab	2,x
	eorb	@long
	andb	#$80
	stab	__sign		; First, determine the sign
;
	tsx
	inx
	inx
	jsr	__f32iszerox	; TOS == 0.0 ?
	bne	__divf32tos01	; no, jump to normal operation
	;			; TOS = 0.0
	jsr	__f32iszero	; @long == 0.0 ?
	jne	__f32retInf	; no, returns Inf
	jmp	__f32retNaN	; otherwise ( 0.0 / 0.0 ), returns NaN
__divf32tos01:
	jsr	__f32iszero	; @long == 0.0 ?
	jeq	__f32retZeros	; 0.0 / any return 0.0
;
	tsx
	jsr	__setup_both
	clra
	ldab	__lexp
	subb	__texp
	sbca	#0
	addb	#127
	adca	#0
	stab	__expdiff+1	; expdiff = lexp - texp + 127
	staa	__expdiff
	beq	__divf32tos03
	bpl	__divf32tos02
	cmpb	#$E9		; underflow
	bcc	__divf32tos03
	jmp	__f32retZeros
__divf32tos02:			; overflow
	jmp	__f32retInf
__divf32tos03:
	;			; TODO: exception check
	jsr	__fdiv32x32	; @tmp3:@tmp3+1:@tmp4:@tmp4+1 = @long / TOS
	ldaa	__expdiff+1
	beq	__divf32tos04
	tst	__expdiff
	bpl	__divf32tos08
__divf32tos04:
	; 			; subnormal number: shift right
	deca
__divf32tos05:
	lsr	@tmp3
	ror	@tmp3+1
	ror	@tmp4
	ror	@tmp4+1
	inca
	bne	__divf32tos05
	bra	__divf32tos30	; skip normalize
__divf32tos08:
	tst	@tmp3		; hidden bit set?
	bmi	__divf32tos20
__divf32tos10:
	deca
	beq	__divf32tos30	; under flow
	asl	@tmp4+1
	rol	@tmp4
	rol	@tmp3+1
	rol	@tmp3
	bpl	__divf32tos10
__divf32tos20:
;				; TODO: round
	ldab	@tmp4+1
	bpl	__divf32tos30
	andb	#$7f
	bne	__divf32tos22
	ldab	@tmp4
	asrb
	bcc	__divf32tos30
__divf32tos22:
	inc	@tmp4
	bne	__divf32tos30
	ldx	@tmp3
	inx	
	stx	@tmp3
__divf32tos30:
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
;	@tmp3:@tmp4		= @long / TOS
;	@tmp1+1:AccA:AccB	= @long % TOS
;	@tmp2: loop counter
;	brake @long
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
	beq	__cmpf32tos_eq	; branch if TOS==0.0
__cmpf32tos_10:
	tsx
	ldab	@long
	cmpb	2,x
	bgt	__cmpf32tos_gt
	blt	__cmpf32tos_lt
	ldab	@long+1
	cmpb	3,x
	bhi	__cmpf32tos_gt
	bcs	__cmpf32tos_lt
	ldab	@long+2
	cmpb	4,x
	bhi	__cmpf32tos_gt
	bcs	__cmpf32tos_lt
	ldab	@long+3
	cmpb	5,x
	bhi	__cmpf32tos_gt
	bcs	__cmpf32tos_lt
__cmpf32tos_eq:			; @long == TOS
	clrb
	clra
	jmp	__pullret
__cmpf32tos_gt:			; @long > TOS
	ldab	#1
	clra
	jmp	__pullret
__cmpf32tos_lt:			; @long < TOS
	clrb			; C=0 for not unordered relation
	decb
	tba
	jmp	__pullret
