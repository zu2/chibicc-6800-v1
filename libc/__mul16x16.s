;
;	AccAB = TOS*AccAB
;
	.export __mul16x16
	.code
__mul16x16:
	stab @tmp1+1
	staa @tmp1
	tsx
	tst 2,x		; TOS < 256 ?
	bne  __mul16x16_10
	ldab 3,x
	stab 2,x
	clr 3,x
	ldab #8
	bra __mul16x16_11
__mul16x16_10:
	ldab #16
__mul16x16_11:
	stab @tmp2
	; 0-1,x return address
	; 2-3,x TOS
	clra
	clrb
__mul16x16_50:
	aslb
	rola
	rol 3,x
	rol 2,x
	bcc __mul16x16_90
	addb @tmp1+1
	adca @tmp1
__mul16x16_90:
	dec @tmp2
	bne __mul16x16_50
	rts
