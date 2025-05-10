;
;	int memcmp(const void *s1,const void *s2, size_t n)
;	{
;	  while (n>0 && *s1 == *s2) {
;	    s1++; s2++; n--;
;	  }
;	  return (unsigned char)*s1 - (unsigned char)*s2;
;	}
;
;	AccAB: s1
;
;	stack:
;	+0 ret addr
;	+2 s2
;	+4 n
;

	.code
	.export	_memcmp
_memcmp:
	tsx
	ldx	4,x		; n==0?
	beq	_memcmp_eq
;
	stab	@tmp2+1		; save s1
	staa	@tmp2
;
	tsx
	ldx	2,x		; get s2
	stx	@tmp3
;
	addb	@tmp3+1
	adca	@tmp3
	stab	@tmp4+1		; check end addres (s1+n)
	staa	@tmp4
;
_memcmp_loop:
	ldx	@tmp2
	ldab	0,x
	inx
	stx	@tmp2
	ldx	@tmp3
	cmpb	0,x
	bne	_memcmp_ne
	inx
	stx	@tmp3
	cpx	@tmp4
	bne	_memcmp_loop
_memcmp_eq:
	clrb
	clra
	rts			; return 0
_memcmp_ne:
	bcs	_memcmp_lt
	ldab	#1		; return 1
	clra
	rts
_memcmp_lt:
	ldab	#$FF		; return -1
	tba
	rts
