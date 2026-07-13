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
	ror offset,x	; One bit remains in C; it is shifted in.
	dex
	cpx #long-1
	bne loop
	stab @long+1
	staa @long
        rts
;
; Note: Consider later.
;
; The routine has an 8-bit shift-and-add inner loop and a two-byte outer loop.
; Unroll only the outer loop: dec 0,x → dex and ror 2,x → ror @long+n,
;   saving 4 cycles/loop (≥64 cycles total).
; Unrolling both loops would make the code too large.
;
