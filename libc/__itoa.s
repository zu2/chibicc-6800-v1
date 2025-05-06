;
;	char *_itoa(int value, char *string, int radix);
;
;	convert value to sting with radix (2-36).
;
;	AccAB:	value
;
;	stack:
; 	    +0	return address
;	    +2	string
;	    +4	radix
;
;	work:
;	__value	value
;	__p	string pointer
;	__f	first digit
;	__sign	sign
;
	.data
__value:.word	0
__p:	.word	0
__q:	.word	0
	.export __itoa
	.code
__itoa:
	tsx
	ldx 2,x
	stx __p
	bne __itoa_not_null
;
__itoa_ret_null:
	clrb
	clra
	rts
;
__itoa_not_null:
	stab __value+1		; save value
	staa __value		;
;
	tsx
	tst 4,x			; check radix high byte
	bne __itoa_ret_null
	ldab 5,x		; check radix low byte
	cmpb #2
	bcs __itoa_ret_null
	cmpb #37
	bcc __itoa_ret_null
;
	ldx __value
	bne __itoa_not_0
;
	ldx __p
	ldab #$30		; '0'
	stab 0,x
	inx
	clr 0,x
;
__itoa_ret_str:
	tsx			; return str
	ldab 3,x
	ldaa 2,x
	rts
;
__itoa_not_0:
	tsx
	ldab 5,x		; check radix==10?
	cmpb #10
	bne __itoa_not_minus
	tsta			; value < 0 ?
	bpl __itoa_not_minus
;				; radix==10 && value < 0
	ldx __p
	ldab #$2D		; '-'
	stab 0,x
	inx
	stx __p
;
	ldab __value+1
	nega
	negb
	sbca #0
	stab __value+1		; save value
	staa __value
;
__itoa_not_minus:
	ldx __p
	stx __q			; most significant digit
__itoa_loop:
	ldab __value+1
	ldaa __value
	tsx
	pshb
	psha
	ldab 5,x		; radix
	ldaa 4,x
	jsr __div16x16		; AccAB = TOS % AccAB	@tmp2 = TOS / AccAB
	ins
	ins
;
	cmpb #10		; (value % radix) < 10?
	bcc __itoa_hex
	addb #$30		; +'0'
	bra __itoa_store
;
__itoa_hex:
	addb #$57		; +'a'-10
__itoa_store:
	ldx __p
	stab 0,x
	inx
	stx __p
	ldx @tmp2		; value /= radix
	stx __value
	bne __itoa_loop
;
	ldx __p
	clr 0,x
	dex			; least significant digit
	stx __p
;
;	reverse __p to __q
;
	ldx __q
__itoa_rev_loop:
	ldab __q+1
	ldaa __q
	subb __p+1
	sbca __p
	bge __itoa_ret_str
	ldab 0,x
	ldx __p
	ldaa 0,x
	stab 0,x
	dex
	stx __p
	ldx __q
	staa 0,x
	inx
	stx __q
	bra	__itoa_rev_loop
;
