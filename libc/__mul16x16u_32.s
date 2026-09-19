;
;	@long = AccAB * TOS
;
;	0-1,x return address
;	2-3,x TOS
;
	.export __mul16x16u_32
	.export __mul16x16u_sub
	.code
;
__mul16x16u_32:
	stab @tmp1+1	; multiplicand
	staa @tmp1
;
        tsx
	ldx 2,x
	stx @long+2	; multiplier
;
__mul16x16u_sub:
	ldx #8
	clra
	clrb		; clear Carry
;
loop1:
	ror long+3
	bcc skip1
	addb @tmp1+1
	adca @tmp1
skip1:
	rora
	rorb
	dex
	bne loop1
;
	ror long+3
;
	ldx #8
loop2:
	ror long+2
	bcc skip2
	addb @tmp1+1
	adca @tmp1
skip2:
	rora
	rorb
	dex
	bne loop2
;
	ror long+2
;
	stab @long+1
	staa @long
        rts
;
