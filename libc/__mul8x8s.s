;
;	AccAB = AccA*B signed
;
	.export __mul8x8s
	.code
__mul8x8s:
        staa    @tmp2           ; 4 2
        bpl     plus1           ; 4 2
        nega                    ; 2 1
plus1:
        tstb                    ; 2 1
        bpl     plus2           ; 4 2
        negb                    ; 2 1
        com     @tmp2           ; 6 3
plus2:
        jsr     __mul8x8u       ; 9 3
        tst     @tmp2           ; 6 3
        bpl     ret             ; 4 2
        nega                    ; 2 1
        negb                    ; 2 1
        sbca    #0              ; 2 2
ret:
        rts                     ; 5 1
