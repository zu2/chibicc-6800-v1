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
;	TOS rel @long ?
;
;	IX points at TOS, so the IX form takes the reversed condition.
;
;
;	TOS == @long ?
;
__eq32:
__eq32s:
__eq32u:
	tsx
	inx
	inx
	jsr	__eq32x
	bra	__ret
;
;	TOS != @long ?
;
__ne32:
__ne32s:
__ne32u:
	tsx
	inx
	inx
	jsr	__ne32x
	bra	__ret
;
;	TOS < (signed) @long ?
;
__lt32s:
	tsx
	inx
	inx
	jsr	__gt32sx
	bra	__ret
;
;	TOS < (unsigned) @long ?
;
__lt32u:
	tsx
	inx
	inx
	jsr	__gt32ux
	bra	__ret
;
;	TOS > (signed) @long ?
;
__gt32s:
	tsx
	inx
	inx
	jsr	__lt32sx
	bra	__ret
;
;	TOS > (unsigned) @long ?
;
__gt32u:
	tsx
	inx
	inx
	jsr	__lt32ux
	bra	__ret
;
;	TOS <= (signed) @long ?
;
__le32s:
	tsx
	inx
	inx
	jsr	__ge32sx
	bra	__ret
;
;	TOS <= (unsigned) @long ?
;
__le32u:
	tsx
	inx
	inx
	jsr	__ge32ux
	bra	__ret
;
;	TOS >= (signed) @long ?
;
__ge32s:
	tsx
	inx
	inx
	jsr	__le32sx
	bra	__ret
;
;	TOS >= (unsigned) @long ?
;
__ge32u:
	tsx
	inx
	inx
	jsr	__le32ux
	bra	__ret
;
__ret:
	tsx
	ldx	0,x
	ins
	ins
	ins
	ins
	ins
	ins
        tstb                    ; Test AccB for comparison after return
	jmp	0,x
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
