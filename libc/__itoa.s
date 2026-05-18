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
;	value	value
;	p	string pointer
;
	.data
value:	.word	0
radix:	.byte	0
p:	.word	0
q:	.word	0
len:	.byte	0
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
radix_check:
	ldab 5,x		; check radix low byte
	stab radix		; save radix
	cmpb #2
	bcs radix_check_ret
	cmpb #36
	bhi radix_check_ret
	tst 4,x			; check radix high byte
radix_check_ret:
        rts
;
_itoa:
__itoa:
	stab value+1		; save value
	staa value		;
        tsx
        bsr radix_check
        bne uitoa_ret_null
;
	cmpb #10		; check radix==10?
        bne uitoa_null_check
        tsta
        bpl uitoa_null_check
;
        nega                    ; value = -value
        neg value+1
        sbca #0
        staa value
;
        ldx 2,x
        beq uitoa_ret_null
        ldab #'-'
        stab 0,x
        inx
        bra uitoa_main
;
_uitoa:
__uitoa:
	stab value+1		; save value
	staa value		;
        tsx
        bsr radix_check
        bne uitoa_ret_null
uitoa_null_check:
	ldx 2,x
	bne uitoa_main
uitoa_ret_null:
	clrb
	clra
	rts
;
uitoa_main:
	stx p
;
	ldx value
	bne uitoa_not_0
;
	ldx p
	ldab #'0'
	stab 0,x
	inx
	clr 0,x
;
uitoa_ret_str:
	tsx			; return str
	ldab 3,x
	ldaa 2,x
	rts
;
uitoa_not_0:
	ldx p
	stx q			; the most significant digit
	clr len
;				; Calculating each digit requires division.
;				; AccB = value % radix, value /= radix
	ldaa value+1
uitoa_loop:
	ldx #16
	clrb
div_loop:
	asla
	rol value
	rolb
	inca
	subb radix
	bcc div_skip
	addb radix
	deca
div_skip:
	dex
	bne div_loop
;
	addb #'0'		; +'0'
	cmpb #'0'+10		; (value % radix) < 10?
	bcs uitoa_store
	addb #'a'-'9'-1		; +'a'-('9'+1)
uitoa_store:
	ldx p
	stab 0,x
	inx
	stx p
	inc len
	staa value+1
	ldx value
	bne uitoa_loop		; loop until value==0
;
	ldx p
	clr 0,x
	dex
	stx p
;
;	Reverse the string from q to p, len bytes
;
	lsr len
	beq uitoa_ret_str
uitoa_rev_loop:
	ldab 0,x
	ldx q
	ldaa 0,x
	stab 0,x
	inx
	stx q
	ldx p
	staa 0,x
	dex
	stx p
	dec len
	bne uitoa_rev_loop
	bra uitoa_ret_str
;
