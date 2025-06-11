;
;	char *_ltoa(long value, char *string, int radix);
;	char *_ultoa(unsigned long value, char *string, int radix);
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
	.export _ultoa
	.export __ultoa
	.code
;
;       2<= radix <= 36 ?
;         Z=1: yes
;         Z=0: no
;
__radix_check:
	ldab 5,x		; check radix low byte
	cmpb #2
	bcs __radix_check_ret
	cmpb #36
	bhi __radix_check_ret
	tst 4,x			; check radix high byte
__radix_check_ret:
        rts
;
_ltoa:
__ltoa:
        tsx
        bsr __radix_check
        bne __ret_null
	ldab 5,x		; check radix==10?
	cmpb #10
        bne __ultoa_null_check
        tst @long
        bpl __ultoa_null_check
        jsr __neg32
        tsx
        ldx 2,x
        beq __ret_null
        ldab #'-'
        stab 0,x
        inx
        bra __ultoa_main
;
;
_ultoa:
__ultoa:
	tsx
        bsr __radix_check
        bne __ret_null
__ultoa_null_check:
	ldx 2,x
        bne __ultoa_main
__ret_null:
	clrb
	clra
	rts                     ; return NULL
__ultoa_main:
	stx __p
        stx __q
;
	ldx @long+2
	bne __ultoa_not_0
	ldx @long
	bne __ultoa_not_0
;
	ldx __p
	ldab #$30		; '0'
	stab 0,x
	inx
	clr 0,x
;
__ultoa_ret_str:
	tsx			; return str
	ldab 3,x
	ldaa 2,x
	rts
;
__ultoa_not_0:
;
__ultoa_loop:
	tsx
	ldab 5,x		; radix
	pshb
	clrb
	pshb
	pshb
	pshb
	jsr __div32x32		; @long = @long / TOS (unsigned)
				; @tmp1:AccAB = @long % TOS
	ins
	ins
	ins
	ins
;				; here, reminder < 36
	cmpb #10		; (value % radix) < 10?
	bcc __ultoa_hex
	addb #$30		; +'0'
	bra __ultoa_store
;
__ultoa_hex:
	addb #$57		; +'a'-10
__ultoa_store:
	ldx __p
	stab 0,x
	inx
	stx __p
	ldx @long+2
	bne __ultoa_loop
	ldx @long
	bne __ultoa_loop
;
	ldx __p
	clr 0,x
	dex			; least significant digit
	stx __p
;
;	reverse __p to __q
;
	ldx __q
__ultoa_rev_loop:
	ldab __q+1
	ldaa __q
	subb __p+1
	sbca __p
	bge __ultoa_ret_str
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
	bra	__ultoa_rev_loop
;
