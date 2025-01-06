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
	.data
	.export __sign
	.export __texp
	.export __lexp
__sign:	.byte	0	; sign of reslut
__texp:	.byte	0	; TOS's exp
__lexp:	.byte	0	; @long's exp
	.code
;
;	(0-3,x) == 0.0 ?
;		== 0x0000 0000 or 0x8000 0000	
;	if 0.0 then Z=1 else Z=0
;
__i32iszerox:
__f32iszerox:
	tst	3,x
	bne	iszerox_ret
	tst	2,x
	bne	iszerox_ret
	tst	1,x
	bne	iszerox_ret
	ldab	0,x
	bitb	#$7f		; (0,x) == 0x00 or 0x80 ?
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
;		TODO: round up/down
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
	bpl	__i32tof32_10
	nega
	negb
	sbca	#0
	;
__i32tof32_10:
	clr	@long+3
	stab	@long+2
	ldab	#$8e		; exp
	staa	@long+1
	bmi	__i32tof32_21
__i32tof32_20:
	decb
	asl	@long+2
	rol	@long+1
	bpl	__i32tof32_20
__i32tof32_21:
	asl	@long+1
	lsrb
	ror	@long+1
	orab	__sign
	stab	@long
	rts
;
;	int32 to float32
;		TODO: round up/down
;
__i32tof32:
	ldx	#long
	jsr	__f32iszerox
	bne	__i32tof32_1
	tstb			; 0x0000 0000 ?
	jeq	__f32zero
__f32minint:			; -2147483648 (0x8000 0000) = CF00 0000
	ldx	#0
	stx	@long+2
	ldx	#$CF00
	stx	long
	rts
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
	ldab	#$9C		; exp.
__i32tof32_right:
	decb
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
__i32tof32_left:
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
;	float to nsigned long
;		@long -> @long
;	
__f32toi32:
	ldx	#long
	jsr	__f32iszerox
	beq	__u32zero
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
	bpl	__i327fffffff
	bra	__i3280000000	; return 4,294,967,295
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
	ldab	2,x		; flip the sign
	eorb	#$80
	stab	2,x
;	jmp	addf32tos	; thru to __addf32tos
;
;	@long = TOS + @long
;	pull TOS
;
__addf32tos:
        ldx     #long
        jsr     __f32iszerox
	bne	__addf32tos1
	tsx
	inx
	inx
	jsr	__load32x
	jmp	__pullret
__addf32tos1:
	tsx
	inx
	inx
        jsr     __f32iszerox
	bne	__addf32tos2
	jmp	__pullret
__addf32tos2:
	tsx
	ldab	2,x		; get MSB of TOS
	eorb	@long
	andb	#$80		; sign equal?
	bne	__addf32tos50
	;			; sign equal, add it
	ldab	@long		; save sign
	andb	#$80
	stab	__sign
	jsr	__setup_both	; get both exp to texp, lexp. AccB = texp - lexp
	bne	__addf32tos3
	;
	bra	__addf32tos11	; texp==lexp, Simply add it.
__addf32tos3:
	bcc	__addf32tos10
__addf32tos21:			; TOS < @long
	negb
	jsr	__lsr_tos	;   shift TOS right
	ldaa	__lexp
	jmp	__addf32tos11
__addf32tos10:			; TOS > @long
	ldaa	__texp
	jsr	__lsr_long	;   shift @long right
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
	bcc	__addf32tos13	; over flow?
        ror     @long+1		; shift one bit
        ror     @long+2
        ror     @long+3
	inca			; exp++
__addf32tos13:
	asl	@long+1		; exp's LSB set to @long+1
	lsra
	ror	@long+1
	ora	__sign
__addf32tos15:
	staa	@long		; set exp
	jmp	__pullret
;
;	TOS and @long have different signs, do subtract
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
	;
__addf32tos54:
	jsr	__normalize_long	; while hidden bit ==0 do asl @long, AccA--
__addf32tos55:
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
;
;
__setup_both:			; get both exp, set hidden bit
	jsr	__setup_long	; @long's exp->AccA, set hidden bit of @long
	jsr	__setup_tos	; TOS's   exp->AccA, set hidden bit of TOS
	ldab	__texp
	subb	__lexp		; AccB = TOS'exp - @long's exp
	rts
;
__setup_long:		; @long's exp->AccA, set hidden bit of @long
	ldab	@long+1	; get TOS's exp to a
	ldaa	@long
	aslb
	rola
	sec		; set hidden bit of TOS
	rorb
	stab	@long+1
	staa	__lexp
	rts
;
__setup_tos:		; TOS's   exp->AccA, set hidden bit of TOS
	ldab	3,x	; get TOS's exp to a
	ldaa	2,x
	aslb
	rola
	sec		; set hidden bit of TOS
	rorb
	stab	3,x
	staa	__texp
	rts
;
__lsr_long:		; lsr @long by AccB
	lsr	@long+1
	ror	@long+2
	ror	@long+3
	decb
	bne	__lsr_long
	rts
__lsr_tos:		; lsr TOS (3-5,x) by AccB
	lsr	3,x
	ror	4,x
	ror	5,x
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
;	No arithmetic is required when multiplying by 0 or 1, but simply multiply it.
;
__mulf32tos:
	tsx
	ldab	2,x
	eorb	@long
	andb	#$80
	stab	__sign
	jsr	__setup_both
	ldab	__lexp
	subb	#127		; bias
	ldaa	__texp
	suba	#127
	aba
	; 			; TODO:	exception check
	adda	#127
	staa	__lexp		; saveit
;
;	clr	@tmp2+3		; clear working area
;	clr	@tmp2+2
	clr	@tmp2+1
	clr	@tmp2
;
	ldab	@long+3
	ldaa	3,x
	jsr	__mul8x8
;	addb	@tmp2+3
;	adca	@tmp2+2
	stab	@tmp2+3
	staa	@tmp2+2
	bcc	__mulf32tos_10
	inc	@tmp2+1
__mulf32tos_10:
;
	ldab	@long+2
	ldaa	4,x
	jsr	__mul8x8
	addb	@tmp2+3
	adca	@tmp2+2
	stab	@tmp2+3
	staa	@tmp2+2
	bcc	__mulf32tos_11
	inc	@tmp2+1
__mulf32tos_11:
;
	ldab	@long+1
	ldaa	5,x
	jsr	__mul8x8
	addb	@tmp2+3
	adca	@tmp2+2
	stab	@tmp2+3
	staa	@tmp2+2
	bcc	__mulf32tos_12
	inc	@tmp2+1
__mulf32tos_12:
;
	ldab	@long+1
	ldaa	4,x
	jsr	__mul8x8
	addb	@tmp2+2
	adca	@tmp2+1
	stab	@tmp2+2
	staa	@tmp2+1
	bcc	__mulf32tos_13
	inc	@tmp2
__mulf32tos_13:
;
	ldab	@long+2
	ldaa	3,x
	jsr	__mul8x8
	addb	@tmp2+2
	adca	@tmp2+1
	stab	@tmp2+2
	staa	@tmp2+1
	bcc	__mulf32tos_14
	inc	@tmp2
__mulf32tos_14:
	ldab	@long+1
	ldaa	3,x
	jsr	__mul8x8
	addb	@tmp2+1
	adca	@tmp2
	stab	@tmp2+1		; TODO:
	staa	@tmp2
	bcc	__mulf32tos_15
	swi	;  TODO: overflow check needed???
__mulf32tos_15:
	ldaa	__lexp		; new exp
	clrb
	tst	@tmp2		; hidden bit set?
	bpl	__mulf32tos_16
	inca
	bra	__mulf32tos_20
__mulf32tos_16:
	incb
	asl	@tmp2+3
	rol	@tmp2+2
	rol	@tmp2+1
	rol	@tmp2
	bpl	__mulf32tos_16
__mulf32tos_20:
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
;	TODO: TOS==0.0
;
__divf32tos:
	tsx
	ldab	2,x
	eorb	@long
	andb	#$80
	stab	__sign
	jsr	__setup_both
	ldaa	__lexp
	suba	#127		; bias
	ldab	__texp
	subb	#127
	sba
	;			; TODO: exception check
	adda	#127
	staa	__lexp		; saveit
	jsr	__div32x32	; @tmp3+1:@tmp4:@tmp4+1 = @long / TOS
	ldaa	__lexp
	tst	@tmp3		; hidden bit set?
	bmi	____divf32tos_20
__divf32tos_10:
	deca
	asl	@tmp4+1
	rol	@tmp4
	rol	@tmp3+1
	rol	@tmp3
	bpl	__divf32tos_10
____divf32tos_20:
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
;	@tmp3:@tmp4	= @long / TOS
;	@tmp1:AccA:AccB	= @long % TOS
;	@tmp2: loop counter
;	brake @long
;
	.export __div32x32
	.code
__div32x32:
	ldab #32
	stab @tmp2	; loop counter
	clr  @long
	ldab @long+1
	stab @tmp1+1
	clr  @tmp1
	ldab @long+3
	ldaa @long+2
loop:
	asl  @tmp4+1
	rol  @tmp4
	rol  @tmp3+1
	rol  @tmp3
	subb 5,x
	sbca 4,x
	pshb
	psha
	ldab @tmp1+1
	sbcb 3,x
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
	addb 5,x
	adca 4,x
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
;	AccAB<0:	@long<TOS
;	AccAB=0:	@long==TOS
;	AccAB>0:	@long>TOS
;
__cmpf32tos:
        ldx     #long
        jsr     __f32iszerox
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
__cmpf32tos_eq:
	clrb
	clra
	jmp	__pullret
__cmpf32tos_gt:
	ldab	#1
	clra
	jmp	__pullret
__cmpf32tos_lt:
	ldab	#$ff
	tba
	jmp	__pullret
