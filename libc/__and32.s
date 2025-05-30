;
;	long basic library and32
;
	.export __and32tos

	.code
;
;
;
__and32tos:
	tsx
	ldx	0,x
	ins
	ins
	pulb
	andb	@long
	stab	@long
	pulb
	andb	@long+1
	stab	@long+1
	pulb
	andb	@long+2
	stab	@long+2
	pulb
	andb	@long+3
	stab	@long+3
	jmp	0,x
