;
;	8-bit subtraction
;
	.export __eq8
	.export __ne8
	.export __lt8s
	.export __lt8u
	.export __gt8s
	.export __gt8u
	.export __le8s
	.export __le8u
	.export __ge8s
	.export __ge8u
	.code
__eq8:
	bne	__false
        clra
	incb
	rts
__false:
	clra
__false2:
	clrb
	rts
;
;	AccAB=0 then AccAB=0 else 1
;
__ne8:
	beq	__false
__true:
	clra
	ldab #1
	rts
;
;	a-b < 0 ?
;
__lt8s:
	blt	__true
	clrb
	clra
	rts
;
__lt8u:
	ldaa	#0
	tab
	adcb	#0
	rts
;	bcs	__true
;	clrb
;	clra
;	rts
;
;	a-b > 0 ?
;
__gt8s:
	bgt	__true
__false3:
	clrb
	clra
	rts
;
__gt8u:
	bhi	__true
	clrb
	clra
	rts
;
;	a-b <= 0 ?
;
__le8s:
	ble	__true
	clrb
	clra
	rts
;
__le8u:
	bcs	__true
	clrb
	clra
	rts
;
;	a-b >= 0
;
__ge8s:
	bge	__true
	clrb
	clra
	rts
;
__ge8u:
	ldaa	#0
	ldab	#1
	sbcb	#0
	rts
;	bcc	__true
;	clrb
;	clra
;	rts
