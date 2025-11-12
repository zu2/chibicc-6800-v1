;
;	AccAB = TOS*AccAB
;
	.export __mul16x16
	.code
;
; 0-1,x return address
; 2-3,x TOS
;
__mul16x16:
        tsx
	staa @tmp1
        bne __mul16x16_20
        ldaa 2,x
        bne __mul16x16_20
;
        ldaa 3,x
        jmp __mul8x8
;
__mul16x16_20:
	stab @tmp1+1
        ldx 2,x
        stx @tmp2
	clra
	clrb
__mul16x16_40:
        ldx #8
        lsr @tmp2+1
        bcs __mul16x16_55
        beq __mul16x16_70
__mul16x16_50:
        bcc __mul16x16_60
__mul16x16_55:
	addb @tmp1+1
	adca @tmp1
__mul16x16_60:
        rora
        rorb
        ror @tmp2+1
        dex
	bne __mul16x16_50
;
        ldx #8
__mul16x16_70:
        lsr @tmp2
        bcs __mul16x16_85
        bne __mul16x16_90
        tsx
        tba
        ldab @tmp2+1
        rts
__mul16x16_80:
        bcc __mul16x16_90
__mul16x16_85:
	addb @tmp1+1
	adca @tmp1
__mul16x16_90:
        rora
        rorb
        ror @tmp2
        dex
	bne __mul16x16_80
        ldab @tmp2+1
        ldaa @tmp2
        rts
;
