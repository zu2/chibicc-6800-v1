	.export	_toupper
	.code
;
_toupper:			; islower(c)? c - ('a' - 'A'): c;
	tsta
	bne	ret
	cmpb	#'a'
	bcs	ret
	cmpb	#'z'+1
	bcs	ret
	subb	#'a'-'A'
ret:	rts

