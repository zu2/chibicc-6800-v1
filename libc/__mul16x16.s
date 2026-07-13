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
	oraa 2,x
	bne up8bit_nonzero
	ldaa 3,x
	jmp __mul8x8u
;
up8bit_nonzero:
	stab @tmp1+1
        ldx 2,x
        stx @tmp2
;
        ldx #8
	clrb
	clra
        lsr @tmp2+1
loop_1:
        bcc skip_1              ; 4 2
	addb @tmp1+1            ; 3 2
	adca @tmp1              ; 3 2
skip_1:
        rora                    ; 2 1
        rorb                    ; 2 1
        ror @tmp2+1             ; 6 3
        dex                     ; 4 1
	bne loop_1              ; 4 2   ↑ 28cyc/loop
;
        ldx #8
	ldaa @tmp2
        lsra
loop_2:
        bcc skip_2
	addb @tmp1+1
skip_2:
        rorb
        rora
        dex
	bne loop_2
        ldab @tmp2+1
        rts
