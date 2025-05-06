;
;	@long = @long / TOS
;	@tmp1:AccAB = @long % TOS
;
	.export __div32x32u
	.export __div32x32s
	.export __rem32x32u
	.export __rem32x32s
	.export __div32x32	; @long / TOS
	.export __div32x32x	; @long / (2-5,x)
	.code

;
;	@long = @long % TOS	signed
;
;	remainder has the same sign as the dividend.
;
__rem32x32s:
	tsx
	ldab 2,x
	bpl __rem32x32s_10
	inx
	inx
	jsr __neg32x
__rem32x32s_10:
	ldab @long
	stab @tmp2+1		; save sign
	bpl __rem32x32s_20
	jsr __neg32
__rem32x32s_20:
	tsx
	jsr __div32x32x
	stab @long+3
	staa @long+2
	ldab @tmp1+1
	ldaa @tmp1
	stab @long+1
	staa @long
	ldab @tmp2+1
	bpl __pullret
	jsr __neg32
	bra __pullret
;
;	@long = @long % TOS	unsigned
;
__rem32x32u:
	tsx
	jsr __div32x32x
	stab @long+3
	staa @long+2
	ldab @tmp1+1
	ldaa @tmp1
	stab @long+1
	staa @long
	bra __pullret
;
;	@long = @long / TOS	signed
;	@tmp1+1: sign flag
;
__div32x32s:
	tsx
	ldab 2,x
	stab @tmp2+1		; sign
	bpl __div32x32s_10
	inx
	inx
	jsr __neg32x
__div32x32s_10:
	ldab @long
	bpl __div32x32s_20
	com @tmp2+1		; flip sign
	jsr __neg32
__div32x32s_20:
	tsx
	jsr __div32x32x
	ldab @tmp2+1
	bpl __pullret
	jsr __neg32
	bra __pullret
;
;	@long = @long / TOS	unsigned
;
__div32x32u:
	tsx
	jsr __div32x32x
__pullret:
	ldx 0,x
	ins
	ins
	ins
	ins
	ins
	ins
	jmp 0,x
;
;	@long = @long / (2-5,x)
;	@tmp1:AccAB = @long % TOS
;	@tmp2: loop counter
;
__div32x32:
	tsx
__div32x32x:
	ldab #32
	stab @tmp2
	clra		; work area clear
	clrb
	stab @tmp1+1
	staa @tmp1
loop:
	asl @long+3	; shift divient
	rol @long+2
	rol @long+1
	rol @long
	rolb		; shit work area @tmp1:AccAB
	rola
	rol @tmp1+1
	rol @tmp1
	subb 5,x	; subtract the divisor
	sbca 4,x
	pshb
	psha
	ldab @tmp1+1
	sbcb 3,x
	ldaa @tmp1
	sbca 2,x
	bcs skip
	stab @tmp1+1	; subtracted.
	staa @tmp1
	pula
	pulb
	inc @long+3	; set the lower bit of the quotient
	bra next
skip:
	pula		; can't substract. pull it back.
	pulb
	addb 5,x
	adca 4,x
next:
	dec @tmp2
	bne loop
	rts
