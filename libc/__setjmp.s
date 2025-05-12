;
;	void setjmp(jmp_buf env)
;	void longjmp(jmp_buf env, int val)
;
;	AccAB: env address
;
;	jmp_buf env
;		0:CCR
;		1:IX
;		3:PC
;		5:SP
;		7:@bp
;
	.export _setjmp
	.export _longjmp
	.code
_setjmp:
	psha
	tpa
	staa	@tmp1		; CCR	(needed??)
	pula
;
	stab	@tmp2+1
	staa	@tmp2
	stx	@tmp3
	ldx	@tmp2		; env
;
	ldaa	@tmp1		; CCR
	staa	0,x
;
	ldab	@tmp3+1		; IX
	ldaa	@tmp3
	stab	2,x
	staa	1,x
;
	sts	5,x		; SP
;
	pula
	pulb
	stab	4,x		; PC
	staa	3,x
	pshb
	psha
;
	ldab	@bp+1		; @bp
	ldaa	@bp
	stab	8,x
	staa	7,x
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
	ldx	2,x
	bne	__longjmp_val_ok
	inx
__longjmp_val_ok:
	stx	@tmp3		; val
;
	ldx	@tmp2		; env
;
	ldab	8,x		; @bp
	ldaa	7,x
	stab	@bp+1
	staa	@bp
;
	lds	5,x		; recover SP
;
	ldab	4,x		; PC
	ldaa	3,x
	pshb
	psha
;
	ldab	2,x		; IX
	ldaa	1,x
	pshb
	psha
;
	ldab	@tmp3+1		; AccA,B
	ldaa	@tmp3
	psha
	pshb
;
	ldab	0,x		; CCR
	pshb
;
	rti
