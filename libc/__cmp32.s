;
;	32-bit comparison
;
;	TOS rel @long ?
;
;
	.export __eq32
	.export __eq32s
	.export __eq32u
	.export __ne32
	.export __ne32s
	.export __ne32u
	.export __lt32s
	.export __lt32u
	.export __gt32s
	.export __gt32u
	.export __le32s
	.export __le32u
	.export __ge32s
	.export __ge32u
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
;	TOS == @long ?
;
__eq32:
__eq32s:
__eq32u:
	tsx
	ldx	4,x
	cpx	@long+2
	bne	__false
	tsx
	ldx	2,x
	cpx	@long
	bne	__false
__true:
	tsx
	ldx	0,x
	clra
	ldab	#1
	bra	__ret
;
;	TOS != @long ?
;
__ne32:
__ne32s:
__ne32u:
	tsx
	ldx	4,x
	cpx	@long+2
	bne	__true
	tsx
	ldx	2,x
	cpx	@long
	bne	__true
	bra	__false
;
;	TOS < @long ?
;
__lt32s:
	bsr	__subtl		; TOS - @long
	blt	__true
	bra	__false
;
__lt32u:
	bsr	__subtl		; TOS - @long
	bcs	__true
__false:
	tsx
	ldx	0,x
	clrb
	clra
__ret:
	ins
	ins
	ins
	ins
	ins
	ins
        tstb                    ; Test AccB for comparison after return
	jmp	0,x
__sublt:			; @long - TOS only cc affected
	tsx
	ldab	@long+3
	subb	7,x
	ldab	@long+2
	sbcb	6,x
	ldab	@long+1
	sbcb	5,x
	ldab	@long
	sbcb	4,x
	rts
__subtl:			; TOS - @long, only cc affected
	tsx
	ldab	7,x
	subb	@long+3
	ldab	6,x
	sbcb	@long+2
	ldab	5,x
	sbcb	@long+1
	ldab	4,x
	sbcb	@long
	rts
;
;	TOS > @long ?		; @long < TOS ?
;
__gt32s:
	bsr	__sublt		; @long - TOS
	blt	__true
	bra	__false
;
__gt32u:
	bsr	__sublt
	bcs	__true
	bra	__false
;
;	TOS <= @long ? 		; @long >= TOS ?
;
__le32s:
	bsr	__sublt		; @long - TOS
	bge	__true
	bra	__false
;
__le32u:
	bsr	__sublt		; @long - TOS
	bcc	__true
	bra	__false
;
;	TOS >= @long ?
;
__ge32s:
	bsr	__subtl		; TOS - @long
	bge	__true
	bra	__false
;
__ge32u:
	bsr	__subtl		; TOS - @long
	bcc	__true
	bra	__false
;
;	32-bit comparison, IX form
;
;	@long rel (0-3,x) ?
;
;	@long - (0-3,x), only cc affected
;
__sublx:
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
__subxl:
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
	ldab	@long+3
	cmpb	3,x
	bne	__eq32x_0
	ldab	@long+2
	cmpb	2,x
	bne	__eq32x_0
	ldab	@long+1
	cmpb	1,x
	bne	__eq32x_0
	ldab	@long
	cmpb	0,x
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
	ldab	@long+3
	cmpb	3,x
	bne	__ne32x_1
	ldab	@long+2
	cmpb	2,x
	bne	__ne32x_1
	ldab	@long+1
	cmpb	1,x
	bne	__ne32x_1
	ldab	@long
	cmpb	0,x
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
	jsr	__sublx
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
	jsr	__sublx
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
	jsr	__sublx
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
	jsr	__sublx
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
	jsr	__subxl
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
	jsr	__subxl
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
	jsr	__subxl
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
	jsr	__subxl
	bcc	__le32ux_1
	clra
	clrb
	rts
__le32ux_1:
	clra
	ldab	#1
	rts
;
