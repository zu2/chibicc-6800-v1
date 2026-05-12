;
;	char *strchr(const char *s,int c)
;	{
;	  while (*s && (*s!=c)) {
;		s++;
;	  }
;	  return (*s==c)? s: NULL;
;	}
;
;	AccAB: s
;
;	stack:
;	+0 ret addr
;	+2 c
;

	.code
	.export	_strchr
_strchr:
	stab	@tmp1+1		; save s
	staa	@tmp1
;
	tsx
	ldab	3,x		; get c
;
	ldx	@tmp1
	dex
;
loop:
	inx
	ldaa	0,x
	beq	str_end
	cba
	bne	loop
found:
	stx	@tmp1
	ldab	@tmp1+1
	ldaa	@tmp1
	rts
str_end:
	cba
	beq	found
	clrb
;	clra                    ; here, AccA==0
	rts

	
	
	
	


