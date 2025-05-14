;
;	size_t *strlen(const char *s)
;	{
;	  size_t n=0;
;	  while (*s++) {
;	    n++;
;	  }
;	  return n;
;	}
;
;	AccAB: s
;
;	stack:
;	+0 ret addr
;

	.code
	.export	_strlen
_strlen:
	stab	@tmp1+1
	staa	@tmp1
	ldx	@tmp1
;
	dex
_strlen_loop:
	inx
	ldab	0,x
	bne	_strlen_loop
_strlen_ret:
;
	stx	@tmp2
	ldab	@tmp2+1
	ldaa	@tmp2
	subb	@tmp1+1
	sbca	@tmp1
	rts
;
