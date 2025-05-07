;
;	char *_ltoa(int value, char *string, int radix);
;
;	convert value to sting with radix (2-36).
;
;	@long:	value
;
;	stack:
; 	    +0	return address
;	    +2	string
;	    +4	radix
;
;	work:
;	__p	string pointer
;	__f	first digit
;	__sign	sign
;
	.data
__p:	.word	0
__q:	.word	0
	.export _ltoa
	.export __ltoa
	.code
_ltoa:
__ltoa:
	tsx
	ldx 2,x
	stx __p
	bne __ltoa_not_null
;
__ltoa_ret_null:
	clrb
	clra
	rts
;
__ltoa_not_null:
;
	tsx
	tst 4,x			; check radix high byte
	bne __ltoa_ret_null
	ldab 5,x		; check radix low byte
	cmpb #2
	bcs __ltoa_ret_null
	cmpb #37
	bcc __ltoa_ret_null
;
	ldx @long
	bne __ltoa_not_0
	ldx @long+2
	bne __ltoa_not_0
;
	ldx __p
	ldab #$30		; '0'
	stab 0,x
	inx
	clr 0,x
;
__ltoa_ret_str:
	tsx			; return str
	ldab 3,x
	ldaa 2,x
	rts
;
__ltoa_not_0:
	tsx
	ldab 5,x		; check radix==10?
	cmpb #10
	bne __ltoa_not_minus
	tst @long
	bpl __ltoa_not_minus
;				; radix==10 && value < 0
	ldx __p
	ldab #$2D		; '-'
	stab 0,x
	inx
	stx __p
;
	jsr __neg32		; @long = -@long, breaks IX
;
__ltoa_not_minus:
	ldx __p
	stx __q			; most significant digit
__ltoa_loop:
	tsx
	ldab 5,x		; radix
	pshb
	clrb
	pshb
	pshb
	pshb
	jsr __div32x32		; @long = @long / TOS
				; @tmp1:AccAB = @long % TOS
	ins
	ins
	ins
	ins
;				; here, reminder < 36
	cmpb #10		; (value % radix) < 10?
	bcc __ltoa_hex
	addb #$30		; +'0'
	bra __ltoa_store
;
__ltoa_hex:
	addb #$57		; +'a'-10
__ltoa_store:
	ldx __p
	stab 0,x
	inx
	stx __p
	ldx @long+2
	bne __ltoa_loop
	ldx @long
	bne __ltoa_loop
;
	ldx __p
	clr 0,x
	dex			; least significant digit
	stx __p
;
;	reverse __p to __q
;
	ldx __q
__ltoa_rev_loop:
	ldab __q+1
	ldaa __q
	subb __p+1
	sbca __p
	bge __ltoa_ret_str
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
	bra	__ltoa_rev_loop
;
