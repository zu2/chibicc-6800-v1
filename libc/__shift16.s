;
;	AccAB << TOS
;	AccAB >> TOS
;
	.export __shl16
	.export __shr16u
	.export __shr16s
	.code
__shl16:
	tsx
	tst 2,x
	bne zero
	pshb
	ldab 3,x
	beq noshift
	cmpb #16
	pulb
	bcc noshift
shlloop:
	lslb
	rola
	dec 3,x
	bne shlloop
	rts
noshift:
	ins
	rts
zero:
	clrb
	clra
	rts
;
__shr16u:
	tsx
	tst 2,x
	bne zero
	pshb
	ldab 3,x
	beq noshift
	cmpb #16
	pulb
	bcc zero
shruloop:
	lsra
	rorb
	dec 3,x
	bne shruloop
	rts
;
__shr16s:
	tsx
	tst 2,x
	bne zero
	pshb
	ldab 3,x
	beq noshift
	cmpb #16
	pulb
	bcc zero
shrsloop:
	asra
	rorb
	dec 3,x
	bne shrsloop
	rts
