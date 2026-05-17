	.export	_isdigit
	.code
;
; int isdigit(int c)
; {
;   return ('0' <= c && c <= '9');
; }
;
_isdigit:
	tsta
	bne	ret00
	cmpb	#'0'
	bcs	ret0
	cmpb	#'9'+1
	tab
	rolb
	rts
ret00:
	clra
ret0:
	clrb
	rts
