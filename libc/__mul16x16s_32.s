;
;	@long = AccAB*TOS
;
	.export __mul16x16s_32
	.code
;
; 0-1,x return address
; 2-3,x TOS
;
__mul16x16s_32:
	staa @tmp2	; sign flag
	bpl nonneg
	nega
	negb
	sbca #0
nonneg:	stab @tmp1+1
	staa @tmp1
;
	tsx
	ldx 2,x
	stx @long+2
	bpl nonneg_2
;
	ldab @long+3
	ldaa @long+2
	nega
	negb
	sbca #0
	stab @long+3
	staa @long+2
	com @tmp2	; flip sign
;
nonneg_2:
	jsr __mul16x16u_sub
;
	tst @tmp2
	bpl nonneg_3
	jsr __neg32
;
nonneg_3:
	rts
