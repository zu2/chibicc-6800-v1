;
;	long basic library and32
;
	.export __and32i
        .export __and32bx
        .export __and32bx
	.export __and32x

	.code
;
;	__and32i	@long &= #imm
;
__and32i:		; and 32bit immediate
	tsx
	ldx 0,x
	ins
	ins
        bsr __and32x
	jmp  4,x
;
;
;
__and32bx:              ; and 32bit from b,x
        clra
__and32dx:		; and 32bit from d,x
	jsr  __adx
__and32x:
	ldab    3,x
	andb	@long+3
	stab	@long+3
	ldab    2,x
	andb	@long+2
	stab	@long+2
	ldab    1,x
	andb	@long+1
	stab	@long+1
	ldab    0,x
	andb	@long
	stab	@long
        rts
