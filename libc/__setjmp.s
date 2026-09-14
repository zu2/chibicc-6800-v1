;
;	int  setjmp(jmp_buf env)
;	void longjmp(jmp_buf env, int val)
;
;	AccAB: env address
;
;	jmp_buf env
;		0:PC
;		2:SP
;		4:@bp
;
	.export _setjmp
	.export _longjmp
	.code
_setjmp:
	stab	@tmp2+1
	staa	@tmp2
	ldx	@tmp2		; env
;
	pula
	pulb
	stab	1,x		; PC
	staa	0,x
;
	sts	2,x		; SP
	pshb
	psha
;
	ldab	@bp+1		; @bp
	ldaa	@bp
	stab	5,x
	staa	4,x
;
	clrb			; return 0
	clra
;
	rts
;
;
_longjmp:
	stab	@tmp2+1		; env
	staa	@tmp2
;
	tsx
	ldaa	2,x
	ldab	3,x
	bne	val_ok
	tsta
	bne	val_ok
	incb
val_ok:
;
	ldx	@tmp2		; env
	ldx	4,x		; @bp
	stx	@bp
;
	ldx	@tmp2		; env
	lds	2,x		; recover SP
	ldx	0,x		; PC
	jmp	0,x
