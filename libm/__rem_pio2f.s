;
;	int __rem_pio2f(float x, float *rp)
;
;	Payne-Hanek reduction
;
;	entry:	@long = x, 127 <= exp(x) <= 254
;		2,x = rp
;	exit:	*rp = r, x = n * pi/2 + r
;		AccAB = n & 3
;
	.export	___rem_pio2f
;
	.data
;
;	__pdat[8k+s] = frac byte k of 2/pi * 2^(s-25)
;	__win[i] = __pdat[exp(x) - 127 + 8i]
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
; 25 * 8 = 200 bytes, exp(x) - 127 + 72 <= 199
__pio2m:	.byte	$c9,$0f,$da,$a2	; round(pi/2 * 2^31)
__rp_exp:	.byte	0
__mc:		.byte	0,0,0,0		; __mc[i] = bits left in __mm[i]
__mm:		.byte	0,0,0,0		; __mm - __mc = 4 (ror 4,x)
;
__rp_ptr:	.byte	0,0
__rp_ct:	.byte	7,8,8		; __rp_mant - __rp_ct = 3 (asl 3,x)
__rp_mant:	.byte	0,0,0
__rp_quad:	.byte	0
__win:		.byte	0,0,0,0,0,0,0,0,0,0
__acc:		.byte	0,0,0,0,0,0,0,0,0,0
__frac		.equ	__acc		; __acc after __rp_mul
;
	.code
;
___rem_pio2f:
;	@long = seeeeeee emmmmmmm mmmmmmmm mmmmmmmm
;	AccB  = eeeeeeee, AccA = mmmmmmm0
	ldaa	@long+1
	ldab	@long
	asla
	rolb
	staa	__rp_mant
	ldx	@long+2
	stx	__rp_mant+1
	subb	#127			; exp(x) >= 127, no borrow
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
	tab
;
;	__acc = AccA:__acc+1..+8:AccB
;	__acc = __win * (2^23 + (__rp_mant >> 1)) mod 2^80
	ldx	#$0708
	stx	__rp_ct
	ldaa	#8
	staa	__rp_ct+2
	ldaa	__acc+0
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
	asl	3,x
	bcc	__rp_skip
	staa	__acc+0
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
	adca	__win+0
__rp_skip:
	dec	0,x
	jne	__rp_mul
	inx
	cpx	#__rp_ct+3
	jne	__rp_mul
	staa	__acc+0
;
;	__frac+0 = qqffffff, q = quadrant, f = fraction
;
	ldaa	__frac
	rola
	rola
	rola
	anda	#3
	ldab	@long
	bpl	__rp_qpos
	nega
__rp_qpos:
	staa	__rp_quad
;
	ldaa	__frac
	anda	#$3f
	staa	__frac
;
;	__frac >= 2^53: __frac = 2^54 - __frac, |n| = q + 1
;
	bita	#$20
	beq	__rp_nocomp
	neg	__frac+6
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
	staa	__frac
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
	eora	#$3f			; = ~A & $3f, b7:b6 = 0
	staa	__frac
__rp_c9:
	ldab	@long
	bmi	__rp_cneg
	inc	__rp_quad
	bra	__rp_cdone
__rp_cneg:
	dec	__rp_quad
__rp_cdone:
	com	@long			; sign(r) = -sign(x)
__rp_nocomp:
;
;	__frac = AccA:AccB:__frac+2..+6
	ldx	#56			; X = 56 - shifts
	ldaa	__frac
	ldab	__frac+1
__rp_bskip:
	tsta
	bne	__rp_norm
	pshb
	ldab	__frac+2
	ldaa	__frac+3
	staa	__frac+2
	ldaa	__frac+4
	staa	__frac+3
	ldaa	__frac+5
	staa	__frac+4
	ldaa	__frac+6
	staa	__frac+5
	clr	__frac+6
	pula
	dex
	dex
	dex
	dex
	dex
	dex
	dex
	dex
	bne	__rp_bskip
	jmp	__rp_zero
__rp_norm:
	tsta
	bmi	__rp_ndone
	asl	__frac+6
	rol	__frac+5
	rol	__frac+4
	rol	__frac+3
	rol	__frac+2
	rolb
	rola
	dex
	bne	__rp_norm
	jmp	__rp_zero
__rp_ndone:
;
;	*rp = frac * pi/2
;	__frac+4..+6 dropped
;
	staa	__mm+0
	stab	__mm+1
	ldaa	__frac+2
	staa	__mm+2
	ldaa	__frac+3
	staa	__mm+3
	stx	__rp_ptr
	ldab	__rp_ptr+1
	addb	#72			; __rp_exp = X + 72 = 128 - shifts
	stab	__rp_exp
;
;	AccA:__win+1:__win+2:AccB = __mm * __pio2m >> 32
	ldx	#0
	stx	__win+1
	ldx	#$0808
	stx	__mc+0
	stx	__mc+2
	ldx	#__mc+3
	clra
	clrb
__rp_m2:
	ror	4,x
	bcc	__rp_m3
	addb	__pio2m+3
	stab	__win+3
	ldab	__win+2
	adcb	__pio2m+2
	stab	__win+2
	ldab	__win+1
	adcb	__pio2m+1
	stab	__win+1
	adca	__pio2m+0
	ldab	__win+3
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
;	AccA:__win+1..+3 >= 2^30, since __mm, __pio2m >= 2^31
;
	ldab	__rp_exp
	tsta
	bmi	__rp_e1
	asl	__win+3
	rol	__win+2
	rol	__win+1
	rola
	bra	__rp_e0
__rp_e1:
	incb
__rp_e0:
;	AccA  = 1mmmmmmm, AccB = eeeeeeee
;	@long = seeeeeee emmmmmmm __win+1  __win+2
	asla
	lsrb
	rora
	staa	@long+1
	ldaa	@long
	anda	#$80
	aba
	staa	@long
	ldaa	__win+1
	staa	@long+2
	ldaa	__win+2
	staa	@long+3
	ldaa	__win+3
	bpl	__rp_nornd
	inc	@long+3			; mantissa overflow -> exponent + 1
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
	ldab	__rp_quad
	andb	#3
	clra
	rts
;
__rp_zero:
	ldx	#0
	stx	@long+2
	stx	@long
	bra	__rp_out
