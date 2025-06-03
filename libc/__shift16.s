;
;	AccAB << TOS (8bit)
;	AccAB >> TOS (8bit)
;
	.export __shl16
	.export __shr16u
	.export __shr16s
	.code
__shl16:
        stx @tmp1
        bsr __shl16sub
        ldx @tmp1
        rts
__shl16sub:
	tsx
	pshb
	ldab 4,x
	beq noshift
	cmpb #16
	pulb
	bcc zero
shlloop:
	lslb
	rola
	dec 4,x
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
        stx @tmp1
        bsr __shr16usub
        ldx @tmp1
        rts
__shr16usub:
	tsx
	pshb
	ldab 4,x
	beq noshift
	cmpb #16
	pulb
	bcc zero
shruloop:
	lsra
	rorb
	dec 4,x
	bne shruloop
	rts
;
__shr16s:
        stx @tmp1
        bsr __shr16ssub
        ldx @tmp1
        rts
__shr16ssub:
	tsx
	pshb
	ldab 4,x
	beq noshift
	cmpb #16
	pulb
	bcc zero
shrsloop:
	asra
	rorb
	dec 4,x
	bne shrsloop
	rts
