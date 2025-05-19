;
;	make fractional part MSB mask
;	AccB:0-23
;	return	IX
;		AccB==0	-> $80 00 00
;		AccB==1	-> $40 00 00
;		AccB==2	-> $20 00 00
;			:
;		AccB==22 ->$00 00 02
;		AccB==23 ->$00 00 01
;
	.export	__fmsbmask
	.export	__fracmask
	.export __bit_fmask
	.export __and_fmask
	.export __and_not_fmask
;
	.code
;
__fmsbmask:				; IX = msb_table[3*(B%8)+2-B/8]
	pshb
	andb	#7			; AccB %= 8
	tba				; AccA = AccB*3
	aba
	aba
	pulb
	lsrb				; AccB /= 8
	lsrb
	lsrb
	sba				; AccA -= AccB
	clrb
	adda	#<__msb_table+2
	adcb	#>__msb_table+2
	staa	@tmp1+1
	stab	@tmp1
	ldx	@tmp1
	rts
;
;	make fractional part mask
;	AccB:0-23
;	return	IX 
;		AccB==0	-> $7F FF FF
;		AccB==1	-> $3F FF FF
;		AccB==2	-> $1F FF FF
;			:
;		AccB==22 ->$00 00 01
;		AccB==23 ->$00 00 00
;
__fracmask:				; IX = frac_table[5*(B%8)+7-B/8]
	pshb
	andb	#7			; AccB %= 8
	tba				; AccA = AccB*5
	asla
	asla
	aba
	pulb
	lsrb				; AccA -= AccB/8
	lsrb
	lsrb
	sba
	clrb
	adda	#<__frac_table+7
	adcb	#>__frac_table+7
	staa	@tmp1+1
	stab	@tmp1
	ldx	@tmp1
	rts
;
;
__msb_table:
	.byte	$00,$00,$80
	.byte	$00,$00,$40
	.byte	$00,$00,$20
	.byte	$00,$00,$10
	.byte	$00,$00,$08
	.byte	$00,$00,$04
	.byte	$00,$00,$02
	.byte	$00,$00,$01
;	.byte	$00,$00			; Reuse the first 2bytes of frac_table
__frac_table:
	.byte	$00,$00,$FF,$FF,$FF
	.byte	$00,$00,$7F,$FF,$FF
	.byte	$00,$00,$3F,$FF,$FF
	.byte	$00,$00,$1F,$FF,$FF
	.byte	$00,$00,$0F,$FF,$FF
	.byte	$00,$00,$07,$FF,$FF
	.byte	$00,$00,$03,$FF,$FF
	.byte	$00,$00,$01,$FF,$FF
	.byte	$00,$00,$00,$FF,$FF
;
;	mantissa & fmask
;
__bit_fmask:
	ldaa	0,x
	anda	@long+1
	bne	__bit_fmask_ret
	ldaa	1,x
	anda	@long+2
	bne	__bit_fmask_ret
	ldaa	2,x
	anda	@long+3
__bit_fmask_ret:
	rts
;
;	mantissa &= fmask
;
__and_fmask:
	ldab	0,x
	andb	@long+1
	stab	@long+1
	ldab	1,x
	andb	@long+2
	stab	@long+2
	ldab	2,x
	andb	@long+3
	stab	@long+3
	rts
;
;	mantissa &= ~fmask
;
__and_not_fmask:
	ldab	0,x
	comb
	andb	@long+1
	stab	@long+1
	ldab	1,x
	comb
	andb	@long+2
	stab	@long+2
	ldab	2,x
	comb	
	andb	@long+3
	stab	@long+3
	rts
