;
;	signed modulo,divide
;	AccAB = AccAB % TOS
;	AccAB = AccAB / TOS
;
	.export __div16x16s
	.export __rem16x16s
;
;	AccAB = TOS % AccAB
;
__rem16x16s:
	tsx
	staa @tmp4
	bpl __rem1
	nega
	negb
	sbca #0
__rem1: pshb
	psha
	; 0-1,x return address
	; 2-3,x	TOS
	ldab 3,x	; TOS
	ldaa 2,x
	bpl __rem2
	nega
	negb
	sbca #0
__rem2:
	jsr __div16x16	; AccAB = TOS % AccAB
			; Please note that TOS and AccAB are reversed
			;   after this function was called.
	tst @tmp4
	bpl __rem3
	nega
	negb
	sbca #0
__rem3:
	ins
	ins
	rts
;
;	AccAB = TOS / AccAB
;
__div16x16s:
	tsx
	staa @tmp4
	bpl __div2
	nega
	negb
	sbca #0
__div2:
	pshb
	psha
	ldab 3,x
	ldaa 2,x
	bpl __div3
	com @tmp4
	nega
	negb
	sbca #0
__div3:
	jsr __div16x16	; @tmp2 = TOS / AccAB
			; Please note that TOS and AccAB are reversed
			;   after this function was called.
	ldab @tmp2+1
	ldaa @tmp2
	tst @tmp4
	bpl __div4
	nega
	negb
	sbca #0
__div4:
	ins
	ins
	rts
