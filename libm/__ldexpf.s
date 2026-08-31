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
;	@long = ldexpf(float value,int exp)
;
;
_ldexpf:
	jsr	__f32isNaNorInf	; @long is NaN or Inf?
	bls	__ldexpf_ret	; yes, return it (C+Z=1)
	jsr	__f32iszero	; @long == 0.0?
	bne	__ldexpf_01
__ldexpf_ret:                   ; return @long
        rts
;
__ldexpf_01:			; @long is not NaN, Inf, 0.0
        tsx
        ldx     2,x
        beq     __ldexpf_ret
;
        ldaa    @long+1
        ldab    @long
        stab    __sign          ; save sign
        anda    #$80
        asla                    ; shift out b7 and AccA=0
        rolb
;
        bne     __ldexpf_03
;                               ; sub normal
        incb
__ldexpf_02:
        subb    #1
        sbca    #0
        asl     @long+3
        rol     @long+2
        rol     @long+1
        bpl     __ldexpf_02
;
__ldexpf_03:
        tsx
        addb    3,x             ; get new exp
        adca    2,x
        subb    #127            ; unbias
        sbca    #0
        stab    @tmp2+1         ; save old exp (unbiased)
        staa    @tmp2
        subb    #128            ; exp>128 ?
        sbca    #0
        jge     __f32Infs       ; return ±INF
        ldab    @tmp2+1
        ldaa    @tmp2
        subb    #<-150          ; exp < -150 ?
        sbca    #>-150
        jlt     __f32zeros      ; Underflow, return ±0.0
;
        ldab    @tmp2+1
        ldaa    @tmp2
        subb    #<-126          ; normal ?
        sbca    #>-126
        jge     __ldexpf_10     ; yes
;                               ; no, sub notmal
;                               ; AccB = -k, k is the right shift count
        ldaa    @long+1
        oraa    #$80            ; set the hidden bit
        staa    @long+1
;
        addb    #24             ; AccB = 24-k, the mask table index
        stab    __mbits
        jsr     __fmsbmask      ; the bit below the new LSB
        jsr     __bit_fmask
        staa    __guard
        ldab    __mbits
        jsr     __fracmask      ; everything under the guard bit
        jsr     __bit_fmask
        staa    __sticky
;
        ldab    __mbits
        subb    #24             ; AccB = -k
__ldexpf_04:
        lsr     @long+1
        ror     @long+2
        ror     @long+3
        incb
        bne     __ldexpf_04
;
        ldaa    __guard
        beq     __ldexpf_05
        ldaa    __sticky
        bne     __ldexpf_06
        ldaa    @long+3
        lsra                    ; a tie goes to the even one
        bcc     __ldexpf_05
__ldexpf_06:
        inc     @long+3
        bne     __ldexpf_05
        inc     @long+2
        bne     __ldexpf_05
        inc     @long+1
;
__ldexpf_05:
        ldab    @long+1
        aslb                    ; carry = bit 23, set only by the round up
        rolb
        andb    #1              ; new exp = 1 if the round up carried, else 0
        bra     __ldexpf_11
;
__ldexpf_10:
        ldab    @tmp2+1
        addb    #127            ; add bias
;
__ldexpf_11:
        asl     @long+1
        asl     __sign
        rorb
        ror     @long+1
        stab    @long
        rts
