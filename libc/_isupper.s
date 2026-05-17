	.export	_isupper
	.code
;
; int isupper(int c)
; {
;   return (c >= 'A' && c <= 'Z');
; }
;
_isupper:
	tsta
	bne	ret00
	cmpb	#'A'
	bcs	ret0
	cmpb	#'Z'+1
	tab
	rolb
	rts
ret00:
	clra
ret0:
	clrb
	rts
