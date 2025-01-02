;
;	long basic library
;
	.export __lloadx
	.export __lstorex
	.export __lpush
	.export __lpop
	.export __laddx
	.export __lsubx
	.export __laddtos
	.export __lsubtos
	.export	__32to8
	.export	__32to16
	.export	__u8to32
	.export	__u16to32
	.export	__s8to32
	.export	__s16to32
	.export	__lzero

	.code
;
;	__lloadx	load long from 0-3,x
;
__lloadx:
	ldab	3,x
	stab	@long+3
	ldab	2,x
	stab	@long+2
	ldab	1,x
	stab	@long+1
	ldab	0,x
	stab	@long+0
	rts
;
;	__lstorex	store long to 0-3,x
;
__lstorex:
	ldab	@long+3
	stab	3,x
	ldab	@long+2
	stab	2,x
	ldab	@long+1
	stab	1,x
	ldab	@long+0
	stab	0,x
	rts
;
;	__lpush		push @long
;
__lpush:
	pula
	pulb
	stab	@tmp1+1
	staa	@tmp1
	ldab	@long+3
	pshb
	ldab	@long+2
	pshb
	ldab	@long+1
	pshb
	ldab	@long
	pshb
	ldx	@tmp1
	jmp	0,x
;
;	__lpop		pul @long
;
__lpop:
	pula
	pulb
	stab	@tmp1+1
	staa	@tmp1
	pulb
	stab	@long
	pulb
	stab	@long+1
	pulb
	stab	@long+2
	pulb
	stab	@long+3
	ldx	@tmp1
	jmp	0,x
;
;	__laddx		@long += (0-3,x)
;
__laddx:
	ldab	3,x
	addb	@long+3
	stab	3,x
	ldab	2,x
	addb	@long+2
	stab	2,x
	ldab	1,x
	addb	@long+1
	stab	1,x
	ldab	0,x
	addb	@long
	stab	0,x
	rts
;
;	__lsubx		@long += (0-3,x)
;
__lsubx:
	ldab	3,x
	addb	@long+3
	stab	3,x
	ldab	2,x
	addb	@long+2
	stab	2,x
	ldab	1,x
	addb	@long+1
	stab	1,x
	ldab	0,x
	addb	@long
	stab	0,x
	rts
;
;	__laddtos	@long += TOS, pull TOS
;
__laddtos:
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
__lsubtos:
	pula
	pulb
	stab	@tmp1+1
	staa	@tmp1
	tsx
	ldab	3,x
	sbcb	@long+3
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
	clr	@long+1
	clr	@long
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
	stab	@long+3
	ldx	#0
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
__lzero:
	ldx	@long+2
	bne	__lzero_end
	ldx	@long
__lzero_end:
	rts
