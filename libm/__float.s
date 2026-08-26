;
	.export	__i16tof32
	.export	__u16tof32
	.export	__i32tof32
	.export	__u32tof32
	.export	__f32tou32
	.export	__f32toi32
	.export	__f32tou16
	.export	__f32toi16
	.export	__f32tou8
	.export	__f32toi8
	.export	__addf32tos
	.export __subf32tos
	.export __mulf32tos
	.export __divf32tos
	.export __cmpf32tos
	.export __cmpf32_x2
	.export __cmpf32x
	.export __cmpf32bx
	.export __cmpf32dx
	.export __load32x_addf
	.export __load32x_subf
	.export __load32x_mulf
	.export __load32x_divf
	.export __load32x_cmpf
	.export __f32iszero
	.export __f32isNaNorInf
	.export __fdiv32x32
	.export	__setup_long
	.export __f32retpZero
	.export __setup_zin
	.export __f32NaN
	.export __f32NaNx
	.export __f32zerox
	.export __f32zeros
	.export __f32ones
	.export	__f32Infs
	.export __adj_subnormal
        .export __f32NaN
	.export __sign
	.export __fp_work
	.export __lexp
	.export __addf32x
	.export __subf32x
	.export __mulf32x
	.export __divf32x
	.export __addf32bx
	.export __subf32bx
	.export __mulf32bx
	.export __divf32bx
	.export __addf32dx
	.export __subf32dx
	.export __mulf32dx
	.export __divf32dx
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
;	.export __f32iszerox
;	.export __f32isNaNorInfx
;
;
	.data
__zin:	.byte	0	; TOS & @long are Zero? Inf? NaN?
__sign:	.byte	0	; sign (TOS & @long sign are different? 1:differ,0:same)
__lexp:	.byte	0	; @long's exp
__expdiff:.word	0	; @long's exp - TOS's exp
__exp2: .word	0	; exp work. subnormal use 2byte (127 to -149)
__fp_ix:.word	0	; address of the operand the routines read
__fp_op:.word	0	; working copy of that operand
	.word	0
;
;	__fp_work: 8 byte scratch area.
;
;	Only one routine uses __fp_work at a time. Add, subtract, multiply
;	and divide never run together, so the routines share the area.
;	Each routine picks its own layout. The layout sits at the top of
;	the routine that uses __fp_work.
;
__fp_work: .word	0
	.word	0
	.word	0
	.word	0
	
	.code
;
;	(0-3,x) is NaN? (exp==255)
;		NaN: 7F800001-7FFFFFFF, FF800001-FFFFFFFF
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
	rola			; AccA=exp, b=(b22-b16)<<1, don't care sign.
	adda	#1		; Use adda to update the carry flag (not inca).
	bne	__f32isNaN_1	; if exp!=$FF, not NaN and Inf. C=0,Z=0
	;			; exp == 255, check mantissa
	;			; Here, a is zero
	sba			; a-b → 0-b
	bne	__f32isNaN_1	; if b!=0 then jump Z=0 C=1
	cmpa	2,x		; Here, a==0, cmpa is faster than tst
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
	ldab	@long
	andb	#$7F
	bne	iszero_ret
	orab	@long+1
	orab	@long+2
	orab	@long+3
iszero_ret:
	rts

__f32iszerox:
	ldab	0,x		; exponent on the MSB side, to check from the top is faster
	andb	#$7F
	bne	iszerox_ret
				; Here, b is zero
        orab    1,x
        orab    2,x
        orab    3,x
iszerox_ret:
	rts
;
;	int16 to float32
;
;	@long ← (float)AccAB
;
__i16tof32:
	tstb
	bne	__i16tof32_1
	tsta
	jeq	__f32zero	; load 0.0 and return
	;
__i16tof32_1:
	staa	__sign
	bpl	__i16tof32_05
	nega			; AccAB = -AccAB
	negb
	sbca	#0
	;
__i16tof32_05:
        bne     __i16tof32_10   ; i32 in ±0〜255
        ldaa    #$87            ; exp ($86+1), if i32>=128 then exp=$86
__i16tof32_06:                  ; Shift left until the MSB becomes 1
        deca
        aslb
        bcc     __i16tof32_06 
;
        asl     __sign
        rora
        rorb
        stab    @long+1
        staa    @long
        clr     @long+3
        clr     @long+2
        rts
;
__i16tof32_10:                  ; i32 in ±256〜32768
	stab	@long+2
	ldab	#$8f		; exp ($8e+1), if i32>=32768 then exp=$8E
__i16tof32_20:			; Shift left until the MSB becomes 1
	decb
	asl	@long+2
	rola
	bcc	__i16tof32_20	; loop until C=1
				; MSB overflows to C, but it’s a hidden bit
__i16tof32_21:
	asl	__sign
	rorb
	rora
	ror	@long+2		; shifted over 1bit, fix it.
	staa	@long+1
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
	jsr	__neg32x	; negate (0-3,x)
__i32tof32_2:
	ldaa	0,x
	beq	__i32tof32_left	; need left shift
;
	ldab	#$96		; exp.
	clr	__fp_work
__i32tof32_right:		; right shift is required until the MSB byte becomes 0
	incb
	lsra
	ror	1,x
	ror	2,x
	ror	3,x
	ror	__fp_work		; save R/S bit
	tsta
	bne	__i32tof32_right
	ldaa	__fp_work
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
	clr	__fp_work
__u32tof32_right:		; right shift is required until the MSB byte becomes 0
	incb
	lsra
	ror	1,x
	ror	2,x
	ror	3,x
	ror	__fp_work		; save R/S bit
	tsta
	bne	__u32tof32_right
	ldaa	__fp_work
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
	jcs	__u32zero
__f32tou32_1:
; Undefined behavior when the value is out of the integer range. C17 §6.3.1.4
; This implementation, 0xffffffff was chosen intentionally for consistency.
	cmpb	#$9f		; if exp>=$9f (x >= 4,294,967,295)
	bcs	__f32tou32_2
	jmp	__u32ffffffff	; return 4,294,967,295
__f32tou32_2:
	ldaa	1,x		; recover hidden bit
	oraa	#$80
	staa	1,x
	clr	0,x
;
	subb	#$96		; TODO:
	beq	__f32tou32_ret
	jcc	__shl32
__f32tou32_4:
	negb
	jmp	__shr32u
;
__f32zerox:
	ldab	__sign
	andb	#$80
	stab	0,x
	clrb
	stab	1,x
	stab	2,x
	stab	3,x
__f32tou32_ret:
	rts
__f32zeros:
	bsr	__f32zero
	ldab	__sign
	andb	#$80
	orab	@long
	stab	@long
	rts
__f32zero:
__i32zero:
__u32zero:
	ldx	#0
__f32stx:
	stx	@long
	stx	@long+2
	rts
__f32ffffffff:
__i32ffffffff:
__u32ffffffff:
	ldx	#$ffff
	bra	__f32stx
__i327fffffff:
	ldx	#$7fff
	stx	@long
	ldx	#$ffff
	stx	@long+2
	rts
__i3280000000:
	ldx	#0
	stx	@long+2
	ldx	#$8000
	stx	@long
	rts
__f32Infs:
	ldab	__sign
	bmi	__f32mInf
__f32pInf:
__f327f800000:
	ldx	#$7F80
__f32Inf2:
	stx	@long
	ldx	#$0000
	stx	@long+2
	rts
__f32mInf:
__f32ff800000:
	ldx	#$FF80
	bra	__f32Inf2
__f32ones:
	tst	__sign
	bmi	__f32mOne
__f32pOne:
	ldx	#$3F80
__f32One_2:
	stx	@long
	ldx	#$0000
	stx	@long+2
	rts
__f32mOne:
	ldx	#$BF80
	bra	__f32One_2
;
;	load plus/minus Inf into @long
;
__f32retInfs:
	ldab	__sign
__f32retInf:		; AccB(Sign)+7f80 0000
	tstb
	bmi	__f32retmInf
__f32retpInf:		; 7f80 0000
	bsr	__f327f800000
	rts
__f32retmInf:		; ff80 0000
	bsr	__f32ff800000
	rts
;
;	load plus/minus qNaN into @long
;
__f32retNaN:
	bsr	__f32NaN
	rts
__f32NaN:
	ldx	#long
__f32NaNx:
	ldab	#$7F		; only +qNaN return
	stab	0,x
	ldab	#$C0
	stab	1,x
	clrb
	stab	2,x
	stab	3,x
	rts
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
	rts
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
	ldab	@long
	pshb
	andb	#$7F
	stab	@long
	jsr	__f32tou32
	pulb
	tstb
	jmi	__neg32
	rts
;
;	float to unsigned char
;		@long -> AccB, clear AccA
;
__f32tou8:
	bsr	__f32tou16
	clra
	rts
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
;	float to signed char
;		@long -> AccB
;	
__f32toi8:
	ldx	#long
__f32toi8x:
	jsr	__f32iszerox
	beq	__s8zero
	ldab	0,x
	ldaa	1,x
	asla
	rolb			; B = exp
	sec			; set hidden bit
	rora			; A = MSB
	cmpb	#$3f		; if exp<=$3e (x < 0.5) then return 0;
	bcc	__f32toi8_1
__s8zero:
	clrb
	clra
	rts
;
__f32toi8_1:
	cmpb	#$86		; if exp>=$86 (x > 127)
	bcs	__f32toi8_2
	ldaa	0,x		; check sign
	bmi	__s8_80		; x <= -32768
__s8_7f:			; x > 127, return 127
	ldab	#$7F
	clra
	rts
__s8_80:			; x <= -128, return -128
	ldab	#$80
	ldaa	#$FF
	rts
;
__f32toi8_2:			; AccA:MSB, AccB:exp (biased)
	subb	#$86
	beq	__f32toi8_ret
__f32toi8_4:
	lsra
	incb
	bne	__f32toi8_4
__f32toi8_ret:
	tab
	clra
	tst	0,x
	bpl	__f32toi8_ret2
	negb
	deca
__f32toi8_ret2:
	rts
;
;
;
__u16ffff:
	ldab	#$FF
	tba
	rts
;
;	@long	= @long - TOS
;	→	= @long + (-TOS)
;
__subf32bx:
	clra
__subf32dx:
	jsr	__adx
__subf32x:
	stx	__fp_ix
	jsr	__setup_zin_x	; TOS & @long is zero/Inf/NaN?
;	ldab	__zin
	eorb	#$80		; sub flips TOS's sign, so the two signs differ
	stab	__zin
	bra	__addf32_0
;
;	@long = TOS + @long
;	pull TOS
;
__addf32bx:
	clra
__addf32dx:
	jsr	__adx
__addf32x:
	stx	__fp_ix
	jsr	__setup_zin_x	; TOS & @long is zero/Inf/NaN?
;	ldab	__zin
__addf32_0:
	bitb	#$3F
	beq	__addf32_1	; No,  normal calculation
;
	bitb	#$03		; TOS or @long is NaN?
	bne	__addf32_retNaN ; Yes: return NaN
;
	bitb	#$0C		; TOS or @long is Inf?
	beq	__addf32_s20
	bitb	#$04		; @long is Inf?
	beq	__addf32_s10	; No, only TOS is Inf
	bitb	#$08		; TOS is Inf too?
	beq	__addf32_s05	; No, return Inf, sign is same as @long
	tstb			; AccB is __zin. each sign are same?
	bpl	__addf32_s05	; 
__addf32_retNaN:
	jmp	__f32retNaN	; No,  return NaN
__addf32_s05:
	ldab	@long		; Yes, return Inf. sign is the same as @long
	jmp	__f32retInf
	;
__addf32_s10:			; TOS is Inf, @long is not
	ldab	@long		; TOS's sign = @long's sign xor __zin's b7
	eorb	__zin
	jmp	__f32retInf	; return Inf, The sign is the same as TOS
;
__addf32_s20:			; TOS and @long are not NaN,Inf.
	andb	#$30		; TOS or @long == 0.0?
	beq	__addf32_1	; No
	cmpb	#$30		; TOS and @long == 0.0?
	jne	__addf32_s50
	ldab	__zin		; Yes. same sign?
	jmi	__f32retpZero	; Not same sign. return +0.0
	jmp	__f32retZerol	; return 0.0, sign is same as @long
;
__addf32_s50:			; TOS or @long == 0.0
	cmpb	#$20		; TOS == 0.0?
	beq	__addf32_s51	; Yes, return @long (do nothing)
	ldab	@long		; TOS's sign = @long's sign xor __zin's b7
	eorb	__zin
	andb	#$80
	stab	__sign
	ldx	__fp_ix
	ldx	2,x
	stx	@long+2
	ldx	__fp_ix
	ldx	0,x
	stx	@long
	ldab	@long
	andb	#$7F
	orab	__sign
	stab	@long
__addf32_s51:
	rts
;
__addf32_1:			; neither @long nor TOS is 0.0
	ldx	__fp_ix
	jsr	__abscmp	; abs(TOS) - abs(@long)
	bhi	__addf32_2	; abs(TOS) > abs(@long), so swap the two
	bne	__addf32_3
	tst	__zin		; equal size. b7 says the signs differ
	jmi	__f32retpZero	; x + (-x) is +0.0
;
__addf32_3:			; @long already holds the bigger value
	ldab	@long		; the result takes @long's sign
	bra	__addf32_4
;
__addf32_2:			; TOS is bigger, so swap the two
	ldab	@long		; the result takes TOS's sign, which is
	eorb	__zin		;   @long's sign xor __zin's b7
	andb	#$80
	stab	__sign
	ldx	@long+2		; keep the smaller value
	stx	__fp_work+6
	ldx	@long
	stx	__fp_work+4
	ldx	__fp_ix
	ldx	2,x		; @long <= the raw operand
	stx	@long+2
	ldx	__fp_ix
	ldx	0,x
	stx	@long
	ldx	#__fp_work+4
	bra	__addf32_41
;
__addf32_4:
	andb	#$80
	stab	__sign
__addf32_41:
	jsr	__setup_long
;
;	Unpack the smaller value into __fp_work and line it up with @long.
;
;	IX:	the address of the smaller value
;
;	__fp_work layout:
;	  +1..+3  mantissa 24bit
;	  +4      guard byte. b7:G b6:R b5:S
;	  +5..+7  scratch
;
	ldab	1,x		; the operand itself stays untouched
	ldaa	0,x
	aslb
	rola
	sec			; set hidden bit
	bne	__setup_work_01	; subnormal number?
	inca
	clc
__setup_work_01:
	rorb			; AccB: mantissa 23-16
	nega
	ldx	2,x		; IX: mantissa 15-0
	adda	__lexp		; AccA: 0 to 253, never overflows
	cmpa	#8
	bcs	__setup_work_10
	cmpa	#16
	bcs	__setup_work_20
	cmpa	#24
	bcs	__setup_work_30
	cmpa	#26
	bcs	__setup_work_40
;				; the smaller value cannot change the result
	ldaa	__lexp
	jmp	__addf32_29
;
__setup_work_10:		; 0 to 7
	stab	__fp_work+1
	stx	__fp_work+2
	clrb			; nothing falls off the guard byte here
	tsta
	beq	__setup_work_60
__setup_work_11:
	lsr	__fp_work+1
	ror	__fp_work+2
	ror	__fp_work+3
	rorb
	deca
	bne	__setup_work_11
	bra	__setup_work_60
;
__setup_work_30:		; 16 to 23
	stab	__fp_work+3
	stx	__fp_work+4	; +5 takes the byte that falls off
	ldab	__fp_work+5
	beq	__setup_work_31
	ldab	#1		; sticky
__setup_work_31:
	orab	__fp_work+4
	bra	__setup_work_43
;
__setup_work_40:		; 24 to 25
	cpx	#0		; the whole 16 bits fall off
	beq	__setup_work_42
	orab	#1		; sticky
__setup_work_42:
	clr	__fp_work+3
__setup_work_43:
	ldx	#0
	stx	__fp_work+1
;
__setup_work_45:		; +1 and +2 are already 0
	anda	#7		; AccA: the shift count
	beq	__setup_work_60
__setup_work_46:
	lsr	__fp_work+3
	rorb
	bcc	__setup_work_47
	orab	#$20
__setup_work_47:
	deca
	bne	__setup_work_46
	bra	__setup_work_60
;
__setup_work_20:		; 8 to 15
	stab	__fp_work+2
	stx	__fp_work+3
	clr	__fp_work+1
;
__setup_work_50:		; +1 is already 0
	ldab	__fp_work+4
	anda	#7		; AccA: the shift count
	beq	__setup_work_60
__setup_work_51:
	lsr	__fp_work+2
	ror	__fp_work+3
	rorb
	bcc	__setup_work_52
	orab	#$20
__setup_work_52:
	deca
	bne	__setup_work_51
__setup_work_60:
	bitb	#$1f		; recover the sticky bit
	beq	__setup_work_61
	orab	#$20
__setup_work_61:
	andb	#$e0
	stab	__fp_work+4
__addf32_5:
	ldaa	__lexp
	tst	__zin		; the signs differ?
	jmi	__addf32_50
__addf32_11:
	ldab	@long+3		; @long = @long + __fp_work , 24bit version
	addb	__fp_work+3
	stab	@long+3
	ldab	@long+2
	adcb	__fp_work+2
	stab	@long+2
	ldab	@long+1
	adcb	__fp_work+1
	stab	@long+1
	bcc	__addf32_20	; over flow?
        ror     @long+1		; shift one bit with carry
        ror     @long+2
        ror     @long+3
        ldab	__fp_work+4	; sticky
	rorb
	bitb	#$3F	
	beq	__addf32_12
	orab	#$20
__addf32_12:
	stab	__fp_work+4
	inca			; exp++
	cmpa	#$FF		; biased exponent exceeds 254, so it is Inf.
	jeq	__f32retInfs
__addf32_20:			; even number rounding
	ldab	@long+3
	lsrb			; LSB -> Carry
	ldab	__fp_work+4
	rorb			; b7:LSB, b6:G, b5:R, b4:S
	bitb	#$40		; b6:G==0?
	beq	__addf32_29	;   Yes, do nothng
	andb	#$F0
	cmpb	#$40		; 0100:only G is 1?
	beq	__addf32_29	;   Yes, do nothng
;
	inc	@long+3		; round up
	bne	__addf32_29
	inc	@long+2
	bne	__addf32_29
	inc	@long+1
	bne	__addf32_29
;
	inca
	cmpa	#$FF
	jeq	__f32retInfs
	lsr	@long+1
	ror	@long+2
	ror	@long+3
;
__addf32_29:
	cmpa	#1		; sub normal number?
	bne	__addf32_30
	tst	@long+1		; check hiden bit, when 1 convert to normal
	bmi	__addf32_30
	clra
__addf32_30:
	asl	@long+1		; exp's LSB set to @long+1
	lsra
	ror	@long+1
	ora	__sign
	staa	@long		; set exp
	rts
;
;	@long holds the bigger value, so the result is @long - __fp_work
;
__addf32_50:
	neg	__fp_work+4	; C=1 when the guard byte borrows
	ldab	@long+3		; @long = @long - __fp_work
	sbcb	__fp_work+3
	stab	@long+3
	ldab	@long+2
	sbcb	__fp_work+2
	stab	@long+2
	ldab	@long+1
	sbcb	__fp_work+1
	stab	@long+1
	;
__addf32_60:
	bmi	__addf32_80	; hidden bit on?
	;
__addf32_70:
	deca
	beq	__addf32_80	; subnormal number. stop shift
	ldab	__fp_work+4	; b4-b0 are always 0, so no sticky fold is needed
	aslb
	stab	__fp_work+4
	rol	@long+3
	rol	@long+2
	rol	@long+1
	jpl	__addf32_70	; hidden bit become 1 ?
;
__addf32_80:
	ldab	@long+3
	lsrb			; LSB -> Carry
	ldab	__fp_work+4
	rorb			; b7:LSB, b6:G, b5:R, b4:S
	bitb	#$40		; b6:G==0?
	beq	__addf32_90	;   Yes, do nothng
	andb	#$F0
	cmpb	#$40		; 0100:only G is 1?
	beq	__addf32_90	;   Yes, do nothng
;
	inc	@long+3		; round up
	bne	__addf32_90
	inc	@long+2
	bne	__addf32_90
	inc	@long+1
	bne	__addf32_90
;
	inca
	cmpa	#$FF
	jeq	__f32retInfs
	lsr	@long+1
	ror	@long+2
	ror	@long+3
;
__addf32_90:
	asl	@long+1		; exp's LSB into @long+1
	lsra
	ror	@long+1
	ora	__sign		; recover sign bit
	staa	@long
	rts
;
;	compare: abs(tos) - abs(@long)
;
;	  TOS<@long:  return C=1 (BCS)
;	  TOS==@long: return Z=1 (BEQ)
;	  TOS>@long:  C=0,Z=0    (BHI)
;
__abscmp:
	ldaa	0,x
	anda	#$7f		; ignore sign bit
	ldab	@long
	andb	#$7f		; ignore sign bit
	sba
	bne	__abscmp_ret
	ldab	1,x
	subb	@long+1
	bne	__abscmp_ret
	ldab	2,x
	subb	@long+2
	bne	__abscmp_ret
	ldab	3,x
	subb	@long+3
__abscmp_ret:
	rts
;
;	check both Inf and NaN
;	__setup_zin:   2,x = TOS top
;	__setup_zin_x: 0,x = TOS top
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
__setup_zin:			; X is 2 based. falls into the _x entry
	inx
	inx
__setup_zin_x:			; X is 0 based
	ldaa	0,x
	tab
	eorb	@long
	andb	#$80
	stab	__sign		; First, determine the sign
;
	anda	#$7F		; exp 2-253 is normal. exp 0,1,254,255 need the full test
	deca			; (exp>>1)-1 is $00-$7D when normal, $FF or $7E otherwise
	cmpa	#$7E
	bcc	__setup_zin_05
	ldaa	@long
	anda	#$7F
	deca
	cmpa	#$7E
	bcc	__setup_zin_05
	stab	__zin
	rts
;
__setup_zin_05:
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
;	Change @long floating point number for easier calculations.
;	  Put the exponent in __lexp (1 byte, biased)
;	    If the biased exponent is 00 (subnormal), it becomes 01.
;	  Set a hidden bit for normal number (without subnormal).
;	  The mantissa stays at @long+1 to +3, and @long+0 takes the carry.
;
;	Special numbers ( Inf, NaN ) cannot be handled here.
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
__fp_settos:			; sub flips the sign, so it needs its own copy
	ldab	0,x
	stab	__fp_op
	ldab	1,x
	stab	__fp_op+1
	ldab	2,x
	stab	__fp_op+2
	ldab	3,x
	stab	__fp_op+3
	ldx	#__fp_op
	stx	__fp_ix
	rts
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
__mulf32bx:
	clra
__mulf32dx:
	jsr	__adx
__mulf32x:
	stx	__fp_ix
	jsr	__setup_zin_x	; TOS & @long is zero/Inf/NaN?
	;
;	ldab	__zin
	bitb	#$3F		;
	beq	__mulf32tos4	; No, nomal calculation
;
	bitb	#$03		; TOS or @long is NaN?
	jne	__f32retNaN	; Yes: return NaN
	bitb	#$0C		; TOS or @long is Inf?
	beq	__mulf32_s10
	andb	#$30		; TOS or @long is zero?
	jeq	__f32retInfs	; No, Inf * (not zero) returns Inf with __sign.
	jmp	__f32retNaN	; Inf*0.0 returns NaN
;
__mulf32_s10:			; TOS and @long is not Inf,NaN
	andb	#$30
	jne	__f32retZeros	; TOS or @long is zero
__mulf32tos4_s:
	jsr	__fp_settos	; shift a private copy instead
	jsr	__adj_subnormal
	bra	__mulf32tos4_e
;
__mulf32tos4:			; like __adj_subnormal, but keeps a normal operand intact
	ldx	__fp_ix
	ldaa	1,x
	ldab	0,x
	asla
	rolb			; get exp in b
	beq	__mulf32tos4_s	; subnormal needs the shifting loop
	clra
	subb	#127		; un bias
	sbca	#0
__mulf32tos4_e:
	stab	__exp2+1
	staa	__exp2
;                               ; IX still points at the operand on both paths
        ldab    1,x
	orab	#$80		; hidden bit, the normal path does not write it back
	stab	__fp_work+3
        ldx     2,x
        stx     __fp_work+4
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
; Exponent sum(=150) appears to underflow,
; but mantissa multiplication carry can keep it subnormal.
;
	subb	#<-151-128	; sum of exp < -151? AccAB still holds exp-128
	sbca	#>-151-128
	jlt	__f32retZeros	; Underflow, return zero with __sign.
;
__mulf32tos03:
;                               ; To improve performance, use AccAB insted of work+4,5
;                       	; setup working area 48bit
;	clr	__fp_work+2        ; use AccB
;	clr	__fp_work+1        ; use AccA
        clra                    ; __fp_work+1
        staa    __fp_work
;
        clrb                    ; __fp_work+2
	ldx	#__fp_work+5
;
__mulf32tos29:
        pshb
        ldab    #4
        stab    @tmp2           ; loop count. the body runs twice per turn
        pulb
;
        lsr     0,x             ; check LSbit
;
__mulf32tos30:
;
        bcc     __mulf32tos32
        addb    @long+3
        adca    @long+2
;       psha
        staa    @tmp2+1
        ldaa   __fp_work
        adca    @long+1
        staa    __fp_work
;       pula
        ldaa    @tmp2+1         ; 1cyc faster
;
__mulf32tos32:
        ror     __fp_work
	rora
	rorb
        ror     0,x             ; Carry used by by __mulf32tos34. Must preserve
;
        bcc     __mulf32tos34
        addb    @long+3
        adca    @long+2
        staa    @tmp2+1
        ldaa   __fp_work
        adca    @long+1
        staa    __fp_work
        ldaa    @tmp2+1
;
__mulf32tos34:
        ror     __fp_work
	rora
	rorb
        ror     0,x             ; Carry used by by __mulf32tos30. Must preserve
	dec     @tmp2
	bne	__mulf32tos30   ; ↑ C flag must not be modified until here
        dex
        cpx     #__fp_work+2
        bne     __mulf32tos29
;
        stab    __fp_work+2
        staa    __fp_work+1
;
        			; end of mant*mant multiply
;
;  Bits 28-47 required only for rounding. To reduce shift operations,
;  round lower 2 bytes (bits 32-47) to sticky bit beforehand.
;
        ldx     __fp_work+4
        beq     __mulf32tos50
        ldab    __fp_work+3
        orab    #$10            ; set sticky
        stab    __fp_work+3
__mulf32tos50:
;
; When highest bit is set, add 1 to the exponent.
; Exponent can be up to 127; overflow if incremented to 128.
;
	ldab	__fp_work		; carryover of the MSB bit?
	bpl	__mulf32tos70
        ldx     __exp2
        inx
        stx     __exp2
        cpx     #128
        bne     __mulf32tos705
        jmp     __f32retInfs    ; Overflow, returns Inf with __sign.
;
; Already rounded to 32-bit, so 4-byte shift is sufficient.
;
__mulf32tos70:
	asl	__fp_work+3
	rol	__fp_work+2
	rol	__fp_work+1
	rol     __fp_work
;
; Denormalize before rounding, otherwise the rounding position is wrong.
;
__mulf32tos705:
	ldab	__exp2+1
	ldaa	__exp2
	subb	#<-126
	sbca	#>-126
	bge	__mulf32tos71
__mulf32tos706:
	lsr	__fp_work
	ror	__fp_work+1
	ror	__fp_work+2
	ror	__fp_work+3
	bcc	__mulf32tos707
	ldaa	__fp_work+3
	oraa	#$02		; sticky
	staa	__fp_work+3
__mulf32tos707:
	incb
	bne	__mulf32tos706
	ldab	#<-127
	stab	__exp2+1
;
; even number rounding
;   ULP G R S
;    0  0 - -   none
;    0  1 0 0   none    to the nearest even
;    0  1 0 1   +1 ULP
;    0  1 1 -   +1 ULP
;    0  0 - -   none
;    1  1 0 0   +1 ULP  to the nearest even
;    1  1 0 1   +1 ULP
;    1  1 1 -   +1 ULP
;
__mulf32tos71:
	ldab	__fp_work+3
	bpl	__mulf32tos72	; G=0, do nothing
	bitb	#$01		; b0 falls off the rorb below
	beq	__mulf32tos721
	orab	#$02
__mulf32tos721:
	ldaa	__fp_work+2	; check LSB
	lsra
	rorb			; b7:ULP, b6:G, b5:R, b4-0:S
	andb	#$BF		; 1011 1111:ULP,R,S are all 0 ?
	beq	__mulf32tos72	;   Yes, do nothng
;
	inc	__fp_work+2	; round up
	bne	__mulf32tos72
	inc	__fp_work+1
	bne	__mulf32tos72
	inc	__fp_work+0
	bne	__mulf32tos72
;
        ldx     __exp2          ; Rounding changed exponent
        inx
        stx     __exp2
        cpx     #128            ; Recheck for overflow
	jeq	__f32retInfs	; Overflow, returns Inf with __sign.
	ldaa	#$80		; the mantissa is all 0 by now, put the hidden bit back
	staa	__fp_work+0
;
__mulf32tos72:
	ldab	__exp2+1
	cmpb	#<-127		; subnormal ?
	bne	__mulf32tos75	; a normal result keeps the hidden bit, so it is never 0
	tst	__fp_work		; round up carried into the hidden bit
	bpl	__mulf32tos74
	ldab	#<-126
	stab	__exp2+1
	bra	__mulf32tos75	; the round up just set the hidden bit, so it is never 0
__mulf32tos74:
	ldab	__fp_work+2
	orab	__fp_work+1
	orab	__fp_work
	jeq	__f32retZeros	; The mantissa is all 0, so the value is 0.
__mulf32tos75:
	ldab	__fp_work+2
	stab	@long+3
	ldab	__fp_work+1
	stab	@long+2
	ldab	__fp_work
	ldaa	__exp2+1
	adda	#127
	aslb
	lsra
	rorb
	stab	@long+1
	oraa	__sign
	staa	@long
	rts
;
;	@long = @long / TOS
;
__divf32bx:
	clra
__divf32dx:
	jsr	__adx
__divf32x:
	jsr	__fp_settos
	jsr	__setup_zin_x	; TOS & @long is zero/Inf/NaN?
;	ldab	__zin
;
	bitb	#$3F
	beq	__divf32tos01	; No, normal calculation
;
	bitb	#$03
	jne	__f32retNaN	; TOS or @long is NaN, return NaN
;
	bitb	#$0C		; TOS or @long is Inf?
	beq	__divf32_s20
	bitb	#$08		; TOS is Inf?
	beq	__divf32_s10	; No: @long is Inf, TOS is finite
	bitb	#$04		; @long is Inf too?
	jne	__f32retNaN	; Yes, Inf/Inf returns NaN
	jmp	__f32retZeros	; num/Inf returns 0.0 with __sign
;
__divf32_s10:			; @long is Inf, TOS is finite. 0.0 included
	jmp	__f32retInfs	; Inf/num returns Inf with __sign
;
__divf32_s20:
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
	ldx	__fp_ix
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
	subb	#<129
	sbca	#>129
	jge	__f32retInfs	; overflow
;
; Exponent diff(=-150) appears to underflow,
; but the round up can still make it subnormal.
;
	subb	#<-150-129	; expdiff < -150? AccAB still holds expdiff-129
	sbca	#>-150-129
	jlt	__f32retZeros	; underflow (can't expressed even in subnormal)
;
	ldx	__fp_ix
;
;	Since division 24bit is done in 32-bit,
;	  the result will never be 0 (Dividend 0 is already excluded)
;
__divf32tos03:
	jsr	__fdiv32x32		; @long = @long / TOS, @tmp1+1:AB = rem
	orab	@tmp1+1                 ; Set sticky bit if any remainder.
	staa	@tmp1
	orab	@tmp1
	beq	____divf32_norem
	ldab	@long+3			; set sticky
	orab	#$10
	stab	@long+3
____divf32_norem:
	ldab	__expdiff+1
	ldaa	__expdiff
	tst	@long
	bmi	__divf32tos04		; if MSB==1 needn't shift
;
__divf32_0301:
	subb	#1			; exp--
	sbca	#0
	asl	@long+3
	rol	@long+2
	rol	@long+1
	rol	@long
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
	lsr	@long
	ror	@long+1
	ror	@long+2
	ror	@long+3
	bcc	__divf32tos06
	ldaa	@long+3			; keep the bit shifted out as sticky
	oraa	#$01
	staa	@long+3
__divf32tos06:
	incb
	bne	__divf32tos05
;
;					; round up check (subnormal)
	bsr	__divf32_rup_check	; if C==1, need round up
	ldab	#<-127			; subnormal's exp. ldab and ldaa keep C
	ldaa	#>-127
	bcc	__divf32_done
;	
	inc	@long+2
	bne	__divf32_done
	inc	@long+1
	bne	__divf32_done
	inc	@long
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
	ldx	__expdiff		; the left shift did not happen, so 128 is still possible
	cpx	#128
	jeq	__f32retInfs
	bsr	__divf32_rup_check	; C==1, need round up
	ldab	__expdiff+1		; ldab and ldaa keep C
	ldaa	__expdiff
	bcc	__divf32_done
;
__divf32_rup:
	inc	@long+2
	bne	__divf32_done
	inc	@long+1
	bne	__divf32_done
	inc	@long		; the mantissa is never all ones here, so no carry out
;
__divf32_done:
	addb	#127
	tba
	ldab	@long+2
	stab	@long+3
	ldab	@long+1
	stab	@long+2
	ldab	@long
	aslb
	lsra
	rorb
	stab	@long+1
	oraa	__sign
	staa	@long
	rts
;
;	round up check, @tmp4 and @tmp4+1
;
__divf32_rup_check:
	ldab	@long+3	
	bpl	__divf32_rup_none	; G==0, no round up
	andb	#$7F
	bne	__divf32_rup_yes
	ldab	@long+2
	andb	#1
	beq	__divf32_rup_none
__divf32_rup_yes:
	sec
	rts
__divf32_rup_none:
	clc
	rts
;
;	@long			= @long / TOS
;	@tmp1+1:AccA:AccB	= @long % TOS
;       @tmp3:@tmp4     copy of 2-4,x
;	mess @long
;
__fdiv32x32:
        ldab 3,x	; the mantissa sits at 1-3,x, so no byte shift is needed
        stab @tmp4
        ldab 2,x
        stab @tmp3+1
        ldab 1,x
        stab @tmp3
;
        ldx #long
;
	ldab @long+1	; tmp1+1:AccAB <- @long+1 24bit
	stab @tmp1+1
	ldaa @long+2
;
        clrb
	stab @long+1	; clear quotient
	stab @long+2
;
	incb		; sentinel. rol carries it out after 8 turns
	stab @long
;
	ldab @long+3	; read it before the clear below
	clr  @long+3
;
        bra  loop_begin
;
loop:
	aslb		; shift reminder
	rola
	rol  @tmp1+1
        bcs loop_begin_1
        bmi loop_begin
;                       ; C=0, so the quotient takes a 0 bit
        rol 0,x
        bcc loop
        bra nextbyte
loop_begin_1:			; bit24 is set, so the divisor always fits
	subb @tmp4	; dividend - divisor
	sbca @tmp3+1
	pshb
	ldab @tmp1+1
	sbcb @tmp3
	stab @tmp1+1
	pulb
        sec
	bra  next
loop_begin:
	subb @tmp4	; dividend - divisor
	sbca @tmp3+1
	pshb
	ldab @tmp1+1
	sbcb @tmp3
	bcs  skip
	stab @tmp1+1	; subtracted.
	pulb
;	inc  0,x	; set the lower bit of the quotient
        sec
	bra  next
skip:
	pulb		; can't substract. pull it back.
	addb @tmp4
	adca @tmp3+1
        clc
next:
        rol 0,x
        bcc loop
nextbyte:
        inx
        cpx #long+4
        beq next8
        pshb
        ldab #1
        cpx #long+3
        bne next4      ; the last byte takes 2 bits / 8*3+2 = 26bit (24+G+R)
        ldab #$40
next4:
        stab 0,x
        pulb
        bra loop
next8:
;
        pshb
        ldab @long+3    ; only 2 bits are valid; move them to b7 G, b6 R
        rorb
        rorb
        rorb
        andb #$C0
        stab @long+3
        pulb
ret:
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
	lsra		; asla left 1,x here. bit7 is the exp LSB and the hidden bit takes it
	ora	#$80
	staa	1,x
	clra
	subb	#127	; un bias
	sbca	#0
	rts

;
;	stack calling convention: the operand sits on the stack,
;	so point X at it and let __pullret drop it on the way back
;
__load32x_addf:
	jsr	__load32x
__addf32tos:
	tsx
	inx
	inx
	jsr	__addf32x
	jmp	__pullret
;
__load32x_subf:
	jsr	__load32x
__subf32tos:
	tsx
	inx
	inx
	jsr	__subf32x
	jmp	__pullret
;
__load32x_mulf:
	jsr	__load32x
__mulf32tos:
	tsx
	inx
	inx
	jsr	__mulf32x
	jmp	__pullret
;
__load32x_divf:
	jsr	__load32x
__divf32tos:
	tsx
	inx
	inx
	jsr	__divf32x
	jmp	__pullret
;
__load32x_cmpf:
	jsr	__load32x
__cmpf32tos:
	tsx
	inx
	inx
	jsr	__cmpf32x
	jmp	__pullret
;
;	compare @long and (0-3,x)
;	IX points to the 2nd operand
;	NaN and ±0.0 are handled here
;
__cmpf32bx:
	clra
__cmpf32dx:
	jsr	__adx
__cmpf32x:
	stx	__fp_ix
	jsr	__f32isNaNorInf	; if @long is NaN, C=1. __f32isNaNorInf destroys IX.
	bcs	__cmpf32x_ret
	ldx	__fp_ix
	jsr	__f32isNaNorInfx ; if (0-3,x) is NaN, C=1
	bcs	__cmpf32x_ret
;
        jsr     __f32iszero	; @long == 0.0 ? __f32iszero keeps IX.
	bne	__cmpf32x_10	; branch if @long!=0.0
        jsr     __f32iszerox
	bne	__cmpf32x_10
	clrb			; ±0.0==±0.0? return eq
	clra
__cmpf32x_ret:
	rts			; rts keeps the carry flag, only rti pulls the CCR
__cmpf32x_10:
	dex			; __cmpf32_x2 reads 2,x
	dex
	jmp	__cmpf32_x2
__cmpf32_x2:			; compare @long and 2,x
				; handle NaN and ±0.0 before you call this
	ldab	@long
	bpl	__cmpf32_x2_p	; jump if @long >= 0
	ldab	2,x
	bpl	__cmpf32_x2_lt	; @long<0 LT TOS>=0
;
	bsr	__cmpf32_x2_s	; @long<0 && TOS<0
	bcs	__cmpf32_x2_gt	; C=1
	beq	__cmpf32_x2_eq	; Z=1
	bra	__cmpf32_x2_lt
;
__cmpf32_x2_lt:			; @long < TOS
	clrb			; C=0
	decb			; Z=0, N=1
	tba
	rts
;
__cmpf32_x2_p:			; if @long >= 0
	ldab	2,x
	bmi	__cmpf32_x2_gt	; @long>=0 && TOS<0
	bsr	__cmpf32_x2_s
	bcs	__cmpf32_x2_lt
	beq	__cmpf32_x2_eq
__cmpf32_x2_gt:			; @long > TOS
	clra			; C=0
	ldab	#1		; Z=0, N=0
	rts
__cmpf32_x2_eq:			; @long == TOS
	clrb
	clra			; Z=1,C=0, N=0
	rts
;
__cmpf32_x2_s:
	ldab	@long
	cmpb	2,x
	bne	__cmpf32_x2_sret
	ldab	@long+1
	cmpb	3,x
	bne	__cmpf32_x2_sret
	ldab	@long+2
	cmpb	4,x
	bne	__cmpf32_x2_sret
	ldab	@long+3
	cmpb	5,x
	bne	__cmpf32_x2_sret
__cmpf32_x2_sret:
	rts			; when @long>0 && TOS>0
				; @long == TOS : C=0, Z=1
				; @long <  TOS : C=0, Z=0
				; @long >  TOS : C=1, Z=0
