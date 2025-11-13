;
;	@tmp2 = TOS / AccAB
;	AccAB = TOS % AccAB
;
	.export __div16x16
	.code
__div16x16:
	tsx
	ldx 2,x
	stx @tmp2	; divient
	stab @tmp1+1	; divisor
	staa @tmp1
        ldx #8
	clra
	clrb		; carry is also cleared
__div16x16_1:
	rol @tmp2 	; shift divient
	rolb
	rola
	subb @tmp1+1	; divisor
	sbca @tmp1
	bcc __div16x16_2
	addb @tmp1+1
	adca @tmp1	; C must 1
__div16x16_2:
        dex
	bne __div16x16_1
        rol @tmp2
        ldx #8
        clc
__div16x16_3:
	rol @tmp2+1 	; shift divient
	rolb
	rola
	subb @tmp1+1	; divisor
	sbca @tmp1
	bcc __div16x16_4
	addb @tmp1+1
	adca @tmp1	; C must 1
__div16x16_4:
        dex
	bne __div16x16_3
        rol @tmp2+1
	com @tmp2+1
	com @tmp2
	rts
;
; Note:
; Direct memory operations on the MC6800 are slow.
; INC/DEC/ROL require 6 cycles for direct pages.
; With index addressing uses 7 cycles.

; To eliminate direct memory operations in the loop, quotient bit uses the carry of subtraction.
; The carry is put into the lower digits in the next loop.
; The last loop bit is not reflected in the quotient, ROL is executed once when the loop is exited.
;
; Also, the quotient bit was inverted, COM operation is required when the loop is exited.
; 
; Delete one INC/DEC from the loop saves 6*16 = 96 cycles.
; The final rol/com takes 24 cycles, but it is still faster.
;
