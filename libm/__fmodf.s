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
__sign:	.blkb	1	; sign (TOS & @long sign are different? 1:differ,0:same)
__exp:  .blkb   1
__expdiff:.blkb	2	; TOS's exp - @long's exp
__expnew:.word	2	; new exp
;
__yadrs:.blkb   2
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
__fmodf_02:				; Here, fabsf(x)>fabsf(y)
;
        tsx                             ; exp's diff > 10?
        ldab    3,x
        ldaa    2,x
        aslb
        rola
        staa    __exp
        ldab    @long+1
        ldaa    @long
        aslb
        rola
        suba    __exp
        cmpa    #10
        jcc     __fmodf_def             ; calculate x - y * truncf(x/y)
;
	tsx
	inx
	inx
	jsr	__adj_subnormal		; do normalize,AccAB = unbiased exp
	stab	__expdiff+1
	staa	__expdiff
	ldx	#long
	jsr	__adj_subnormal
	stab	__expnew+1		; save @long's exp
	staa	__expnew
	subb	__expdiff+1
	sbca	__expdiff
	stab	__expdiff+1		; expdiff = long's exp - TOS's exp
	staa	__expdiff
;					; Here, fabsf(x)>fabsf(y), expdiff>=0
	tsx
        jsr     __asl8_both
;
	ldab	__expdiff+1
	ldaa	__expdiff
	cmpb	#<24
	sbca	#>24
	bge	__fmodf_zeros		; expdiff>23
;
;					; Here, 0<=expdiff<=23, b=expdiff
	tstb
	beq	__fmodf_03
;
	ldaa	5,x
__fmodf_021:				; scale adjust. TOS>>expdiff
	lsr	2,x
	ror	3,x
	ror	4,x
	rora
	bcc	__fmodf_022
	oraa	#$01			; apply sticky bit
__fmodf_022:
	decb
	bne	__fmodf_021
;
	staa	5,x
;
;	Since 24-bit division is performed using 32-bit operations
;
__fmodf_03:
	jsr	__div32x32x		; @tmp1:AccAB  = @long % TOS
;
	stab	@long+3			; @long = @long % TOS
	staa	@long+2
	ldaa	@tmp1+1
	staa	@long+1
	ldaa	@tmp1
	staa	@long
;
	orab	@long+2			; @long == 0 ?
	orab	@long+1
	orab	@long
	jeq	__fmodf_zeros		; mod=0, return 0.0 with sign
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
	bcc	__fmodf_06
	oraa	#$01			; sticky
__fmodf_06:
	incb
	bne	__fmodf_05
;
	staa	@long+3
;
	ldab	#<-127			; subnormal's exp
	ldaa	#>-127
;					; round up check (subnormal)
	bsr	__fmodf32_rup_check	; if C==1, need round up
	bcc	__fmodf32_done
;	
	inc	@long+2
	bne	__fmodf32_done
	inc	@long+1
	bne	__fmodf32_done
	inc	@long
	bpl	__fmodf32_done		; Still subnormal
;
;	annoying thing here is:
;	  round up carry from the subnormal results in a normal number.
;
	ldab	#<-126
	ldaa	#>-126
	stab	__expnew+1
	staa	__expnew
	bra	__fmodf32_done
;
__fmodf_20:				; round up check (normal)
	ldab	__expnew+1
	ldaa	__expnew
	bsr	__fmodf32_rup_check	; C==1, need round up
	bcc	__fmodf32_done
;
__fmodf32_rup:
	inc	@long+2
	bne	__fmodf32_done
	inc	@long+1
	bne	__fmodf32_done
	inc	@long
	bne	__fmodf32_done
;
	lsr	@long+2
	ror	@long+1
	ror	@long
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
;	round up check, @tmp4 and @tmp4+1
;
__fmodf32_rup_check:
	pshb
	ldab	@long+3
	bpl	__fmodf32_rup_none	; G==0, no round up
	aslb				; (bitb #$7F)
	bne	__fmodf32_rup_occur	; R or S==1, do round up
	ldab	@long+2
	asrb
	pulb
	rts				; if LSB==0, no round up
__fmodf32_rup_none:
	pulb
	clc
	rts
__fmodf32_rup_occur:
	pulb
	sec
	rts
;
;       fmodf(x,y) = x - y * trunc(x/y)
;

__fmodf_def:
        tsx
        inx
        inx
        stx __yadrs                     ; save y's address
;
        jsr __push32                    ; push x
;
        ldx __yadrs
        jsr __push32x                   ; push y
        ldx __yadrs
        jsr __push32x                   ; push y
;
	jsr __divf32tos                 ; @long = @long / TOS  = x / y  , discard TOS y
;
        jsr __f32isNaNorInf             ; Z=1 Inf, C=1 NaN
        bhi __fmodf_ok                  ; jump if Z=0 and C=0
;
        ins                             ; discard TOS y
        ins
        ins
        ins
        ins                             ; discard TOS x
        ins
        ins
        ins
        jmp __fmodf_NaN
;
__fmodf_ok:
	jsr _truncf                     ; @long = truncf(x/y)
;
	jsr __mulf32tos                 ; @long = y * truncf(x/y)       , discard TOS y
;
        jsr __subf32tos                 ; @long = @long - TOS = y*trucf(x/y) - x, dis x
;
        ldaa @long                      ; flip sign
        eora #$80
        staa @long
;
	rts
;
