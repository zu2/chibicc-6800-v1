;
;	void *memset(void *s,int c,size_t n)
;	{
;	  char *p = s;
;	  while (n--){
;	    *p++ = c;
;	  }
;	  return s;
;	}
;
;	stack:
;	+0 ret addr
;	+2 c
;	+4 n
;
	.code
	.export	_memset
_memset:
	tsx
	ldx	4,x		; n==0 ?
	beq	_memset_ret
	pshb			; save s, access to the stack will add two.
	psha
	tsx
	ldab	7,x		; n
	ldaa	6,x
	cmpb	#1		; if b!=0 then a++
	sbca	#0
	inca
	staa	@tmp1
;
	ldaa	4,x		; c
;
	ldx	0,x		; saved s
;
_memset_loop:
	staa	0,x
	inx
	decb
	bne	_memset_loop
	dec	@tmp1
	bne	_memset_loop
;
	pula
	pulb
;
_memset_ret:
	rts
;

	
	
	
	


