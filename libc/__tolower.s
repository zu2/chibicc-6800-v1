;
;	int tolower(int c)
;	{
;	  return (c>='A" && c<='Z')? c-'A'+'a': c;
;	}
;
;	AccAB: c
;
;	stack:
;	+0 ret addr
;

	.code
	.export	_tolower
_tolower:
	clra
	cmpb	#'A'
	bcs	_tolower_end
	cmpb	#'Z'+1
	bcc	_tolower_end
	addb	#$20
_tolower_end:
	rts
;
