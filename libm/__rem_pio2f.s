;
;	int __rem_pio2f(float x, float *rp)
;
;	The caller must keep |x| in the range sinf and friends use, from 1.0
;	up to FLT_MAX. This is the same bound the callers apply with W_PH, and
;	it is the lowest exponent, 127, that the table window covers: p is
;	exponent minus 136, so p starts at minus nine. A smaller argument reads the
;	table below its start and the answer is wrong, without any sign of it.
;	Over that range the fraction is never zero: enumerating
;	the best approximations from below for every exponent puts the closest
;	approach at 2^-29.9, well above the 2^-46 the field resolves. The loop
;	bound below still catches a zero, because a caller outside that range
;	gets no such guarantee.
;	Payne-Hanek reduction. @long holds x on entry, 2,x holds rp.
;	The quadrant comes back in AccA:B and *rp gets the reduced value.
;
	.export	___rem_pio2f
;
	.data
;
; 2/pi in eight pre shifted copies, interleaved so that byte 8k+s holds
; copy s at index k. A window starts at __ptab+p and its ten bytes sit
; eight apart, so the address is one addition and no shift loop is needed.
; The label sits seven bytes below the data because p never goes under seven.
;
__ptab		.equ	__pdat+9
__pbase		.equ	__ptab-9
__pdat:
	.byte	$00,$00,$00,$00,$00,$00,$00,$00
	.byte	$00,$00,$00,$00,$00,$00,$00,$00
	.byte	$00,$00,$01,$02,$05,$0a,$14,$28
	.byte	$51,$a2,$45,$8b,$17,$2f,$5f,$be
	.byte	$7c,$f9,$f3,$e6,$cc,$98,$30,$60
	.byte	$c1,$83,$06,$0d,$1b,$36,$6d,$db
	.byte	$b7,$6e,$dc,$b9,$72,$e4,$c9,$93
	.byte	$27,$4e,$9c,$39,$72,$e4,$c8,$91
	.byte	$22,$44,$88,$10,$20,$41,$82,$05
	.byte	$0a,$15,$2a,$54,$a9,$52,$a5,$4a
	.byte	$94,$29,$53,$a7,$4f,$9f,$3f,$7f
	.byte	$fe,$fc,$f8,$f0,$e1,$c2,$84,$09
	.byte	$13,$27,$4e,$9d,$3a,$75,$ea,$d5
	.byte	$ab,$57,$af,$5f,$be,$7d,$fa,$f4
	.byte	$e8,$d1,$a3,$47,$8f,$1f,$3e,$7d
	.byte	$fa,$f5,$ea,$d4,$a9,$53,$a6,$4d
	.byte	$9a,$34,$69,$d3,$a6,$4d,$9b,$37
	.byte	$6e,$dd,$bb,$77,$ee,$dc,$b8,$70
	.byte	$e0,$c0,$81,$03,$06,$0d,$1b,$36
	.byte	$6d,$db,$b6,$6d,$db,$b6,$6c,$d8
	.byte	$b1,$62,$c5,$8a,$14,$29,$52,$a5
	.byte	$4a,$95,$2b,$56,$ac,$59,$b3,$66
	.byte	$cc,$99,$32,$64,$c9,$93,$27,$4f
	.byte	$9e,$3c,$78,$f1,$e2,$c4,$88,$10
	.byte	$21,$43,$87,$0e,$1c,$39,$72,$e4
; 200 bytes, reached through __ptab-9 to __ptab+190
__pio2m:	.byte	$c9,$0f,$da,$a2	; pi over two, scaled by 2^31 and rounded
__rp_exp:	.byte	0
__mc:		.byte	0,0,0,0		; one count per byte of the multiplier
__mm:		.byte	0,0,0,0		; the multiplier, and ror 4,x needs exactly four
;
__rp_ptr:	.byte	0,0
__rp_ct:	.byte	7,8,8		; one count per byte of m, and asl 3,x needs exactly three
__rp_mant:	.byte	0,0,0
__rp_quad:	.byte	0
__win:		.byte	0,0,0,0,0,0,0,0,0,0
__acc:		.byte	0,0,0,0,0,0,0,0,0,0
__frac		.equ	__acc		; the accumulator is spent once the product is out
;
	.code
;
___rem_pio2f:
	ldaa	@long+1
	ldab	@long
	asla				; the hidden bit leaves in the carry, which is fine:
	rolb				; the primed accumulator covers it. AccB gets the exponent
	staa	__rp_mant
	ldx	@long+2
	stx	__rp_mant+1
	subb	#127			; the base is nine below, so p never goes negative here
	clra
	addb	#<__pbase
	adca	#>__pbase
	staa	__rp_ptr
	stab	__rp_ptr+1
	ldx	__rp_ptr
	ldaa	0,x
	staa	__win+0
	staa	__acc+0
	ldaa	8,x
	staa	__win+1
	staa	__acc+1
	ldaa	16,x
	staa	__win+2
	staa	__acc+2
	ldaa	24,x
	staa	__win+3
	staa	__acc+3
	ldaa	32,x
	staa	__win+4
	staa	__acc+4
	ldaa	40,x
	staa	__win+5
	staa	__acc+5
	ldaa	48,x
	staa	__win+6
	staa	__acc+6
	ldaa	56,x
	staa	__win+7
	staa	__acc+7
	ldaa	64,x
	staa	__win+8
	staa	__acc+8
	ldaa	72,x
	staa	__win+9
	tab				; AccB is the accumulator's last byte
;
;
					; the hidden bit makes the first add a copy, so W is
					; already in the accumulator from the copy above
	ldx	#$0708
	stx	__rp_ct
	ldaa	#8
	staa	__rp_ct+2
	ldaa	__acc+0			; AccA holds the top accumulator byte
	ldx	#__rp_ct
__rp_mul:
	aslb
	rol	__acc+8
	rol	__acc+7
	rol	__acc+6
	rol	__acc+5
	rol	__acc+4
	rol	__acc+3
	rol	__acc+2
	rol	__acc+1
	rola
	asl	3,x			; m is only read, so one byte at a time is enough
	bcc	__rp_skip
	staa	__acc+0			; free AccA for the add below
	addb	__win+9
	ldaa	__acc+8
	adca	__win+8
	staa	__acc+8
	ldaa	__acc+7
	adca	__win+7
	staa	__acc+7
	ldaa	__acc+6
	adca	__win+6
	staa	__acc+6
	ldaa	__acc+5
	adca	__win+5
	staa	__acc+5
	ldaa	__acc+4
	adca	__win+4
	staa	__acc+4
	ldaa	__acc+3
	adca	__win+3
	staa	__acc+3
	ldaa	__acc+2
	adca	__win+2
	staa	__acc+2
	ldaa	__acc+1
	adca	__win+1
	staa	__acc+1
	ldaa	__acc+0
	adca	__win+0			; the top byte stays in AccA
__rp_skip:
	dec	0,x
	jne	__rp_mul
	inx
	cpx	#__rp_ct+3
	jne	__rp_mul
	staa	__acc+0
;
;	quadrant is the top two bits of digit 4
;
	ldaa	__frac			; the multiply left the product here
	rola				; a nine bit rotate three times brings bits 7 and 6
	rola				; down to 1 and 0, and the mask drops what rode along
	rola
	anda	#3
	ldab	@long			; the argument sign is still in place, so fold it in now
	bpl	__rp_qpos
	nega
__rp_qpos:
	staa	__rp_quad
;
	ldaa	__frac
	anda	#$3f
	staa	__frac			; AccA keeps it for the test below
;
;	a fraction of one half or more is folded into the next quadrant
;
	bita	#$20
	beq	__rp_nocomp
	neg	__frac+6		; negate the lowest non zero byte, complement above it
	bne	__rp_c5
	neg	__frac+5
	bne	__rp_c4
	neg	__frac+4
	bne	__rp_c3
	neg	__frac+3
	bne	__rp_c2
	neg	__frac+2
	bne	__rp_c1
	neg	__frac+1
	bne	__rp_c0
	ldaa	__frac
	nega
	anda	#$3f
	staa	__frac			; this path needs the mask too
	bra	__rp_c9
__rp_c5:
	com	__frac+5
__rp_c4:
	com	__frac+4
__rp_c3:
	com	__frac+3
__rp_c2:
	com	__frac+2
__rp_c1:
	com	__frac+1
__rp_c0:
	ldaa	__frac
	eora	#$3f			; complement and drop the two bits above the fraction
	staa	__frac
__rp_c9:
	ldab	@long
	bmi	__rp_cneg
	inc	__rp_quad
	bra	__rp_cdone
__rp_cneg:
	dec	__rp_quad		; a negated quadrant counts the other way
__rp_cdone:
	com	@long			; folding the fraction flips the sign of the result
__rp_nocomp:
;
	ldx	#56			; the field is 56 bits wide, so 55 shifts is the limit
	ldaa	__frac			; AccA carries the top byte through every shift below
	ldab	__frac+1		; AccB carries the second byte
__rp_bskip:
	tsta
	bne	__rp_norm		; the usual case leaves after this one test
	pshb				; a whole zero byte goes eight bits at a time
	ldab	__frac+2		; the new second byte, read before it is written over
	ldaa	__frac+3
	staa	__frac+2
	ldaa	__frac+4
	staa	__frac+3
	ldaa	__frac+5
	staa	__frac+4
	ldaa	__frac+6
	staa	__frac+5
	clr	__frac+6
	pula				; the new top byte
	dex
	dex
	dex
	dex
	dex
	dex
	dex
	dex
	bne	__rp_bskip		; seven skips empty X, and only a zero gets that far
	jmp	__rp_zero
__rp_norm:
	tsta
	bmi	__rp_ndone		; bit 47 up means the mantissa sits in AccA:B:__frac+2
	asl	__frac+6
	rol	__frac+5
	rol	__frac+4
	rol	__frac+3
	rol	__frac+2
	rolb
	rola
	dex
	bne	__rp_norm
	jmp	__rp_zero		; the 56th pass means the fraction was zero
__rp_ndone:
;
;	*rp = frac * pi/2, as one fixed point multiply
;
;	The top 32 bits of the fraction go in whole, so no rounding step is
;	needed here and the constant carries 32 bits of pi over two. The
;	result is one rounding away from exact, where the float route took
;	three.
;
	staa	__mm+0
	stab	__mm+1
	ldaa	__frac+2
	staa	__mm+2
	ldaa	__frac+3
	staa	__mm+3
	stx	__rp_ptr
	ldab	__rp_ptr+1
	addb	#72			; 127 - 55 + shifts, with the bias kept out of the loop
	stab	__rp_exp
;
	ldx	#0			; the accumulator starts empty
	stx	__win+1
	ldx	#$0808
	stx	__mc+0
	stx	__mc+2
	ldx	#__mc+3
	clra				; AccA holds the top accumulator byte
	clrb				; AccB holds the low one
__rp_m2:
	ror	4,x			; the multiplier is only read, so one byte at a time
	bcc	__rp_m3
	addb	__pio2m+3
	stab	__win+3			; free AccB for the add below
	ldab	__win+2
	adcb	__pio2m+2
	stab	__win+2
	ldab	__win+1
	adcb	__pio2m+1
	stab	__win+1
	adca	__pio2m+0
	ldab	__win+3			; ldab keeps the carry
__rp_m3:
	rora
	ror	__win+1
	ror	__win+2
	rorb
	dec	0,x
	jne	__rp_m2
	dex
	cpx	#__mc-1
	jne	__rp_m2
	stab	__win+3
;
;	the product sits in AccA:__win+1:__win+2:__win+3, above 2^30
;
	ldab	__rp_exp
	tsta
	bmi	__rp_e1			; already above 2^31, so the exponent goes up one
	asl	__win+3			; below 2^31, so one shift normalises it
	rol	__win+2
	rol	__win+1
	rola
	bra	__rp_e0
__rp_e1:
	incb
__rp_e0:
	asla				; drop the hidden bit
	lsrb				; the exponent's low bit rides the carry
	rora				; and lands in bit 7, where the format wants it
	staa	@long+1
	ldaa	@long			; last read of the sign before the byte is rebuilt
	anda	#$80
	aba
	staa	@long
	ldaa	__win+1
	staa	@long+2
	ldaa	__win+2
	staa	@long+3
	ldaa	__win+3
	bpl	__rp_nornd
	inc	@long+3			; a carry out of the mantissa runs into the exponent
	bne	__rp_nornd
	inc	@long+2
	bne	__rp_nornd
	inc	@long+1
	bne	__rp_nornd
	inc	@long
__rp_nornd:
__rp_out:
	tsx
	ldx	2,x
	ldaa	@long+3
	staa	3,x
	ldaa	@long+2
	staa	2,x
	ldaa	@long+1
	staa	1,x
	ldaa	@long
	staa	0,x
;
	ldab	__rp_quad		; the result comes back in AccA:B, so build it in B
	andb	#3
	clra
	rts
;
__rp_zero:
	ldx	#0
	stx	@long+2
	stx	@long
	bra	__rp_out
