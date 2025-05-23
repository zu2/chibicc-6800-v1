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
	.export _frexpf
	.data
;
	.code
;
;	@long = frexpf(float value,int *exp)
;
;
_frexpf:
	jsr	__f32isNaNorInf	; @long is NaN or Inf?
	bcs	__frexpf_ret	; @long is NaN, return it
	beq	__frexpf_ret
	jsr	__f32iszero	; @long == 0.0?
	bne	__frexpf_01
__frexpf_ret:                   ; return @long, *exp = 0;
        tsx
        ldx     2,x
        clr     0,x             ; clear exp
        clr     1,x
        rts
;
__frexpf_01:			; @long is not NaN, Inf, 0.0
        ldaa    @long+1
        ldab    @long
        anda    #$80
        asla                    ; shift out b7 and AccA=0
        rolb
;
        bne     __frexpf_02
;                               ; sub normal
        incb
__frexpf_10:
        subb    #1
        sbca    #0
        asl     @long+3
        rol     @long+2
        rol     @long+1
        bpl     __frexpf_10
;
__frexpf_02:
        subb    #126
        sbca    #0
        tsx
        ldx     2,x
        stab    1,x             ; exp
        staa    0,x
;
        asl     @long+1         ; drop hidden bit
        lsr     @long+1
;
        ldaa    @long
        anda    #$80
        oraa    #$3F
        staa    @long
;
        rts
