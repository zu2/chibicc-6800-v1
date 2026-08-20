;
;	float __polyf(float x, const float *c, int n)
;
;	Horner: r = c[0]; while (--n) r = r * x + *++c;
;
;	entry:	@long = x, stack: ret(2), c(2), n(2)
;	exit:	@long = r
;
;	The first stage is c[0] * x, so @long already holds a valid operand.
;
	.setcpu 6800
	.export ___polyf

	.data
__polyx:
	.word	0
	.word	0
__polyc:
	.word	0
__polyn:
	.byte	0

	.code
___polyf:
	tsx
	ldab	5,x
	decb
	stab	__polyn
	ldx	2,x
	stx	__polyc
;
	ldx	@long
	stx	__polyx
	ldx	@long+2
	stx	__polyx+2
;
	ldx	__polyc
	jsr	__mulf32x
	bsr	__polyadv
	bra	__polyadd
;
__polyloop:
	ldx	#__polyx
	jsr	__mulf32x
__polyadd:
	ldx	__polyc
	jsr	__addf32x
	bsr	__polyadv
	dec	__polyn
	bne	__polyloop
	rts
;
__polyadv:
	ldx	__polyc
	inx
	inx
	inx
	inx
	stx	__polyc
	rts
