;
;	long basic library sub32
;
        .export __rsub32i
	.export __rsub32bx
	.export __rsub32dx
	.export __rsub32x

	.code
;
;	__rsub32i	@long = #imm - @long
;
__rsub32i:		; sub 32bit immediate
	tsx
	ldx 0,x
	ins
	ins
        bsr __rsub32x
        jmp 4,x
;
;
;	__rsub32x	@long = (0-3,x) - @long
;
__rsub32bx:
	clra
__rsub32dx:
	jsr __adx
__rsub32x:
	ldab 3,x
	subb @long+3
	stab @long+3
	ldab 2,x
	sbcb @long+2
	stab @long+2
	ldab 1,x
	sbcb @long+1
	stab @long+1
	ldab 0,x
	sbcb @long
	stab @long
	rts
