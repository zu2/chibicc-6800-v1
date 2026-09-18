;
;	MC6800 floating point arithmetic library
;
;	Copyright (c) 2025 by ZUKERAN, shin
;	Released under the MIT license
;
;	https://github.com/zu2/chibicc-6800-v1?tab=License-1-ov-file#readme
;
;	Note: written for chibicc-6800-v1.
;	The code favors small size and speed on the MC6800.
;	Results are correctly rounded to nearest, ties to even.
;	Inf, NaN, +0.0, -0.0, and subnormals are handled correctly.
;	IEEE 754 exceptions are not handled.
;
;
	.export _ldexpf
	.data
__mbits:	.byte	0
__guard:	.byte	0
__sticky:	.byte	0
;
	.code
;
;	@long = ldexpf(float value,int n)
;
;
_ldexpf:
	ldab	@long+1
	aslb
	ldab	@long
	rolb			; get exp in AccB
	incb
	cmpb	#2
	bcs	__ldexpf_slow	; if exp==0 or 255, jump slow path
	tsx
	addb	3,x
	ldaa	2,x
	adca	#0		; AccAB = exp + 1 + n
	bne	__ldexpf_01	; new exp < -1 (subnormal, 0) or > 254 (Inf)
	subb	#1
	bls	__ldexpf_01	; new exp is -1 or 0 (subnormal)
	asl	long+1		; make new exp
	asl	long
	rorb
	ror	long+1
	stab	@long
__ldexpf_ret:                   ; return @long
	rts
__ldexpf_slow:
	tstb
	beq	__ldexpf_ret	; exp+1==0 (exp==255), return @long
	jsr	__f32iszero	; @long == 0.0?
	beq	__ldexpf_ret
	tsx
	ldx	2,x
	beq	__ldexpf_ret	; n==0, return @long
;
__ldexpf_01:			; @long is not NaN, Inf, 0.0
	ldaa	@long+1
	ldab	@long
	stab	__sign		; save sign
	anda	#$80
	asla			; shift out b7 and AccA=0
	rolb
;
	bne     __ldexpf_03
;				; sub normal
        incb
__ldexpf_02:
        subb    #1
        sbca    #0
        asl     long+3
        rol     long+2
        rol     long+1
        bpl     __ldexpf_02
;
__ldexpf_03:
        subb    #127            ; unbias
        sbca    #0
        tsx
        addb    3,x             ; get new exp
        adca    2,x
        bvc     __ldexpf_07
        coma
__ldexpf_07:
        subb    #128            ; exp >= 128 ?
        sbca    #0
        jge     __f32Infs       ; return ±INF
        subb    #<-150-128      ; exp < -150 ?
        sbca    #>-150-128
        jmi     __f32zeros      ; Underflow, return ±0.0
;
        subb    #24             ; normal ?
        sbca    #0
        jpl     __ldexpf_10     ; yes
;                               ; no, subnormal
;                               ; AccB = -k, k is the right shift count
        ldaa    @long+1
        oraa    #$80            ; set the hidden bit
        staa    @long+1
;
        addb    #24
        stab    __mbits
        jsr     __fmsbmask
        jsr     __bit_fmask
        staa    __guard
        ldab    __mbits
        jsr     __fracmask
        jsr     __bit_fmask
        staa    __sticky
;
        ldab    __mbits
        subb    #24             ; AccB = -k
__ldexpf_04:
        lsr     long+1
        ror     long+2
        ror     long+3
        incb
        bne     __ldexpf_04
;
        ldaa    __guard
        beq     __ldexpf_05
        ldaa    __sticky
        bne     __ldexpf_06
        ldaa    @long+3
        lsra
        bcc     __ldexpf_05
__ldexpf_06:
        inc     long+3
        bne     __ldexpf_05
        inc     long+2
        bne     __ldexpf_05
        inc     long+1
;
__ldexpf_05:
        ldab    @long+1
        aslb
        rolb
        andb    #1              ; new exp = 1 if the round up carried, else 0
        bra     __ldexpf_11
;
__ldexpf_10:
        incb                    ; AccB = e + 127
;
__ldexpf_11:
        asl     long+1
        asl     __sign
        rorb
        ror     long+1
        stab    @long
        rts
