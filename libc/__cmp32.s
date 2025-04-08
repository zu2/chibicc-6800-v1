;
;	32-bit comparison
;
;	TOS rel @long ?
;
;
	.export __eq32
	.export __ne32
	.export __lt32s
	.export __lt32u
	.export __gt32s
	.export __gt32u
	.export __le32s
	.export __le32u
	.export __ge32s
	.export __ge32u
	.code
;
;	TOS == @long ?
;
__eq32:
	tsx
	ldx	4,x
	cpx	@long+2
	bne	__false
	tsx
	ldx	2,x
	cpx	@long
	bne	__false
	bra	__true
;
;	TOS != @long ?
;
__ne32:
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
	jmp	0,x
__true:
	tsx
	ldx	0,x
	clra
	ldab	#1
	bra	__ret
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
