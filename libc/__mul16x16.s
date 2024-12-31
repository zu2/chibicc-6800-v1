;
;	AccAB = TOS*AccAB
;
	.export __mul16x16
	.code
__mul16x16:
	pshb
	psha
	ldab #16
	pshb
	tsx
	; 0,x	counter
	; 1-2,x AccAB
	; 3-4,x return address
	; 5-6,x TOS
	clra			; AB now becomes our 16bit
	clrb			; work register

	; Rotate through the number
nextbit:
	aslb
	rola
	rol	6,x
	rol	5,x
	bcc noadd
	addb 2,x
	adca 1,x
noadd:
	dec 0,x
	bne nextbit
	; For a 16x16 to 32bit just store 4-5,x into sreg
	ins
	ins
	ins
	rts
