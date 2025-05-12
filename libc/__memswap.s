;
;	void _memswap(void *p,const void *q,size_t n)
;	{
;	  char *p = d;
;	  while (n--){
;	    char tmp = *p;
;	    *p++ = *q;
;	    *q++ = tmp;
;	  }
;	}
;
;	AccAB: p
;	stack:
;	+0 ret addr
;	+2 q
;	+4 n
;

	.code
	.export	__memswap
__memswap:
	tsx
	ldx	4,x		; n==0 ?
	beq	__memswap_ret
	stab	@tmp2+1		; save p
	staa	@tmp2
	tsx
	ldab	5,x		; n
	ldaa	4,x
	addb	3,x
	adca	2,x
	stab	@tmp4+1		; swap end addr (q+n)
	staa	@tmp4
;
	ldx	2,x		; save q
;
__memswap_loop:
	stx	@tmp3
	ldab	0,x
	ldx	@tmp2
	ldaa	0,x
	stab	0,x
	inx
	stx	@tmp2
	ldx	@tmp3
	staa	0,x
	inx
	cpx	@tmp4
	bne	__memswap_loop
;
__memswap_ret:
	rts
;
