;
;	16-bit subtraction
;
	.export __eq16
	.export __ne16
	.export __lt16s
	.export __lt16u
	.export __gt16s
	.export __gt16u
	.export __le16s
	.export __le16u
	.export __ge16s
	.export __ge16u
	.code
;
;	if AccAB!=0 then AccAB=0 else 1
;
__eq16:
	bne	__false
	tstb
	bne	__false2
	incb
	rts
__false:
	clra
__false2:
	clrb
	rts
;
;	if AccAB=0 then AccAB=0 else 1
;
__ne16:
	bne	__true
	tstb
	beq	__ret
__true:
	clra
	ldab #1
__ret:
	rts
;
;	a-b < 0 ?
;
__lt16s:
	blt	__true
	clrb
	clra
	rts
;
;       a-b < 0
;       If a < b, C becomes 1 and is moved to b's LSB.
;
__lt16u:
	ldaa	#0
	tab
        rolb
	rts
;	bcs	__true
;	clrb
;	clra
;	rts

;
;	a-b > 0 ?
;
__gt16s:
	bgt	__true
	blt	__false3
	tstb
	bne	__true
__false3:
	clrb
	clra
	rts
;
__gt16u:
	bhi	__true
	bcs	__false4
	tstb
	bne	__true
__false4:
	clrb
	clra
	rts
;
;	a-b <= 0 ?
;
__le16s:
	blt	__true
	bgt	__false5
	tstb
	beq	__true
__false5:
	clrb
	clra
	rts
;
__le16u:
	bcs	__true
	bhi	__false6
	tstb
	beq	__true
__false6:
	clrb
	clra
	rts
;
;	a-b >= 0
;
__ge16s:
	bge	__true
	clrb
	clra
	rts
;
;       If ! a >= b, C becomes 1 and !C moved to b's LSB.
;
__ge16u:
	ldaa	#0
	ldab	#1
	sbcb	#0
	rts
;	bcc	__true
;	clrb
;	clra
;	rts
