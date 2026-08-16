;
;	char *strncat(char *d,const char *s, size_t n)
;	{
;	  if (n) {
;	    char *p = d;
;	    while (*p)
;	      p++;
;	    do {
;	    } while ((*p++ = *s++) && (--n))
;	    *p[-1] = 0;
;         }
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
;	@tmp2	p
;	@tmp3	s
;	@tmp4	n
;

	.code
	.export	_strncat
_strncat:
	pshb			; save d, the offset for stack +2
	psha
;
	tsx
	ldx	6,x
	beq	ret		; if n==0, do nothing
	stx	@tmp4		; @tmp4 = n
;
	ldaa	@tmp4+1		; loop counter setup
	beq	skip2
	inc	@tmp4
skip2:
;
	tsx
	ldx	0,x		; get d
	dex
skip:				; while (*p) p++;
	inx
	ldab	0,x
	bne	skip
	stx	@tmp2		; @tmp2 = p
;
	tsx
	ldx	4,x
	stx	@tmp3		; @tmp3 = s
;
loop:				; do {
	ldx	@tmp3
	ldab	0,x
	inx
	stx	@tmp3
;
	ldx	@tmp2
	stab	0,x
	beq	ret
	inx
	stx	@tmp2
;
	deca			; } while (--n);
	bne	loop
	dec	@tmp4
	bne	loop
;
end:
;	ldx	@tmp2
	clr	0,x
ret:
	pula			; recover d into AccAB
	pulb
	rts
