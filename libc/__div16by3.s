;
;	@tmp2 = AccAB / 3
;
        .export __div16by3s
	.export __div16by6s
	.export __div16by12s
	.export __div16by3u
        .export __div16by6u
	.export __div16by12u
	.code

__div16by3s:
        tsta
	bpl __div16by3u
	nega
	negb
	sbca #0
        bra __div3
;
__div16by6s:
        tsta
	bpl __div16by6u
	nega
	negb
	sbca #0
        bra __div6
;
__div16by12s:
        tsta
	bpl __div16by12u
	nega
	negb
	sbca #0
        lsra
        rorb
__div6:
        lsra
        rorb
__div3:
        jsr __div16by3u
	nega
	negb
	sbca #0
        rts
;
__div16by12u:
        lsra
        rorb
__div16by6u:
        lsra
        rorb
;
; see. Hacker's Delight 2nd Edition Figure 10-8
;
; work area
;   n : @tmp1
;   q : @tmp2
;   r : @tmp3
;
__div16by3u:
        stab    @tmp1+1         ; n = AccAB
        staa    @tmp1
        lsra
        rorb
        lsra
        rorb
        stab    @tmp2+1         ; n>>2
        staa    @tmp2
        lsra
        rorb
        lsra
        rorb
        addb    @tmp2+1         ; (n>>2) + (n>>4)
        adca    @tmp2
        stab    @tmp2+1         ; q = (n>>2) + (n>>4)
        staa    @tmp2
        lsra
        rorb
        lsra
        rorb
        lsra
        rorb
        lsra
        rorb
        addb    @tmp2+1
        adca    @tmp2
        stab    @tmp2+1         ; q = q + (q>>4)
        staa    @tmp2
        tab
        clra
        addb    @tmp2+1
        adca    @tmp2
        stab    @tmp2+1         ; q = q + (q>>8)
        staa    @tmp2
        aslb
        rola
        addb    @tmp2+1         ; AccAB = q*3
        adca    @tmp2
        nega
        negb
        sbca    #0              ; AccAB = -q*3
        addb    @tmp1+1
        adca    @tmp1
        stab    @tmp3+1         ; r = n - q*3
        staa    @tmp3
        aslb
        rola
        aslb
        rola
        addb    @tmp3+1         ; AccAB = r + (r<<2)
        adca    @tmp3
        addb    #5              ; AccAB = r + 5 + (r<<2)
        adca    #0
        lsra
        rorb
        lsra
        rorb
        lsra
        rorb
        lsra
        rorb                    ; AccAB = ((r + 5 + (r << 2)) >> 4)
        addb    @tmp2+1         ; AccAB = q + ((r + 5 + (r << 2)) >> 4)
        adca    @tmp2
        rts
