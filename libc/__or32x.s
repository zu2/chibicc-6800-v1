;
;	long basic library or32
;
	.export __or32i
	.export __or32bx
	.export __or32dx
	.export __or32x

	.code
;
;	__or32i         @long |= #imm
;
__or32i:		; or 32bit immediate
	tsx
	ldx 0,x
	ins
	ins
        bsr __or32x
	jmp  4,x
;
__or32bx:               ; or 32bit from b,x
        clra
__or32dx:		; or 32bit from d,x
	jsr  __adx
__or32x:
        ldab    3,x
	orab	@long+3
	stab	@long+3
        ldab    2,x
	orab	@long+2
	stab	@long+2
        ldab    1,x
	orab	@long+1
	stab	@long+1
        ldab    0,x
	orab	@long
	stab	@long
        rts
