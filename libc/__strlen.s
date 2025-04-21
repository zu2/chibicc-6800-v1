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
	ldx	@tmp
	dex
;
	ldab	#$ff		; low  byte of n
	tba			; high byte of n
;
_strlen_loop:
	inx
	addb	#1
	adca	#0
	tst	0,x
	bne	_strlen_loop
_strlen_ret:
	rts
;
