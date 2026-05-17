	.export _isxdigit
	.code
;
;	isxdigit: (c >= 'A' && c <= 'F')
;	       || (c >= 'a' && c <= 'f')
;	       || isdigit(c)
;
_isxdigit:
	tsta
	bne	ret00
	cmpb	#'0'
	bcs	ret0
	cmpb	#'9'+1
	bcs	ret
	orab	#$20
	cmpb	#'a'
	bcs	ret0
	cmpb	#'f'+1
	tab
	rolb
	rts
ret00:
	clra
ret0:
	clrb
ret:
	rts
