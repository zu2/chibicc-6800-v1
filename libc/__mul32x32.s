;
;	@long = @long * (0-3,x)
;
;	it's very slow
;
	.export __mul32x32x
	.export __mul32x32bx
	.export __mul32x32dx
	.data
	.code
;
;  __mul32x32x:  @long = @long * (0-3,x)
;  __mul32x32bx: @long = @long * (b,x)
;  __mul32x32dx: @long = @long * (d,x)
;
;  0-3,x:      Multiplier
;
;  long:       Multiplicand & result
;  tmp2:       tmp2:accAB  32bit acc.
;  tmp3:       Multiplier (copy from 0-1,x)
;  tmp4:       Multiplier (copy from 2-3,x)
;
;  IX is destroyed.  The loop counter lives in it.
;
__mul32x32bx:
        clra
__mul32x32dx:
        jsr __adx
__mul32x32x:
        stx @tmp2
        ldx 0,x
        stx @tmp3
        ldx @tmp2
        ldx 2,x
        stx @tmp4
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
        rts
