;
;	char *itoa(int value, char *string, int radix);
;	char *uitoa(unsigned int value, char *string, int radix);
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
;	working area:
;	__value	value
;	__p	string pointer
;	__f	first digit
;	__sign	sign
;
	.data
__value:.word	0
__p:	.word	0
__q:	.word	0
	.export _itoa
	.export __itoa
	.export _uitoa
	.export __uitoa
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
_itoa:
__itoa:
	stab __value+1		; save value
	staa __value		;
        tsx
        bsr __radix_check
        bne __uitoa_ret_null
;
	ldab 5,x		; check radix==10?
	cmpb #10
        bne __uitoa_null_check
        tsta
        bpl __uitoa_null_check
;
        nega                    ; value = -value
        neg __value+1
        sbca #0
        staa __value
;
        ldx 2,x
        beq __uitoa_ret_null
        ldab #'-'
        stab 0,x
        inx
        bra __uitoa_main
;
_uitoa:
__uitoa:
	stab __value+1		; save value
	staa __value		;
        tsx
        bsr __radix_check
        bne __uitoa_ret_null
__uitoa_null_check:
	ldx 2,x
	bne __uitoa_main
__uitoa_ret_null:
	clrb
	clra
	rts
;
__uitoa_main:
	stx __p
;
	ldx __value
	bne __uitoa_not_0
;
	ldx __p
	ldab #$30		; '0'
	stab 0,x
	inx
	clr 0,x
;
__uitoa_ret_str:
	tsx			; return str
	ldab 3,x
	ldaa 2,x
	rts
;
__uitoa_not_0:
	ldx __p
	stx __q			; most significant digit
__uitoa_loop:
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
	bcc __uitoa_hex
	addb #$30		; +'0'
	bra __uitoa_store
;
__uitoa_hex:
	addb #$57		; +'a'-10
__uitoa_store:
	ldx __p
	stab 0,x
	inx
	stx __p
	ldx @tmp2		; value /= radix
	stx __value
	bne __uitoa_loop
;
	ldx __p
	clr 0,x
	dex			; least significant digit
	stx __p
;
;	reverse __p to __q
;
	ldx __q
__uitoa_rev_loop:
	ldab __q+1
	ldaa __q
	subb __p+1
	sbca __p
	bge __uitoa_ret_str
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
	bra	__uitoa_rev_loop
;
