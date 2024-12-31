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
	ldx #16		; loop counter
	clra
	clrb		; carry is also cleared
loop:
	rol @tmp2+1	; shift divient
	rol @tmp2
	rolb
	rola
	subb @tmp1+1	; divisor
	sbca @tmp1
	bcc skip
	addb @tmp1+1
	adca @tmp1	; C must 1
skip:	dex		; quotient bit will be added to @tmp at next loop.
	bne loop
	rol @tmp2+1
	rol @tmp2
	com @tmp2+1
	com @tmp2
	rts
;
;	Note:
;
;	In this program, I stop manipulating @tmp every time the loop goes through,
;	and then make the numbers add up at the end.
;
;	By replacing inc @tmp1+1 with sec in the loop, 6 cycles are saved per pullback.
;	This amounts to max 96 cycles for 16 iterations. 6*16 = 96
;
;	The rol/com takes extra time, but this is 24 cycles.
;
