	.export	_isprint
	.code
;
; int isprint(int c)
; {
;   return (c >= 0x20 && c <= 0x7E);
; }
;
_isprint:
	tsta
	bne	ret00
	cmpb	#$20
	bcs	ret0
	cmpb	#$7E+1
	tab		; return (Carry==1);
	rolb
	rts
ret00:	clra
ret0:	clrb
	rts
