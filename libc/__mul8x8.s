;
;	AccAB = AccA*B
;
	.export __mul8x8
	.code
__mul8x8:
        ldx     #8              ; 3 3
        staa    @tmp1           ; 4 2
        clra                    ; 2 1
        lsrb                    ; 2 1   11cyc
__mul8x8_10:
        bcc __mul8x8_20         ; 4 2
        adda    @tmp1           ; 3 2
__mul8x8_20:
        rora                    ; 2 1
        rorb                    ; 2 1
__mul8x8_30:
        dex                     ; 4 1
        bne     __mul8x8_10     ; 4 2   19cyc/loop = 152cyc
        rts
