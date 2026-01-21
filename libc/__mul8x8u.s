;
;	AccAB = AccA*B unsigned
;
	.export __mul8x8u
	.code
__mul8x8u:
        ldx     #8              ; 3 3
        staa    @tmp1           ; 4 2
        clra                    ; 2 1
        lsrb                    ; 2 1   11cyc
loop:
        bcc     skip            ; 4 2
        adda    @tmp1           ; 3 2
skip:
        rora                    ; 2 1
        rorb                    ; 2 1
        dex                     ; 4 1
        bne     loop            ; 4 2   19cyc/loop = 152cyc
        rts
