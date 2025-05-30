;
;	long basic library xor32
;
	.export __xor32tos

	.code
;
__xor32tos:
	tsx
	ldx	0,x
	ins
	ins
	pulb
	eorb	@long
	stab	@long
	pulb
	eorb	@long+1
	stab	@long+1
	pulb
	eorb	@long+2
	stab	@long+2
	pulb
	eorb	@long+3
	stab	@long+3
	jmp	0,x
