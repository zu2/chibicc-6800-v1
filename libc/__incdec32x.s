;
;	long basic library inc32x/dec32x
;
	.export __inc32x
	.export __inc32bx
	.export __inc32dx
	.export __dec32x
	.export __dec32bx
	.export __dec32dx

	.code
;
;
;	__inc32x	++(0-3,x)
;
__inc32bx:
	clra
__inc32dx:
	jsr  __adx
__inc32x:
	inc 3,x
        bne iret
        inc 2,x
        bne iret
        inc 1,x
        bne iret
        inc 0,x
iret:	rts
;
;	__dec32        	--(0-3,x)
;
__dec32bx:
	clra
__dec32dx:
	jsr  __adx
__dec32x:
        ldab 3,x
	bne skip3
	ldab 2,x
	bne skip2
        ldab 1,x
	bne skip1
	dec 0,x
skip1:	dec 1,x
skip2:	dec 2,x
skip3:	dec 3,x
        rts
