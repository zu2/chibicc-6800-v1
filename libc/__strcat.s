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
	jmp	__strcpy_start
