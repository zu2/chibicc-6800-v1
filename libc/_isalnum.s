	.export _isalnum
	.code
;
; int isalnum(int c)
; {
;   return ('0' <= c && c <='9')
;       || ('A' <= c && c <='Z')
;       || ('a' <= c && c <='z');
;

_isalnum:			; isalpha(c) || isdigit(c)
	tsta
	bne	ret00
	cmpb	#'0'
	bcs	ret0
	cmpb	#'9'+1
	bcs	ret
	orab	#$20
	cmpb	#'a'
	bcs	ret0
	cmpb	#'z'+1
	tab
	rolb
	rts
ret00:
	clra
ret0:
	clrb
ret:
	rts
