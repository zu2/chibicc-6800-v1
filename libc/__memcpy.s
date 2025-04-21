;
;	void *memcpy(void *d,const void *s,size_t n)
;	{
;	  char *p = d;
;	  while (n--){
;	    *p++ = *s++;
;	  }
;	  return d;
;	}
;
;	stack:
;	+0 ret addr
;	+2 s
;	+4 n
;

	.code
	.export	_memcpy
_memcpy:
	tsx
	ldx	4,x		; n==0 ?
	beq	_memcpy_ret
	pshb			; save d, access to the stack will add two.
	psha
	tsx
	ldab	7,x		; n
	ldaa	6,x
	cmpb	#1		; if b!=0 then a++
	sbca	#0
	inca
	staa	@tmp1
;
	ldx	0,x		; saved d
	stx	@tmp2
	tsx
	ldx	4,x		; s
	stx	@tmp3
;
_memcpy_loop:
	ldaa	0,x
	inx
	stx	@tmp3
	ldx	@tmp2
	staa	0,x
	inx
	stx	@tmp2
	ldx	@tmp3
	decb
	bne	_memcpy_loop
	dec	@tmp1
	bne	_memcpy_loop
;
	pula
	pulb
;
_memcpy_ret:
	rts
;

	
	
	
	


