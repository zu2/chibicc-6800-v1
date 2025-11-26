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
;
        .data
sign:   .blkb    1
;
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
	stab sign		; save sign
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
	ldab sign
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
	stab sign
	bpl __div32x32s_10
	inx
	inx
	jsr __neg32x
__div32x32s_10:
	ldab @long
	bpl __div32x32s_20
	com sign		; flip sign
	jsr __neg32
__div32x32s_20:
	tsx
	jsr __div32x32x
	ldab sign
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
        tsx
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
;	@tmp2:loop counter (bit) 8→0
;	@tmp2+1:loop counter (byte) 4→0
;       @tmp3:tmp4: divisor
;
__div32x32:
	tsx
__div32x32x:
        stx @tmp1       ; 5 2
        ldx 4,x         ; 6 2
        stx @tmp4       ; 5 2
        ldx @tmp1       ; 4 2
        ldx 2,x         ; 6 2
        stx @tmp3       ; 5 2   / ↑31cyc, 12bytes
;
        ldx #long
	clra		; work area clear
	clrb
	stab @tmp1+1
	staa @tmp1
loop8:
;       pshb
        stab @tmp2+1
        ldab #8
        stab @tmp2
;       pulb
        ldab @tmp2+1
loop:
	asl 0,x
	rolb		; shift work area @tmp1:AccAB
	rola
	rol @tmp1+1
	rol @tmp1
	pshb
	psha
	subb @tmp4+1	; subtract the divisor
	sbca @tmp4
	ldab @tmp1+1
	sbcb @tmp3+1
	ldaa @tmp1
	sbca @tmp3
	bcs skip
	stab @tmp1+1	; subtracted.
	staa @tmp1
	pula
	pulb
        subb @tmp4+1
        sbca @tmp4
	inc 0,x 	; set the lower bit of the quotient
	bra next
skip:
	pula		; can't substract. pull it back.
	pulb
next:
	dec @tmp2
	bne loop
        inx
        cpx #long+4
        bne loop8
	rts
