	.export	_ispunct
	.code
; int ispunct(int c)
; {
;   return isprint(c) && !isspace(c) && !isalnum(c);
; }
_ispunct:
	tsta
	bne	ret00
	cmpb	#$21
	bcs	ret0
	cmpb	#$2f+1
	bcs	retc
	cmpb	#$3a
	bcs	ret0
	cmpb	#$40+1
	bcs	retc
	cmpb	#$5b
	bcs	ret0
	cmpb	#$60+1
	bcs	retc
	cmpb	#$7b
	bcs	ret0
	cmpb	#$7e+1
	tab
retc:	rolb
	rts
ret00:	clra
ret0:	clrb
	rts
