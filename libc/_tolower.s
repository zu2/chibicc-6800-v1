	.export	_tolower
	.code
;
_tolower:			; isupper(c)? c + ('a' - 'A'): c;
	tsta
	bne	ret
	cmpb	#'A'
	bcs	ret
	cmpb	#'Z'+1
	bcc	ret
	addb	#'a'-'A'
ret:	rts
