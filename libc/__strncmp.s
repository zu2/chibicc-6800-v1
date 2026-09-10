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
	beq	eq		; if n==0, return 0
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
loop:
	ldx	@tmp2		; get s1
	ldab	0,x
	beq	nul		; *s1 == 0 ?
	inx
	stx	@tmp2
	ldx	@tmp3
	cmpb	0,x
	bne	ne		; *s1 - *s2 !=0 ?
	inx
	stx	@tmp3
	cpx	@tmp4
	bne	loop
eq:
	clrb
	clra
	rts			; return 0
;
nul:	ldx	@tmp3
	cmpb	0,x
	beq	eq
ne:
	bcs	lt
	ldab	#1		; return 1
	clra
	rts
lt:
	ldab	#$FF		; return -1
	tba
	rts
