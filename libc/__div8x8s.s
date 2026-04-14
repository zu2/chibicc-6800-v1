;
;	AccB = AccB / 2,s
;
	.export __div8x8s
	.code
;
__div8x8s:
        stab @tmp2      ; sign
        bpl __plus1
        negb
__plus1:
        tsx
        ldaa 2,x
        bpl __plus2
        nega
        com @tmp2
__plus2:
        psha
        jsr __div8x8u
        ins
        tst @tmp2
        bpl __ret
        negb
__ret:  rts
