;
;	AccAB = TOS*AccAB
;
	.export __mul16x16
	.code
__mul16x16:
	stab @tmp1+1
	staa @tmp1
	ldx  @tmp1
	beq __mul16x16_zero
	tsx
        oraa 2,x
        bne __mul16x16_05
        ldaa 3,x                ; here, TOS,tmp1 < 256
        jmp __mul8x8            ; AccAB = AccA * AccB
__mul16x16_05:
        tsx
        ldx  2,x
        beq __mul16x16_zero
	ldaa @tmp1
	bne  __mul16x16_10
        stab @tmp1
	staa @tmp1+1            ; zero clear
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
	rol @tmp1+1
	rol @tmp1
	bcc __mul16x16_90
	addb 3,x
	adca 2,x
__mul16x16_90:
	dec @tmp2
	bne __mul16x16_50
	rts
__mul16x16_zero:
	clrb
	clra
	rts
