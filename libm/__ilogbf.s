;
;	MC6800 floating point arithmetic library
;
;	Copyright (c) 2026 by ZUKERAN, shin
;	Released under the MIT license
;
;	https://github.com/zu2/chibicc-6800-v1?tab=License-1-ov-file#readme
;
;	Note: This program was created for testing chibicc-6800-v1, 
;	and does not pay attention to speed, accuracy, or exception handling.
;
;
	.export _ilogbf
	.data
;
	.code
;
;	AccAB = ilogbf(float x)
;
_ilogbf:
	jsr	__f32isNaNorInf	; @long is NaN or Inf?
	bcs	ret_nan		; @long is NaN, return FP_ILOGBNAN
	beq	ret_inf
	jsr	__f32iszero	; @long == 0.0?
	bne	ilogbf_1
ret_zero:	                ; ilobgf(0.0) or ilobgf(-0.0)
	ldab	#<-32768	; return FP_ILOGB0 (INT_MIN)
	ldaa	#>-32768
        rts
ret_nan:			; ilogbf(NaN) -> FP_ILOGBNAN
ret_inf:			; ilogbf(Inf) or ilogbf(-Inf) -> INT_MAX
	ldab	#<32767		; return FP_ILOGBNAN (INT_MAX)
	ldaa	#>32767
	rts
;
ilogbf_1:			; @long is not NaN, Inf, 0.0
        ldaa    @long+1
        ldab    @long
        anda    #$80
        asla                    ; shift out b7 and AccA=0
        rolb
;
        beq     ilogbf_2
        subb    #127
        sbca    #0
ret:
        rts
;                               ; sub normal
ilogbf_2:
	ldab	#<-126
	ldaa	#>-126
ilogbf_3:
        subb    #1
        sbca    #0
        asl     @long+3
        rol     @long+2
        rol     @long+1
        bpl     ilogbf_3
	rts

