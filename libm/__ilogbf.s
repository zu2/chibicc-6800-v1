;
;	MC6800 floating point arithmetic library
;
;	Copyright (c) 2026 by ZUKERAN, shin
;	Released under the MIT license
;
;	https://github.com/zu2/chibicc-6800-v1?tab=License-1-ov-file#readme
;
;
	.export _ilogbf
	.data
;
	.code
;
;	AccAB = ilogbf(float x)
;		return unbiased exponent 
;
;	NaN,Inf: AccAB = INT_MAX
;	0.0f   : AccAB = INT_MIN (FP_ILOGB0)
;
_ilogbf:
	ldaa	@long+1		; 1 to 254 is an ordinary number
	ldab	@long
	asla
	rolb
	beq	ilogbf_0	; exp==0: zero or subnormal
	cmpb	#$FF
	beq	ret_nan		; NaN and Inf both return INT_MAX
	clra			; return exponent in AccA:B
	subb	#127
	sbca	#0
	rts
;
ilogbf_0:
	jsr	__f32iszero	; @long == 0.0?
	bne	ilogbf_2	; No, subnormal
ret_zero:	                ; ilobgf(0.0) or ilobgf(-0.0)
	ldab	#<-32768	; return FP_ILOGB0 (INT_MIN)
	ldaa	#>-32768
        rts
ret_nan:			; ilogbf(NaN) -> FP_ILOGBNAN
ret_inf:			; ilogbf(Inf) or ilogbf(-Inf) -> INT_MAX
	ldab	#<32767		; return FP_ILOGBNAN (INT_MAX)
	ldaa	#>32767
	rts
;                               ; sub normal
ilogbf_2:
	ldab	#<-126
	ldaa	#>-126
ilogbf_3:
        decb                    ; exp range -126 to -149, AccA stays $FF
        asl     @long+3
        rol     @long+2
        rol     @long+1
        bpl     ilogbf_3
	rts

