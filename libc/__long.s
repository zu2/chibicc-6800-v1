;
;	long basic library
;
	.export __neg32
	.export __neg32x
	.export __add32x
	.export __sub32x
	.export __add32tos
	.export __sub32tos
	.export __and32tos
	.export __or32tos
	.export __xor32tos
	.export __not32
	.export	__32to8
	.export	__32to16
	.export	__u8to32
	.export	__u16to32
	.export	__s8to32
	.export	__s16to32
	.export	__iszero32

	.code
;
;	__neg32		negate 32bit long, break IX
;
__neg32:
	com @long
	com @long+1
	com @long+2
	com @long+3
	ldx @long+2
	inx
	stx @long+2
	bne __neg32_ret
	ldx @long
	inx
	stx @long
__neg32_ret:
	rts
;
;	__neg32x	negate 32bit long (0-3,x)
;
__neg32x:
	com 0,x
	com 1,x
	com 2,x
	neg 3,x
	bne __neg32x_ret
	inc 2,x
	bne __neg32x_ret
	inc 1,x
	bne __neg32x_ret
	inc 0,x
__neg32x_ret:
	rts
;
;
;	__add32x	@long += (0-3,x)
;
__add32x:
	ldab 3,x
	addb @long+3
	stab @long+3
	ldab 2,x
	addb @long+2
	stab @long+2
	ldab 1,x
	addb @long+1
	stab @long+1
	ldab 0,x
	addb @long
	stab @long
	rts
;
;	__sub32x	@long -= (0-3,x)
;
__sub32x:
	ldab @long+3
	subb 3,x
	stab @long+3
	sbcb @long+2
	sbca 2,x
	stab @long+2
	ldab @long+1
	sbcb 1,x
	stab @long+1
	ldab @long
	sbca 0,x
	stab @long
	rts
;
;	__laddtos	@long += TOS, pull TOS
;
__add32tos:
	pula
	pulb
	stab	@tmp1+1
	staa	@tmp1
	tsx
	ldab	3,x
	addb	@long+3
	stab	@long+3
	ldab	2,x
	adcb	@long+2
	stab	@long+2
	ldab	1,x
	adcb	@long+1
	stab	@long+1
	ldab	0,x
	adcb	@long
	stab	@long
	ins
	ins
	ins
	ins
	ldx	@tmp1
	jmp	0,x
;
;	__lsubtos	@long = TOS - @long, pull TOS
;
__sub32tos:
	pula		; pull return address
	pulb
	stab	@tmp1+1
	staa	@tmp1
	tsx
	ldab	3,x
	subb	@long+3
	stab	@long+3
	ldab	2,x
	sbcb	@long+2
	stab	@long+2
	ldab	1,x
	sbcb	@long+1
	stab	@long+1
	ldab	0,x
	sbcb	@long
	stab	@long
	ins
	ins
	ins
	ins
	ldx	@tmp1
	jmp	0,x
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
;
__not32:
	com	@long+3
	com	@long+2
	com	@long+1
	com	@long
	rts
;
;	convert value
;
__32to8:
	ldab	@long+3
	rts
__32to16:
	ldab	@long+3
	ldaa	@long+2
	rts
__u8to32:
	stab	@long+3
	clra
	staa	@long+2
	staa	@long+1
	staa	@long
	rts
__u16to32:
	stab	@long+3
	staa	@long+2
	clra
	staa	@long+1
	staa	@long
	rts
__s8to32:
	clra
	stab	@long+3
	bpl	__s8to32_1
	deca
__s8to32_1:
	staa	@long+2
	staa	@long+1
	staa	@long
	rts
__s16to32:
	ldx	#0
	stab	@long+3
	staa	@long+2
	bpl	__s16to32_1
	dex
__s16to32_1:
	stx	@long
	rts
;
;	if (@long == zero)
;		set Z
;
__iszero32:
	ldx	@long+2
	bne	__iszero32_end
	ldx	@long
__iszero32_end:
	rts

 
