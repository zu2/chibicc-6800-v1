;
;	AccAB = TOS*AccAB
;
	.export __mul16x16
	.code
__mul16x16:
	stab @tmp1+1
	staa @tmp1
	ldab #16
	stab @tmp2
	tsx
	; 0-1,x return address
	; 2-3,x TOS
	clra
	clrb
__mul16x16_1:
	aslb
	rola
	rol 3,x
	rol 2,x
	bcc __mul16x16_2
	addb @tmp1+1
	adca @tmp1
__mul16x16_2:
	dec @tmp2
	bne __mul16x16_1
	ins
	ins
	ins
	rts
