;
;	AccAB = AccA*B
;
	.export __mul8x8
	.code
__mul8x8:
        stab    @tmp1+1
;
        clrb
        ldx     #8
__mul8x8_10:
        aslb
        rola
        bcc     __mul8x8_20
        addb    @tmp1+1
        adca    #0
__mul8x8_20:
        dex
        bne     __mul8x8_10
        rts
