;
;	AccAB = AccAB*100
;
	.export __mul100
	.code
__mul100:
        stab    @tmp1+1         ; @tmp1 = x
        staa    @tmp1
;
        aslb
        rola
        aslb
        rola
        aslb
        rola
        stab    @tmp2+1
        staa    @tmp2           ; @tmp2 = x<<3, x*8
;
        aslb
        rola                    ; AccAB = x<<4, x*16
;
        addb    @tmp1+1         ; AccAB = (x<<4) + (x<<3) + x, x*16 + x*8 + x == x*25
        adca    @tmp1
        addb    @tmp2+1
        adca    @tmp2
;
        aslb                    ; AccAB<<2 = x*25*4 = x*100 
        rola
        aslb
        rola
;
        rts
