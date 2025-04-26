;
;	void *memmove(void *d,const void *s,size_t n)
;	{
;         const char *p = s;
;         char *q = d;
;
;         if (d<s) {
;	    while (n--) {
;             *q++ = *p++;
;           }
;         }else{
;           p+ = n;
;           q+ = n;
;	    while (n--) {
;             *--q = *--p;
;           }
;         }
;         return d;
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
	.export	_memmove
_memmove:
	tsx
	ldx	4,x		; n==0 ?
	beq	_memmove_ret
	pshb			; save d, access to the stack will add two.
	psha
;
	tsx
	ldx	0,x		; saved d
	stx	@tmp2
	tsx
	ldx	4,x		; s
	stx	@tmp3
;
	tsx
	subb	@tmp3+1		; d - s
	sbca	@tmp3
	bcs	_memmove_inc	; if d<s
	bhi	_memmove_dec	; if d>s
	beq	_memmove_ret_d	; if d==s, return d
;
_memmove_dec:
	ldab	@tmp3+1		; s += n
	ldaa	@tmp3
	addb	7,x
	adca	6,x
	stab	@tmp3+1
	staa	@tmp3
;
	ldab	@tmp2+1		; d += n
	ldaa	@tmp2
	addb	7,x
	adca	6,x
	stab	@tmp2+1
	staa	@tmp2
;
	bsr	_adjust_n
;
_memmove_dec_loop:
	dex
	ldaa	0,x
	stx	@tmp3
	ldx	@tmp2
	dex
	staa	0,x
	stx	@tmp2
	ldx	@tmp3
	decb
	bne	_memmove_dec_loop
	dec	@tmp1
	bne	_memmove_dec_loop
_memmove_ret_d:
	pula
	pulb
_memmove_ret:
	rts
;
_memmove_inc:
	bsr	_adjust_n
_memmove_inc_loop:
	ldaa	0,x
	inx
	stx	@tmp3
	ldx	@tmp2
	staa	0,x
	inx
	stx	@tmp2
	ldx	@tmp3
	decb
	bne	_memmove_inc_loop
	dec	@tmp1
	bne	_memmove_inc_loop
;
	pula
	pulb
	rts
;
_adjust_n:
	ldaa	6,x
	ldab	7,x
	bne	_adjust_n_ret
	inca
_adjust_n_ret:
	staa	@tmp1
	rts
