	.export	_isascii
	.code
;
; isascii(c)
;   return (0 <= c && c <= 0x7f);
;
_isascii:
	tsta
	bne	ret0
	andb	#$80
	eorb	#$80	; non-zero is true
	rts
ret0:	clrb
	clra
	rts
