;
;	AccAB << TOS (8bit)
;	AccAB >> TOS (8bit)
;
	.export __shl8
	.export __shr8u
	.export __shr8s
	.code
;
; SP+0: return adrs
; SP+2: TOS (shift count)
;
__shl8:
        stx @tmp1
	tsx
	ldaa 2,x
	beq shl8_ret
	cmpa #16
	bcc zero
        clra
shlloop:
	lslb
	rola
	dec 2,x
	bne shlloop
shl8_ret:
        ldx @tmp1
	rts
zero:
	clrb
	clra
        ldx @tmp1
	rts
;
__shr8u:
        stx @tmp1
	tsx
	ldaa 2,x
	beq shru_ret
	cmpa #8
	bcc zero
shruloop:
	lsrb
	deca
	bne shruloop
shru_ret:
        ldx @tmp1
	rts
;
__shr8s:
        stx @tmp1
	tsx
	ldaa 2,x
	beq shr_ret
	cmpa #8
	bcc shr_ret2
shrsloop:
	asrb
        deca
	bne shrsloop
shr_ret:
        ldx @tmp1
	rts
shr_ret2:
        aslb            ; return (AccB>=0)? 0: 255;
        ldab #0
        sbcb #0
        ldx @tmp1
	rts


