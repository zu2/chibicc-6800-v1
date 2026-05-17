	.export	_islower
	.export _isalpha
	.code
;
; int islower(int c)
; {
;   return (c >= 'a' && c <= 'z');
; }
;
_isalpha:
	orab	#$20
_islower:
	tsta
	bne	ret00
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
	rts
