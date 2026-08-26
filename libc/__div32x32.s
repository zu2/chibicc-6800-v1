;
;	@long = @long / TOS
;	@tmp1:AccAB = @long % TOS
;
	.export __div32x32u
	.export __div32x32s
	.export __rem32x32u
	.export __rem32x32s
	.export __div32x32	; @long / TOS
	.export __div32x32ux
	.export __div32x32ubx
	.export __div32x32udx
	.export __div32x32sx
	.export __div32x32sbx
	.export __div32x32sdx
	.export __rem32x32ux
	.export __rem32x32ubx
	.export __rem32x32udx
	.export __rem32x32sx
	.export __rem32x32sbx
	.export __rem32x32sdx
;
        .data
sign:   .blkb    1
;
	.code
;
;	@long = @long / (0-3,x)		unsigned
;
__div32x32ubx:
	clra
__div32x32udx:
	jsr __adx
__div32x32ux:
	stx @tmp1
	ldx 0,x
	stx @tmp3
	ldx @tmp1
	ldx 2,x
	stx @tmp4
	jmp __div32x32_main
;
;	@long = @long / (0-3,x)		signed
;
__div32x32sbx:
	clra
__div32x32sdx:
	jsr __adx
__div32x32sx:
	stx @tmp1
	ldx 0,x
	stx @tmp3
	ldx @tmp1
	ldx 2,x
	stx @tmp4
	ldab @tmp3
	stab sign
	bpl __div32x32sx_10
	ldx #tmp3
	jsr __neg32x
__div32x32sx_10:
	ldab @long
	bpl __div32x32sx_20
	com sign		; flip sign
	jsr __neg32
__div32x32sx_20:
	jsr __div32x32_main
	ldab sign
	bpl __div32x32sx_30
	jsr __neg32
__div32x32sx_30:
	rts
;
;	@long = @long % (0-3,x)		unsigned
;
__rem32x32ubx:
	clra
__rem32x32udx:
	jsr __adx
__rem32x32ux:
	stx @tmp1
	ldx 0,x
	stx @tmp3
	ldx @tmp1
	ldx 2,x
	stx @tmp4
	jsr __div32x32_main
	stab @long+3
	staa @long+2
	ldab @tmp1+1
	ldaa @tmp1
	stab @long+1
	staa @long
	rts
;
;	@long = @long % (0-3,x)		signed
;
;	remainder has the same sign as the dividend.
;
__rem32x32sbx:
	clra
__rem32x32sdx:
	jsr __adx
__rem32x32sx:
	stx @tmp1
	ldx 0,x
	stx @tmp3
	ldx @tmp1
	ldx 2,x
	stx @tmp4
	ldab @tmp3
	bpl __rem32x32sx_10
	ldx #tmp3
	jsr __neg32x
__rem32x32sx_10:
	ldab @long
	stab sign		; save sign
	bpl __rem32x32sx_20
	jsr __neg32
__rem32x32sx_20:
	jsr __div32x32_main
	stab @long+3
	staa @long+2
	ldab @tmp1+1
	ldaa @tmp1
	stab @long+1
	staa @long
	ldab sign
	bpl __rem32x32sx_30
	jsr __neg32
__rem32x32sx_30:
	rts
;
;	@long = @long / TOS,  @long = @long % TOS
;
;	The divisor stays on the stack.  __pullret removes it.
;
__rem32x32s:
	tsx
	inx
	inx
	jsr __rem32x32sx
	bra __pullret
;
__rem32x32u:
	tsx
	inx
	inx
	jsr __rem32x32ux
	bra __pullret
;
__div32x32s:
	tsx
	inx
	inx
	jsr __div32x32sx
	bra __pullret
;
__div32x32u:
	tsx
	inx
	inx
	jsr __div32x32ux
	bra __pullret
;
;	__div32x32 leaves the divisor for the caller.
;
__div32x32:
	tsx
	inx
	inx
	jmp __div32x32ux
;

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
;	@long = @long / @tmp3:@tmp4
;	__div32x32_main is for internal use.
;	@tmp1:AccAB = @long % @tmp3:@tmp4
;	@tmp2:loop counter (bit) 8→0
;	@tmp2+1:loop counter (byte) 4→0
;       @tmp3:tmp4: divisor
;
__div32x32_main:
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
