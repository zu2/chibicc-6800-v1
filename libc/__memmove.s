;
;	void *memmove(void *d,const void *s,size_t n)
;	{
;	  const char *p = s;
;	  char *q = d;
;
;	  if (d<s) {
;	    while (n--) {
;	      *q++ = *p++;
;	    }
;	  }else{
;	    p+ = n;
;	    q+ = n;
;	    while (n--) {
;	      *--q = *--p;
;	    }
;	  }
;	  return d;
;	}
;
;	AccAB: d
;
;	stack:
;	+0 ret addr
;	+2 s
;	+4 n
;

	.code
	.export	_memmove
_memmove:
	tsx
	ldx	4,x		; n==0 ?
	beq	ret
	pshb			; save d, access to the stack will add two.
	psha
;
	tsx
	ldx	0,x		; saved d
	stx	@tmp2
	tsx
	ldx	4,x		; s
	cpx	@tmp2
	beq	ret_d
	stx	@tmp3
;
	tsx
	subb	@tmp3+1		; d - s
	sbca	@tmp3
	bcs	forward	; if d<s
;
backward:
	ldab	@tmp3+1		; s += n
	ldaa	@tmp3
	addb	7,x
	adca	6,x
	stab	@tmp3+1
	staa	@tmp3
;
	ldab	@tmp2+1		; d += n
	ldaa	@tmp2
	addb	7,x
	adca	6,x
	stab	@tmp2+1
	staa	@tmp2
;
	bsr	adjust_n
	ldx	@tmp3
;
backward_loop:
	dex
	ldaa	0,x
	stx	@tmp3
	ldx	@tmp2
	dex
	staa	0,x
	stx	@tmp2
	ldx	@tmp3
	decb
	bne	backward_loop
	dec	@tmp1
	bne	backward_loop
ret_d:
	pula
	pulb
ret:
	rts
;
forward:
	bsr	adjust_n
	ldx	@tmp3
forward_loop:
	ldaa	0,x
	inx
	stx	@tmp3
	ldx	@tmp2
	staa	0,x
	inx
	stx	@tmp2
	ldx	@tmp3
	decb
	bne	forward_loop
	dec	@tmp1
	bne	forward_loop
;
	pula
	pulb
	rts
;
adjust_n:
	ldaa	6,x
	ldab	7,x
	beq	adjust_ret
	inca
adjust_ret:
	staa	@tmp1
	rts
