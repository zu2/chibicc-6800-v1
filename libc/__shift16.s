;
;	AccAB << TOS (8bit)
;	AccAB >> TOS (8bit)
;
	.export __shl16
	.export __shr16u
	.export __shr16s
	.code
__shl16:
	tsx
	pshb
	ldab 2,x
	beq noshift
	cmpb #16
	pulb
	bcc zero
shlloop:
	lslb
	rola
	dec 2,x
	bne shlloop
	rts
noshift:
	pulb
	rts
zero:
	clrb
	clra
	rts
;
__shr16u:
	tsx
	pshb
	ldab 2,x
	beq noshift
	cmpb #16
	pulb
	bcc zero
shruloop:
	lsra
	rorb
	dec 2,x
	bne shruloop
	rts
;
__shr16s:
	tsx
	pshb
	ldab 2,x
	beq noshift
	cmpb #16
	pulb
	bcc zero
shrsloop:
	asra
	rorb
	dec 2,x
	bne shrsloop
	rts
