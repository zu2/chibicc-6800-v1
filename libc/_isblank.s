	.export	_isblank
	.code
;
_isblank:			; (c == ' ' || c == '\t')
	tsta
	bne	ret0
	cmpb	#$20
	beq	ret
	cmpb	#$09		; TAB
	beq	ret
ret0:	clrb
	clra
ret:	rts			; Any non‑zero return is true.
