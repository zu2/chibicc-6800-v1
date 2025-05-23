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
	.export _ldexpf
	.data
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
__ldexpf_ret:                   ; return @long, *exp = 0;
        rts
;
__ldexpf_01:			; @long is not NaN, Inf, 0.0
        tsx
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
        subb    #<-149          ; exp < -149 ?
        sbca    #>-149
        jlt     __f32zeros      ; Underflow, return ±0.0
;
        ldab    @tmp2+1
        ldaa    @tmp2
        subb    #<-126          ; normal ?
        sbca    #>-126
        jge     __ldexpf_10     ; yes
;                               ; no, sub notmal
        asl     @long+1         ; drop b7
        lsr     @long+1
__ldexpf_04:
        lsr     @long+1
        ror     @long+2
        ror     @long+3
        incb
        bne     __ldexpf_04
;
        ldx     @long+2
        bne     __ldexpf_05
        tst     @long
        jeq     __f32zeros      ; under flow
;
__ldexpf_05:
        clrb                    ; new exp = 0 (sub normal, biased)
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
