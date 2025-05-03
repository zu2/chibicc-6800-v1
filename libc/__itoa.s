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
;
	stab __value+1		; save value and check zero
	bne __itoa_not_0
	tsta
	bne __itoa_not_0
;
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
	stx __p
;
	tsx
	ldab 5,x		; check radix
	ldx __p
	cmpb #10
	bne __itoa_chk_0
	tsta			; AccA has high byte of value
	bpl __itoa_chk_0
;				; radix==10 && value < 0
	ldab #$2D		; '-'
	stab 0,x
	inx
	stx __p
	ldab __value+1
	nega
	negb
	sbca #0
	stab __value+1		; save value
__itoa_chk_0:
	ldab __value+1
	staa __value
	stx __q			; most significant digit
	bra __itoa_start;
__itoa_pre1:
	ldaa @tmp2
__itoa_pre2:
	staa __value
__itoa_start:
	tsx
	pshb
	psha
	ldab 5,x		; radix
	ldaa 4,x
	jsr __div16x16		; AccAB = TOS % AccAB	@tmp2 = TOS / AccAB
	ins
	ins
;
	cmpb #10		; < 10?
	bcc __itoa_hex
	addb #$30		; '0'
	bra __itoa_store
;
__itoa_hex:
	addb #$57		; 'a'-10
__itoa_store:
	ldx __p
	stab 0,x
	inx
	stx __p
	ldab @tmp2+1
	stab __value+1
	bne __itoa_pre1
	ldaa @tmp2
	bne __itoa_pre2
;
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

	


