;
;	int strcmp(const char *s1,const char *s2)
;	{
;	  while (*s1 == *s2) {
;	    if (*s1)
;	      return 0;
;	    s1++; s2++;
;	  }
;	  return (unsigned char)*s1 - (unsigned char)*s2;
;	}
;
;	AccAB: s1
;
;	stack:
;	+0 ret addr
;	+2 s2
;

	.code
	.export	_strcmp
_strcmp:
	stab	@tmp2+1		; save s1
	staa	@tmp2
;
	tsx
	ldx	2,x		; get s2
	stx	@tmp3
;
_strcmp_loop:
	ldx	@tmp2
	ldab	0,x
	ldaa	1,x
	inx
	inx
	stx	@tmp2
	ldx	@tmp3
	cmpb	0,x
	bne	_strcmp_ne
	tstb
	beq	_strcmp_eq
	cmpa	1,x
	bne	_strcmp_ne
	inx
	inx
	stx	@tmp3
	tsta
	bne	_strcmp_loop
_strcmp_eq:
	clrb
	clra
	rts			; return 0
_strcmp_ne:			; if C==1 return -1 else return 1
	ldab	#0
	sbcb	#0		; if C==1 then AccB=$FF else AccB=$00
	tba
	orab	#1		; set AccB to $FF or $01
	rts
