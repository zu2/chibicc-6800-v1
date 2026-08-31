;
;	float __polyf(float x, const float *c, int n)
;
;	Horner's method
;
;	r = c[0];
;	while (n--) {
;		r = r * x + *++c;
;	}
;	return r;
;
;	entry:	@long = x, c[0..n], n>=1
;	exit:	@long = r;
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
	stab	__polyn
	ldx	2,x
	stx	__polyc
;
	ldx	@long
	stx	__polyx
	ldx	@long+2
	stx	__polyx+2
;
;	1st stage: c[0] * x, using @long
;
	ldx	__polyc
	jsr	__mulf32x
	bra	__polyadd	; c[0] * @long
;
__polyloop:
	ldx	#__polyx
	jsr	__mulf32x
__polyadd:
	ldx	__polyc
	inx
	inx
	inx
	inx
	stx	__polyc
	jsr	__addf32x
	dec	__polyn
	bne	__polyloop
	rts
