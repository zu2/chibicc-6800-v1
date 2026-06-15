	.export	_isspace
	.code
;
;	isspace: (c == ' '  || c == '\f' || c == '\n'
;	       || c == '\r' || c == '\t' || c == '\v')
;
;    -> isspace: (c == ' ' || ('\t' <= c && c <= '\n'))
;
_isspace:
	tsta
	bne	ret00
	cmpb	#' '
	beq	ret
	cmpb	#$09		; \t
	bcs	ret0
	cmpb	#$0D+1		; \r
	tab
	rolb
ret:
	rts
ret00:
	clra
ret0:
	clrb
	rts
