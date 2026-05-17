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
	bra	start
;
loop:
	inx			; 4 1
	inx			; 4 1
start:
	stx	@tmp3		; 5 2
	ldx	@tmp2		; 4 2
	ldab	0,x		; 5 2
	beq	s1_end		; 4 2
	ldaa	1,x		; 5 2
	beq	s2_end		; 4 2	; ↑ 27cyc
	inx			; 4 1
	inx			; 4 1
	stx	@tmp2		; 5 2
	ldx	@tmp3		; 4 2
	subb	0,x		; 5 2
	bne	ne_b		; 4 2	; ↑ 53cyc
	suba	1,x		; 5 2
	beq	loop		; 4 2
	tab			; 2 1
ne_b:
	rora			; 2 1
	rts			; 5 1
s1_end:
	ldx	@tmp3		; 4 2
	clra			; 2 1
	subb	0,x		; 5 2
	rora			; 2 1
	rts			; 5 1
s2_end:
	ldx	@tmp3		; 4 2
	subb	0,x		; 5 2
	bne	ne_b		; 4 1
	suba	1,x		; 5 2
	rora			; 2 1
	rts			; 5 1
