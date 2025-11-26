;
;	AccAB = AccAB * IX (IX=0-255)
;
	.export __mul16x8x
	.code
;
; 0-1,x return address
;
__mul16x8x:
        stx @tmp2
        stab @tmp1+1
	staa @tmp1
;
	ldx #8
	clrb
	clra
__mul16x8_50:
        aslb
        rola
        rol @tmp2+1
        bcc __mul16x8_90
	addb @tmp1+1
	adca @tmp1
__mul16x8_90:
	dex
	bne __mul16x8_50
__mul16x8_95:
        rts             ; Result in AccAB
;
