;
;	AccAB = IX / AccB
;
	.export __div16x8
	.export __mod16x8
	.code
;
__mod16x8:
	tsx
	ldx 2,x
	bsr divide
	rts
;
__div16x8:
	tsx
	ldx 2,x
	bsr divide
	ldab @tmp2+1
	ldaa @tmp2
	comb
	coma
	rts
;
;	@tmp2 = IX / AccB
;	AccAB: Remainder
;
divide:
	stx @tmp2	; dividend
	stab @tmp1+1	; divisor
	ldx #16		; loop counter
	clra
	clrb		; carry also cleared
loop:
	rol @tmp2+1	; shift dividend
	rol @tmp2
	rolb
	rola
	subb @tmp1+1	; divisor
	sbca #0
	bcc skip
	addb @tmp1+1
	adca #0		; C must 1
skip:
	dex		; quotient bit will be added to @tmp2+1 at next loop.
	bne loop
	rol @tmp2+1	; Shift to get the remaining quotient 1 bit
	rol @tmp2
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
; one INC/DEC in the loop needs 0..96cycles. (6*16 = 96).
; The final rol/com takes 24 cycles, but it is still faster.
;
