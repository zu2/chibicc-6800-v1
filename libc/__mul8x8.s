;
;	AccAB = AccA*B
;
	.export __mul8x8
	.code
__mul8x8:
        stab    @tmp1+1
        staa    @tmp1
;
        clra
        clrb
        ldx     #8
__mul8x8_10:
        aslb
        rola
        rol     @tmp1
        bcc     __mul8x8_20
        addb    @tmp1+1
        adca    #0
__mul8x8_20:
        dex
        bne     __mul8x8_10
        rts
