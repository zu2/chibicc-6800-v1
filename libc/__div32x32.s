;
;	@tmp3:@tmp4 = TOS / @long
;	@tmp1:AccAB = TOS % @long
;
	.export __div32x32
	.code
__div32x32:
	tsx
	ldx 4,x		; divient
	stx @tmp4
	ldx 2,x
	stx @tmp3
	ldx #32		; loop counter
	clra		; work area clear
	clrb
	stab @tmp1+1
	staa @tmp1
loop:
	rol @tmp4+1	; shift divient
	rol @tmp4
	rol @tmp3+1
	rol @tmp3
	rolb		; shit work area @tmp1:AccAB
	rola
	rol @tmp1+1
	rol @tmp1
	subb @long+3	; subtract the divisor
	sbca @long+2
	pshb
	psha
	ldab @tmp1+1
	sbcb @long+1
	ldaa @tmp1
	sbca @long
	bcs skip
	stab @tmp1+1	; subtracted.
	staa @tmp1
	pulb
	pula
	inc @tmp4+1	; set the lower bit of the quotient
	bra next
skip:
	pulb		; can't substract. pull it back.
	pula
	addb @long+3
	adca @long+2
next:
	dex
	bne loop
	rts
