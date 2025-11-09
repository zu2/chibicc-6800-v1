;
;	long basic library xor32x
;
        .export __xor32i
	.export __xor32bx
	.export __xor32dx
	.export __xor32x

	.code
;
;	__xor32i	@long ^= #imm
;
__xor32i:		; xor 32bit immediate
	tsx
	ldx 0,x
	ins
	ins
        bsr __xor32x
	jmp  4,x
;
__xor32bx:              ; xor 32bit from b,x
        clra
__xor32dx:		; xor 32bit from d,x
	jsr  __adx
__xor32x:
	ldab    3,x
	eorb	@long+3
	stab	@long+3
	ldab    2,x
	eorb	@long+2
	stab	@long+2
	ldab    1,x
	eorb	@long+1
	stab	@long+1
	ldab    0,x
	eorb	@long
	stab	@long
        rts
