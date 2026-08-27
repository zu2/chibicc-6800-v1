;
;	32-bit comparison
;
	.export __eq32x
	.export __eq32bx
	.export __eq32dx
	.export __ne32x
	.export __ne32bx
	.export __ne32dx
	.export __lt32sx
	.export __lt32sbx
	.export __lt32sdx
	.export __lt32ux
	.export __lt32ubx
	.export __lt32udx
	.export __gt32sx
	.export __gt32sbx
	.export __gt32sdx
	.export __gt32ux
	.export __gt32ubx
	.export __gt32udx
	.export __le32sx
	.export __le32sbx
	.export __le32sdx
	.export __le32ux
	.export __le32ubx
	.export __le32udx
	.export __ge32sx
	.export __ge32sbx
	.export __ge32sdx
	.export __ge32ux
	.export __ge32ubx
	.export __ge32udx
;
	.code
;
;	32-bit comparison, IX form
;
;	@long rel (0-3,x) ?
;
;	@long - (0-3,x), only cc affected
;
__sub_l_x:
	ldab	@long+3
	subb	3,x
	ldab	@long+2
	sbcb	2,x
	ldab	@long+1
	sbcb	1,x
	ldab	@long
	sbcb	0,x
	rts
;
;	(0-3,x) - @long, only cc affected
;
__sub_x_l:
	ldab	3,x
	subb	@long+3
	ldab	2,x
	sbcb	@long+2
	ldab	1,x
	sbcb	@long+1
	ldab	0,x
	sbcb	@long
	rts
;
;	@long == (0-3,x) ?
;
__eq32bx:
	clra
__eq32dx:
	jsr	__adx
__eq32x:
	stx	@tmp1
	ldx	2,x
	cpx	@long+2
	bne	__eq32x_0
	ldx	@tmp1
	ldx	0,x
	cpx	@long
	bne	__eq32x_0
	clra
	ldab	#1
	rts
__eq32x_0:
	clra
	clrb
	rts
;
;	@long != (0-3,x) ?
;
__ne32bx:
	clra
__ne32dx:
	jsr	__adx
__ne32x:
	stx	@tmp1
	ldx	2,x
	cpx	@long+2
	bne	__ne32x_1
	ldx	@tmp1
	ldx	0,x
	cpx	@long
	bne	__ne32x_1
	clra
	clrb
	rts
__ne32x_1:
	clra
	ldab	#1
	rts
;
;	@long < (signed) (0-3,x) ?
;
__lt32sbx:
	clra
__lt32sdx:
	jsr	__adx
__lt32sx:
	jsr	__sub_l_x
	blt	__lt32sx_1
	clra
	clrb
	rts
__lt32sx_1:
	clra
	ldab	#1
	rts
;
;	@long < (unsigned) (0-3,x) ?
;
__lt32ubx:
	clra
__lt32udx:
	jsr	__adx
__lt32ux:
	jsr	__sub_l_x
	bcs	__lt32ux_1
	clra
	clrb
	rts
__lt32ux_1:
	clra
	ldab	#1
	rts
;
;	@long >= (signed) (0-3,x) ?
;
__ge32sbx:
	clra
__ge32sdx:
	jsr	__adx
__ge32sx:
	jsr	__sub_l_x
	bge	__ge32sx_1
	clra
	clrb
	rts
__ge32sx_1:
	clra
	ldab	#1
	rts
;
;	@long >= (unsigned) (0-3,x) ?
;
__ge32ubx:
	clra
__ge32udx:
	jsr	__adx
__ge32ux:
	jsr	__sub_l_x
	bcc	__ge32ux_1
	clra
	clrb
	rts
__ge32ux_1:
	clra
	ldab	#1
	rts
;
;	@long > (signed) (0-3,x) ?
;
__gt32sbx:
	clra
__gt32sdx:
	jsr	__adx
__gt32sx:
	jsr	__sub_x_l
	blt	__gt32sx_1
	clra
	clrb
	rts
__gt32sx_1:
	clra
	ldab	#1
	rts
;
;	@long > (unsigned) (0-3,x) ?
;
__gt32ubx:
	clra
__gt32udx:
	jsr	__adx
__gt32ux:
	jsr	__sub_x_l
	bcs	__gt32ux_1
	clra
	clrb
	rts
__gt32ux_1:
	clra
	ldab	#1
	rts
;
;	@long <= (signed) (0-3,x) ?
;
__le32sbx:
	clra
__le32sdx:
	jsr	__adx
__le32sx:
	jsr	__sub_x_l
	bge	__le32sx_1
	clra
	clrb
	rts
__le32sx_1:
	clra
	ldab	#1
	rts
;
;	@long <= (unsigned) (0-3,x) ?
;
__le32ubx:
	clra
__le32udx:
	jsr	__adx
__le32ux:
	jsr	__sub_x_l
	bcc	__le32ux_1
	clra
	clrb
	rts
__le32ux_1:
	clra
	ldab	#1
	rts
;
