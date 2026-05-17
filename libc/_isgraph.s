	.export	_isgraph
	.code
;
; int isgraph(int c)
; {
;   return (c >= 0x21 && c <= 0x7E);
; }
;
_isgraph:
	tsta
	bne	ret00
	cmpb	#$21
	bcs	ret0
	cmpb	#$7E+1
	tab		; return (Carry==1);
	rolb
	rts
ret00:	clra
ret0:	clrb
	rts
