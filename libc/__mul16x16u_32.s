;
;	@long = AccAB*TOS
;
	.export __mul16x16u_32
	.export __mul16x16u_sub
	.code
;
; 0-1,x return address
; 2-3,x TOS
;
offset	.equ 2		; long+1(loop counter) to long+3 offset
;
__mul16x16u_32:
	stab @tmp1+1	; multiplicand
	staa @tmp1
;
        tsx
	ldx 2,x
	stx @long+2	; multiplier
;
__mul16x16u_sub:
	ldx #$0808	; loop count
	stx @long
;
	ldx #long+1
	clra
	clrb
;
loop:
	ror offset,x
	bcc skip
	addb @tmp1+1
	adca @tmp1
skip:
	rora
	rorb
	dec 0,x
	bne loop
	ror offset,x
	dex
	cpx #long-1
	bne loop
	stab @long+1
	staa @long
        rts
;
