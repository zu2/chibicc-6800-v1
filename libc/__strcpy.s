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
	.export	__strcpy_start
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
	bra	__strcpy_start
;
_strcpy_loop:
	inx
	inx
	stx	@tmp3
	ldx	@tmp2
	staa	0,x
	stab	1,x
	inx
	inx
	stx	@tmp2
	ldx	@tmp3
__strcpy_start:
	ldaa	0,x
	beq	_strcpy_ret1
	ldab	1,x
	bne	_strcpy_loop
	ldx	@tmp2
	stab	1,x
_strcpy_ret1:
	ldx	@tmp2
_strcpy_ret2:
	staa	0,x
_strcpy_ret:
	pula			; recover d into AccAB
	pulb
	rts
