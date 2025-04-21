;
;	int toupper(int c)
;	{
;	  return (c>='a" && c<='z')? c-'a'+'A': c;
;	}
;
;	AccAB: c
;
;	stack:
;	+0 ret addr
;

	.code
	.export	_toupper
_toupper:
	clra
	cmpb	#'a'
	bcs	_toupper_end
	cmpb	#'z'+1
	bcc	_toupper_end
	subb	#$20
_toupper_end:
	rts
;
