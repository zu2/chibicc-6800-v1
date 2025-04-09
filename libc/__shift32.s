;
;	@long << shift
;	@long >> shift
;	The shift amount is given by AccB
;
	.export __shl32
	.export __shr32u
	.export __shr32s
	.code
;
__shl32x24:
	ldaa @long+3
	staa @long
	clra
	staa @long+3
	staa @long+2
	staa @long+1
	subb #24
	rts
__shl32x16:
	ldaa @long+3
	staa @long+1
	ldaa @long+2
	staa @long
	clr @long+3
	clr @long+2
	subb #16
	rts
__shl32x8:
	ldaa @long+1
	staa @long
	ldaa @long+2
	staa @long+1
	ldaa @long+3
	staa @long+2
	clr @long+3
	subb #8
	rts
;
shl_check:
	cmpb #24
	bcc __shl32x24
	cmpb #16
	bcc __shl32x16
	cmpb #8
	bcc __shl32x8
	tstb		; If AccB is 0, do nothing.
	rts
;
__shl32:
	cmpb #32
	bcc zero	; AccB>=32, return 0
	bsr shl_check
	beq ret		; the shift count is 0/8/16/24, done.
shlloop:
	lsl @long+3
	rol @long+2
	rol @long+1
	rol @long
	decb
	bne shlloop
	rts
zero:
	clrb
	stab @long+3
	stab @long+2
	stab @long+1
	stab @long
ret:	rts
;
;
;
__shr32u:
	cmpb #32
	bcc zero
	bsr shru_check
	beq ret		; the shift count is 0/8/16/24, done.
shruloop:
	lsr @long
	ror @long+1
	ror @long+2
	ror @long+3
	decb
	bne shruloop
	rts
;
shru_check:
	cmpb #24
	bcc __shru32x24
	cmpb #16
	bcc __shru32x16
	cmpb #8
	bcc __shru32x8
	tstb		; If AccB is 0, do nothing.
	rts
;
__shru32x24:
	ldaa @long
	staa @long+3
	clra
	staa @long+2
	staa @long+1
	staa @long+0
	subb #24
	rts
__shru32x16:
	ldaa @long+1
	staa @long+3
	ldaa @long
	staa @long+2
	clr @long+1
	clr @long+0
	subb #16
	rts
__shru32x8:
	ldaa @long+2
	staa @long+3
	ldaa @long+1
	staa @long+2
	ldaa @long
	staa @long+1
	clr @long
	subb #8
	rts
;
shrs32ret:
	clra
	asl @long
	sbca #0
	staa @long+3
	staa @long+2
	staa @long+1
	staa @long
	rts
;
__shr32s:
	cmpb #32
	bcc shrs32ret
	bsr shru_check
	beq ret		; the shift count is 0/8/16/24, done.
shrsloop:
	asr @long
	ror @long+1
	ror @long+2
	ror @long+3
	decb
	bne shrsloop
	rts
;
;
__shrs32x24:
	ldaa @long
	staa @long+3
	asla
	ldaa #0
	sbca #0
	staa @long+2
	staa @long+1
	staa @long
	subb #24
	rts
__shrs32x16:
	ldaa @long+1
	staa @long+3
	ldaa @long
	staa @long+2
	asla
	ldaa #0
	sbca #0
	staa @long+1
	staa @long
	subb #16
	rts
__shrs32x8:
	ldaa @long+2
	staa @long+3
	ldaa @long+1
	staa @long+2
	ldaa @long
	staa @long+1
	asla
	ldaa #0
	sbca #0
	staa @long
	subb #8
	rts
;
shls_check:
	cmpb #24
	bcc __shrs32x24
	cmpb #16
	bcc __shrs32x16
	cmpb #8
	bcc __shrs32x8
	tstb		; If AccB is 0, do nothing.
	rts
;
