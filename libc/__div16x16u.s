;
;	unsigned modulo,divide
;	AccAB = AccAB % TOS
;	AccAB = AccAB / TOS
;
	.export __div16x16u
	.export __rem16x16u
;
;	AccAB = TOS % AccAB
;
__rem16x16u:
	tsx
__rem1: pshb
	psha
	; 0-1,x return address
	; 2-3,x	TOS
	ldab 3,x	; TOS
	ldaa 2,x
__rem2:
	jsr __div16x16	; AccAB = TOS % AccAB
			; Please note that TOS and AccAB are reversed
			;   after this function was called.
__rem3:
	ins
	ins
	rts
;
;	AccAB = TOS / AccAB
;
__div16x16u:
	tsx
__div2:
	pshb
	psha
	ldab 3,x	; TOS
	ldaa 2,x
__div3:
	jsr __div16x16	; @tmp2 = TOS / AccAB
			; Please note that TOS and AccAB are reversed
			;   after this function was called.
	ldab @tmp2+1
	ldaa @tmp2
__div4:
	ins
	ins
	rts
