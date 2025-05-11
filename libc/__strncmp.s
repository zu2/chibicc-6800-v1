;
;	int strncmp(const char *s1,const char *s2, size_t n)
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
;	+4 n
;

	.code
	.export	_strncmp
_strncmp:
	tsx
	ldx	4,x		; check n
	beq	_strncmp_eq	; if n==0, return 0
	stab	@tmp2+1		; save s1
	staa	@tmp2
;
	tsx
	ldab	3,x		; s2
	ldaa	2,x
	addb	5,x		; n
	adca	4,x
	stab	@tmp4+1		; end addr = s2 + n
	staa	@tmp4
;
	ldx	2,x		; get s2
	stx	@tmp3
;
_strncmp_loop:
	ldx	@tmp2		; get s1
	ldab	0,x
	inx
	stx	@tmp2
	ldx	@tmp3
	cmpb	0,x
	bne	_strncmp_ne	; s1 - s2 !=0 ?
	inx
	stx	@tmp3
	cpx	@tmp4
	bne	_strncmp_loop
_strncmp_eq:
	clrb
	clra
	rts			; return 0
_strncmp_ne:
	bcs	_strncmp_lt
	ldab	#1		; return 1
	clra
	rts
_strncmp_lt:
	ldab	#$FF		; return -1
	tba
	rts
