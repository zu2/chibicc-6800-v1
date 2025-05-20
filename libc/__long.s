;
;	long basic library
;
	.export __load32
	.export __load32i
	.export __load32x
	.export __load32bx
	.export __load32dx
	.export __store32x
	.export __store32dx
	.export __load32x_push
	.export __push32
	.export __push32x
	.export __push32dx
	.export __pop32
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
__load32:		; load 32bit from (AccAB)
	pshb
	psha
	tsx
	ldx 0,x
	ins
	ins
	ldab 3,x
	stab @long+3
	ldab 2,x
	stab @long+2
	ldab 1,x
	stab @long+1
	ldab 0,x
	stab @long+0
	rts
__load32i:		; load 32bit immediate
	tsx
	ldx 0,x
	ins
	ins
	ldab 3,x
	stab @long+3
	ldab 2,x
	stab @long+2
	ldab 1,x
	stab @long+1
	ldab 0,x
	stab @long+0
	jmp  4,x
__load32bx:             ; loda 32bit from b,x
        clra
__load32dx:		; load 32bit from d,x
	jsr  __adx
__load32x:		; load 32bit from (0-3,x)
	ldab 3,x
	stab @long+3
	ldab 2,x
	stab @long+2
	ldab 1,x
	stab @long+1
	ldab 0,x
	stab @long+0
	rts
;
;	__store32x	store long to 0-3,x
;	__store32bx	store long to b,x  (mess IX)
;	__store32dx	store long to d,x  (mess IX)
;
__store32bx:
        clra
__store32dx:
	jsr __adx
__store32x:
	ldab @long+3
	stab 3,x
	ldab @long+2
	stab 2,x
	ldab @long+1
	stab 1,x
	ldab @long+0
	stab 0,x
	rts
;
;	__load32x & __push32
;
__load32x_push:
	jsr __load32x
        jmp __push32
;
;	__push32	push @long
;
__push32:
	tsx
	ldx 0,x
	ins
	ins
	ldab @long+3
	pshb
	ldab @long+2
	pshb
	ldab @long+1
	pshb
	ldab @long
	pshb
	jmp 0,x
;
;	__push32x	push 0-3,x
;	__push32x	push d,x
;
__push32dx:
	jsr	__adx
__push32x:
	pulb
	stab	@tmp1
	pulb
	stab	@tmp1+1
	ldab	3,x
	pshb
	ldab	2,x
	pshb
	ldab	1,x
	pshb
	ldab	0,x
	pshb
	ldx	@tmp1
	jmp	0,x
;
;	__pop32		pul @long
;
__pop32:
	tsx
	ldx 0,x
	ins
	ins
	pulb
	stab @long
	pulb
	stab @long+1
	pulb
	stab @long+2
	pulb
	stab @long+3
	jmp 0,x
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

 
