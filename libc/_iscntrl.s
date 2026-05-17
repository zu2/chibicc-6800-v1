	.export	_iscntrl
	.code
;
; int iscntrl(int c) {
;   return (0 <=c && c < 0x20) || (c == 0x7F)
; }
;
_iscntrl:
	tsta
	bne	ret0
	cmpb	#$20
	bcs	ret1
	cmpb	#$7F		; DEL
	beq	ret
ret0:	clrb
	clra
	rts
ret1:	incb			; Any non‑zero return is true.
ret:	rts
