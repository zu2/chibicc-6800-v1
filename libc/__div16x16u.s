;
;	unsigned modulo,divide
;	AccAB = AccAB % TOS
;	AccAB = AccAB / TOS
;
	.export __rem16x16u
	.export __div16x16u
;
;	AccAB = AccAB % TOS
;	  0-1,x	return address
;	  2-3,x	TOS
;
	.code
__rem16x16u:
	tsx
__rem1: pshb
	psha
	ldab 3,x	; TOS
	ldaa 2,x
__rem2:
	jsr __div16x16	; AccAB = TOS % AccAB
__rem3:
	ins
	ins
	rts
;
;	AccAB = AccAB / TOS
;	  0-1,x	return address
;	  2-3,x	TOS
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
	ldab @tmp2+1    ; The div16x16 returns the bit-inverted quotient.
	ldaa @tmp2
        comb
        coma
__div4:
	ins
	ins
	rts
