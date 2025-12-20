;
;	signed modulo,divide
;	AccAB = AccAB % TOS
;	AccAB = AccAB / TOS
;
	.export __div16x16s
	.export __rem16x16s
;
;	AccAB = AccAB % TOS
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
;	AccAB = AccAB / TOS
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
	ldab @tmp2+1    ; The div16x16a returns the bit-inverted quotient.
	ldaa @tmp2
	tst @tmp4
	bpl __div4
        addb #1
        adca #0
	ins
	ins
	rts
__div4:
        comb
        coma
        ins
        ins
        rts
