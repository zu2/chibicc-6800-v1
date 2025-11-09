;
;	long basic library sub32
;
        .export __sub32i
	.export __sub32x
	.export __sub32tos

	.code
;
;	__sub32i	@long -= #imm
;
__sub32i:		; sub 32bit immediate
	tsx
	ldx 0,x
	ins
	ins
        bsr __sub32x
        jmp 4,x
;
;
;	__sub32x	@long -= (0-3,x)
;
__sub32x:
	ldab @long+3
	subb 3,x
	stab @long+3
	ldab @long+2
	sbcb 2,x
	stab @long+2
	ldab @long+1
	sbcb 1,x
	stab @long+1
	ldab @long
	sbcb 0,x
	stab @long
	rts
;
;	__sub32tos	@long = TOS - @long, pull TOS
;
__sub32tos:
	pula		; pull return address
	pulb
	stab	@tmp1+1
	staa	@tmp1
	tsx
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
	ins
	ins
	ins
	ins
	ldx	@tmp1
	jmp	0,x
