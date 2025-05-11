;
;	void *strncpy(char *d,const char *s, size_t n)
;	{
;	  char *p = d;
;	  while (n-->0 && *p++ = *s++) {
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
	.export	_strncpy
_strncpy:
	pshb			; save d, access to the stack will add two.
	psha
;
	tsx
	ldx	6,x		; if n==0 then return d
	beq	_strncpy_ret
;
	tsx
	ldab	1,x		; d
	ldaa	0,x
	addb	7,x
	adca	6,x
	stab	@tmp4+1		; @tmp4 = d + n
	staa	@tmp4
;
	ldx	4,x		; get s
	stx	@tmp3
;
	tsx
	ldx	0,x		; get d
;
_strncpy_loop:
	stx	@tmp2
	ldx	@tmp3
	ldab	0,x
	inx
	stx	@tmp3
	ldx	@tmp2
	stab	0,x
	beq	_strncpy_ret
	cpx	@tmp4
	bne	_strncpy_loop
_strncpy_ret:
	pula			; recover d into AccAB
	pulb
	rts
