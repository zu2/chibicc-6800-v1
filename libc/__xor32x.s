;
;	long basic library xor32x
;
        .export __xor32i
	.export __xor32bx
	.export __xor32dx
	.export __xor32x
	.export __xor32x_store32x

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
	ldaa    2,x
	eora	@long+2
	staa	@long+2
	ldaa    1,x
	eora	@long+1
	staa	@long+1
	ldaa    0,x
	eora	@long
	staa	@long
        rts
;
__xor32x_store32x:
	bsr	__xor32x
	jmp	__store32x
