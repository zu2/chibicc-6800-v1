;
;	int abs(int n)
;	{
;          return (n>0)? n: -n;
;	}
;
;	AccAB: n
;
	.data
	.code
	.export	_abs
_abs:
	tsta
        bpl __abs_ret
__neg16:
	nega
	negb
	sbca	#0
__abs_ret:
	rts
;
