;
;	@long << shift
;	@long >> shift
;	The shift amount is given by AccB
;
	.export __shl32
	.export __shr32u
	.export __shr32s
	.export __shl32b
	.export __shr32ub
	.export __shr32sb
	.code
;
__shl32:
	tstb
	beq ret		; If AccB is 0, do nothing.
	cmpb #32
	bcs shl8check
	bra zero	; AccB>=32, return 0
shl8loop:
	ldaa @long+1
	staa @long
	ldaa @long+2
	staa @long+1
	ldaa @long+3
	staa @long+2
	clr @long+3
	subb #8
__shl32b:
shl8check:
	cmpb #8
	bcc shl8loop
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
__shr32u:
	tstb
	beq ret
	cmpb #32
	bcs shru8check
	bra zero
shru8loop:
	ldaa @long+2
	staa @long+3
	ldaa @long+1
	staa @long+2
	ldaa @long
	staa @long+1
	clr  @long
	subb #8
shru8check:
__shr32ub:
	cmpb #8
	bcc shru8loop
shruloop:
	lsr @long
	ror @long+1
	ror @long+2
	ror @long+3
	decb
	bne shruloop
	rts
;
__shr32s:
	tstb
	beq ret
	cmpb #32
	bcs shrs8check
	bra zero
shrs8loop:
	ldaa @long+2
	staa @long+3
	ldaa @long+1
	staa @long+2
	ldaa @long
	clr  @long
	staa @long+1
	bpl shrs8loop2
	com @long
shrs8loop2:
	subb #8
__shr32sb:
shrs8check:
	cmpb #8
	bcc shrs8loop
shrsloop:
	asr @long
	ror @long+1
	ror @long+2
	ror @long+3
	decb
	bne shrsloop
	rts
