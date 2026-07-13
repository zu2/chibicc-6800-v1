;
;	@long = TOS*@long
;
;	it's very slow
;
	.export __mul32tos
	.export __mul32x
	.code
;
;  __mul32tos: @long = @long * TOS
;
;  0-1,x:      return address
;  2-5,x:      Multiplicand (TOS)
;
;  long:       Multiplicand & result
;  tmp2:       tmp2:accAB  32bit acc.
;  tmp3:       Multiplier (copy from TOS)
;  tmp4:       Multiplier (copy from TOS)
;
;
	.data
	.code
__mul32x:
        stx @tmp2
        jsr __push32
        ldx @tmp2
        jsr __load32x
        bsr __mul32tos
        rts
__mul32tos:
	tsx
	ldx 4,x
	stx @tmp4
	tsx
	ldx 2,x
	stx @tmp3
;
        ldx #0
        stx tmp2
;
        clrb
        clra
;
	ldx #8
;
        lsr  @long+3
loop_1:
        bcc  skip_1
        addb @tmp4+1
        adca @tmp4
        psha
        ldaa @tmp3+1
        adca @tmp2+1
        staa @tmp2+1
        ldaa @tmp3
        adca @tmp2
        staa @tmp2
        pula
skip_1: ror tmp2
        ror tmp2+1
        rora
        rorb
        ror @long+3
	dex
	bne loop_1
;
	ldx #8
	lsr @long+2
loop_2:
        bcc  skip_2
        addb @tmp4+1
        adca @tmp4
        psha
        ldaa @tmp3+1
        adca @tmp2+1
        staa @tmp2+1
        pula
skip_2: ror tmp2+1
        rora
        rorb
        ror @long+2
	dex
	bne loop_2
;
	ldx #8
	lsr @long+1
loop_3:
        bcc  skip_3
        addb @tmp4+1
        adca @tmp4
skip_3: rora
        rorb
        ror @long+1
	dex
	bne loop_3
;
	ldx #8
	ldaa @long
	lsra
loop_4:
        bcc  skip_4
        addb @tmp4+1
skip_4: rorb
        rora
	dex
	bne loop_4
;
	staa @long
;
        tsx
	ldx 0,x
	ins
	ins
	ins
	ins
	ins
	ins
	jmp	0,x
