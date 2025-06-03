;
;	char *strrchr(const char *s,int c)
;	{
;	  char *p = NULL;
;
;	  while (*s) {
;	    if (*s==c)
;             p = s;
;	      s++;
;	  }
;	  return p;
;	}
;
;	AccAB: s
;
;	stack:
;	+0 ret addr
;	+2 c
;

	.code
	.export	_strrchr
_strrchr:
	stab	@tmp1+1		; save s
	staa	@tmp1
;
	clr	@tmp2+1		; p = NULL
	clr	@tmp2
;
	tsx
	ldab	3,x		; get c
;
	ldx	@tmp1
	dex
;
_strrchr_loop:
	inx
	cmpb	0,x
	bne	_strrchr_ne
	stx	@tmp2
_strrchr_ne:
	ldaa	0,x
	bne	_strrchr_loop
;
	ldab	@tmp2+1
	ldaa	@tmp2
	rts
;

	
	
	
	


