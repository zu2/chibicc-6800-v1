;
;	void *strcpy(char *d,const char *s)
;	{
;	  char *p = d;
;	  while (*p++ = *s++) {
;	  }
;	  return d;
;	}
;
;	AccAB: d
;
;	stack:
;	+0 ret addr
;	+2 s
;

	.code
	.export	_strcpy
_strcpy:
	pshb			; save d, access to the stack will add two.
	psha
;
	tsx
	ldx	0,x		; get d
	stx	@tmp2
;
	tsx
	ldx	4,x		; get s
	bra	_strcpy_start
;
_strcpy_loop:
	inx
	stx	@tmp2
	ldx	@tmp3
_strcpy_start:
	ldaa	0,x
	inx
	stx	@tmp3
	ldx	@tmp2
	staa	0,x
	bne	_strcpy_loop
;
	pula			; recover d into AccAB
	pulb
_strcpy_ret:
	rts
;

	
	
	
	


