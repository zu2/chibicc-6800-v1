;
;	void *memchr(const void *s,int c,size_t n)
;	{
;		const unsigned char *p = s;
;
;		while (n--) {
;			if (*s == (unsigned char)c)
;				return (void *)p;
;			p++;
;		;
;		RETURN NULL;
;	}
;
;	AccAB: s
;
;	stack:
;	+0 ret addr
;	+2 c
;	+4 n
;

	.code
	.export	_memchr
_memchr:
	stab	@tmp1+1		; save s
	staa	@tmp1
;
	tsx
	ldaa	3,x		; get c
;
	ldx	4,x		; n==0?
	beq	_memchr_null
;
	stx	@tmp2		; @tmp2 = n
;
	ldab	@tmp2+1		; if lo(n)!=0, hi(n)++
	beq	_memchr_1
	inc	@tmp2
;
_memchr_1:
	ldx	@tmp1
;
_memchr_loop:
	cmpa	0,x
	beq	_memchr_found
	inx
	decb
	bne	_memchr_loop
	dec	@tmp2
	bne	_memchr_loop
_memchr_null:
	clrb
	clra
	rts
_memchr_found:
	stx	@tmp1
	ldab	@tmp1+1
	ldaa	@tmp1
	rts
