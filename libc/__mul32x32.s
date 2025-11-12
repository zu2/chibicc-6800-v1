;
;	@long = TOS*@long
;
;	it's very slow
;
	.export __mul32tos
	.code
;
;  __mul32tos: @long = @long * TOS
;
;  0-1,x:      return address
;  2-5,x:      Multiplicand (TOS) & result
;  tmp2:AccAB  working space
;  long:       Multiplier
;  tmp1:       loop counter
;  tmp4:       loop end (IX)
;
__mul32tos:
        ldx #0
        stx @tmp3
        stx @tmp2
;
        tsx
        inx
        stx @tmp4
        inx
        inx
        inx
        inx
;
        clrb
        clra
;
loop8:
        pshb
        ldab #$08
        stab @tmp1
        pulb
        lsr 0,x
loop:
        bcc  skip
;
        addb @long+3
        adca @long+2
        psha
        ldaa @tmp2+1
        adca @long+1
        staa @tmp2+1
        ldaa @tmp2
        adca @long
        staa @tmp2
        pula
;
skip:   lsr @tmp2
        ror @tmp2+1
        rora
        rorb
        ror 0,x
;
next:   dec @tmp1
        bne loop
next8:
        dex
        cpx @tmp4
        bne loop8
ret:
        tsx
        ldx 2,x
        stx @long
        tsx
        ldx 4,x
        stx @long+2
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
