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
;
;		more test
;		refactoring
;		Exception handling
;		Speed up
;

	.zp
	.data
	.export _fmodf
;
	.data
__zin:	.blkb	1	; TOS & @long are Zero? Inf? NaN?
__exp:  .blkb   1
__expdiff:.blkb	1	; high half of @long's exp - TOS's exp
__expnew:.word	2	; new exp
;
__tos_p:.blkb	2	; points to TOS
;
	.code
;
;	@long = @long % TOS
;
_fmodf:
	tsx
	jsr	__setup_zin
;	ldab	__zin
	bitb	#$3F			; TOS or @long is NaN,Inf,0.0 ?
	beq	__fmodf_01		; No, do normal calculation
;
	bitb	#$03
	jne	__fmodf_NaN		; TOS or @long is NaN, return NaN
;
	bitb	#$04			; @long is Inf ?
	bne	__fmodf_NaN		; Yes, Inf%any returns NaN
	bitb	#$08			; TOS is Inf?
	bne	__fmodf32_s15		; Yes, return num(@long) it self.
;
	bitb	#$20			; TOS is 0.0 ?
	bne	__fmodf_NaN		; Yes, any/0.0 return NaN
;	bitb	#$10			; @long==0.0 ?
;	beq	__fmodf32_s15		; No,  0.0%any returns 0.0 it self.
__fmodf32_s15:
	rts				; returns num(@long) it self.
__fmodf_NaN:
	jmp	__f32NaN
;
__fmodf_01:
	ldab	@long			; sign of fmod(x,y) same as the x
	tba
	andb	#$7F			; @long = fabsf(@long)
	stab	@long	
	anda	#$80
	staa	__sign
;
	tsx				; TOS = fabsf(TOS)
	ldab	2,x
	andb	#$7F
	stab	2,x
;
	jsr	__cmpf32x		; @long == TOS?
	beq	__fmodf_zeros		; return 0.0, sign: same as the @long
	tstb				; @long < TOS? (b=$FF?)
	bpl	__fmodf_02
__fmodf_retlong:
	ldab	__sign			; Yes, return x (@long) it self
	orab	@long
	stab	@long
	rts
;
__fmodf_zeros:
	jmp	__f32zeros
;
__fmodf_02:                             ; Here, fabsf(@long)>fabsf(TOS)
;
;	@long = mx * 2^(ex-23), TOS = my * 2^(ey-23), mx and my are 24 bit
;	@long % TOS = (mx * 2^(ex-ey) % my) * 2^(ey-23), and the shift and
;	subtract below keeps every bit, so the result needs no rounding.
;
        tsx
        inx
        inx
        stx     __tos_p                 ; save TOS's address
        jsr     __adj_subnormal         ; do normalize,AccAB = unbiased exp
        stab    __expnew+1              ; the remainder sits at TOS's exp
        staa    __expnew
;
        ldx     #long
        jsr     __adj_subnormal
        subb    __expnew+1
        sbca    __expnew                ; expdiff = long's exp - TOS's exp
        staa    __expdiff               ; AccB keeps the low half, expdiff >= 0
;
        clr     @long                   ; r = mx, and r stays under 2^24
        ldx     __tos_p
        incb                            ; the loop runs expdiff+1 times
        inc     __expdiff
        bra     __fmodf_cmp             ; the first pass reduces without a shift
;
;	r -= my if r >= my.  r < 2*my holds every time, so one subtract is enough.
;
__fmodf_shift:
        asl     @long+3
        rol     @long+2
        rol     @long+1
        bcs     __fmodf_sub             ; r >= 2^24 > my
__fmodf_cmp:
        ldaa    @long+1
        cmpa    1,x
        bhi     __fmodf_sub
        bcs     __fmodf_keep
        ldaa    @long+2
        cmpa    2,x
        bhi     __fmodf_sub
        bcs     __fmodf_keep
        ldaa    @long+3
        cmpa    3,x
        bcs     __fmodf_keep
__fmodf_sub:
        ldaa    @long+3
        suba    3,x
        staa    @long+3
        ldaa    @long+2
        sbca    2,x
        staa    @long+2
        ldaa    @long+1
        sbca    1,x
        staa    @long+1
__fmodf_keep:
        decb
        bne     __fmodf_shift
        dec     __expdiff
        bne     __fmodf_shift
;
__fmodf_rem:
        ldaa    @long+1                 ; r == 0 ?
        oraa    @long+2
        oraa    @long+3
        jeq     __fmodf_zeros           ; mod=0, return 0.0 with sign
;
        ldab    __expnew+1              ; r is a 24 bit integer, while the tail
        ldaa    __expnew                ; normalizes a 1.f from bit 31: 31-23
        addb    #8
        adca    #0
        stab    __expnew+1
        staa    __expnew
;
	ldab	__expnew+1
	ldaa	__expnew
	tst	@long
	bmi	__fmodf_04		; MSB==1 needn't shitft
;
__fmodf32_0301:
	subb	#1			; exp--
	sbca	#0
	asl	@long+3			; @long <<= 1
	rol	@long+2
	rol	@long+1
	rol	@long
	bpl	__fmodf32_0301
;
__fmodf_04:
	stab	__expnew+1
	staa	__expnew
;
	subb	#<-126			; subnormal?
	sbca	#>-126
	jge	__fmodf_20		; no, it's normal number
;
	ldaa	@long+3
__fmodf_05:
	lsr	@long
	ror	@long+1
	ror	@long+2
	rora
	incb
	bne	__fmodf_05
;
	staa	@long+3
;
	ldab	#<-127			; subnormal's exp
	ldaa	#>-127
	bra	__fmodf32_done
;
__fmodf_20:
	ldab	__expnew+1
	ldaa	__expnew
;
__fmodf32_done:				; here, AccAB = expnew-shift(normalized)
	addb	#127			; add bias
	tba
	ldab	@long+2			; @long >> 8
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
