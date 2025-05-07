;
;	int abs(int n)
;	{
;          return (n>0)? n: -n;
;	}
;
;	AccAB: n
;
;	long labs(long n)
;	{
;          return (n>0)? n: -n;
;	}
;
;	@long: n
;

	.data
	.code
	.export	_abs
_abs:
	tsta
	bpl	__abs_ret
	nega
	negb
	sbca	#0
__abs_ret:
	rts
;
	.export	_labs
_labs:
	tst	@long
	jmi	__neg32
	rts
