;
;	long basic library neg32
;
	.export __neg32
	.export __neg32x

	.code
;
;	__neg32		negate 32bit long, break IX
;	__neg32x	negate 32bit long (0-3,x)
;
__neg32:
        ldx #long
__neg32x:
	com 0,x
	com 1,x
	com 2,x
	neg 3,x
	bne __neg32x_ret
	inc 2,x
	bne __neg32x_ret
	inc 1,x
	bne __neg32x_ret
	inc 0,x
__neg32x_ret:
	rts
