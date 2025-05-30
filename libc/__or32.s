;
;	long basic library or32
;
	.export __or32tos

	.code
;
__or32tos:
	tsx
	ldx	0,x
	ins
	ins
	pulb
	orab	@long
	stab	@long
	pulb
	orab	@long+1
	stab	@long+1
	pulb
	orab	@long+2
	stab	@long+2
	pulb
	orab	@long+3
	stab	@long+3
	jmp	0,x
