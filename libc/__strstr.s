;
;	char *strstr(const char *h,const char *k)
;	{
;	  if (*k == 0)
;		return s;
;	  while (*s) {
;		const char *p = s;
;		const char *q = k;
;		for (;;) {
;			if (*q == 0)
;				return s;
;			if (*p++ != *q++)
;				break;
;		}
;		s++;
;	  }
;	  return NULL;
;	}
;
;	AccAB: s
;
;	stack:
;	+0 ret addr
;	+2 k
;

	.code
	.export	_strstr
_strstr:
	stab	@tmp1+1		; save s
	staa	@tmp1
;
	tsx
	ldx	2,x		; get k
	stx	@tmp2
	ldab	0,x
	beq	found
;
loop:
	ldx	@tmp1
	ldab	0,x
	beq	ret_null
	stx	@tmp3		; @tmp3:p = s
	ldx	@tmp2
	stx	@tmp4		; @tmp4:q = k
	bra	loop2
;
inc2:
	inx
	stx	@tmp3
	ldx	@tmp4
loop2:
	ldab	0,x
	beq	found
	inx
	stx	@tmp4
;
	ldx	@tmp3
	cmpb	0,x
	beq	inc2
;
tail:
	ldx	@tmp1		; s++
	inx
	stx	@tmp1
	bra	loop
;
found:
	ldab	@tmp1+1
	ldaa	@tmp1
	rts
ret_null:
	clrb
	clra
	rts
