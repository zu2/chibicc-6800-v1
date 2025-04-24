;
;	void *strcat(char *d,const char *s)
;	{
;	  char *p = d;
;	  while (*p++)
;		;
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
	.export	_strcat
_strcat:
	pshb			; save d, access to the stack will add two.
	psha
;
	tsx
	ldx	0,x		; get d
;
	dex
_strcat_skip:
	inx
	ldab	0,x
	bne	_strcat_skip
	stx	@tmp2
;
	tsx
	ldx	4,x		; get s
	bra	_strcat_start
;
_strcat_loop:
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
_strcat_start:
	ldaa	0,x
	beq	_strcat_ret1
	ldab	1,x
	bne	_strcat_loop
	ldx	@tmp2
	stab	1,x
_strcat_ret1:
	ldx	@tmp2
_strcat_ret2:
	staa	0,x
_strcat_ret:
	pula			; recover d into AccAB
	pulb
	rts
