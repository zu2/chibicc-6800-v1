;
;	char *strchr(const char *s,int c)
;	{
;	  while (*s && (*s!=c)) {
;		s++;
;	  }
;	  return (*s==c)? s: NULL;
;	}
;
;	AccAB: s
;
;	stack:
;	+0 ret addr
;	+2 c
;

	.code
	.export	_strchr
_strchr:
	stab	@tmp1+1		; save s
	staa	@tmp1
;
	tsx
	ldab	3,x		; get c
;
	ldx	@tmp1
	dex
;
_strchr_loop:
	inx
	ldaa	0,x
	cba
	beq	_strchr_found
	tsta
	bne	_strchr_end
_strchr_end:
	clrb
;	clra			; here, AccA == 0
	rts
_strchr_found:
	stx	@tmp1
	ldab	@tmp1+1
	ldaa	@tmp1
	rts
;

	
	
	
	


