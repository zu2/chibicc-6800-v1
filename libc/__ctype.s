	.export _isdigit
	.export _islower
	.export _isupper
	.export _isalpha
	.export _isalnum
	.export _isxdigit
	.export _isspace
	.export _isprint
	.export _ispunct
	.export _iscntrl
	.export _isblank
	.export _tolower
	.export _toupper
;
	.code
;
;	isxdigit: (c >= 'A' && c <= 'F')
;              || (c >= 'a' && c <= 'f')
;              || isdigit(c)
;
_isxdigit:
	tsta
	bne	__ret_0
        bsr     __isdigit_test
        bcs     __ret_1
        orab	#$20
	cmpb	#'a'
	bcs	__ret_00
	cmpb	#'f'+1
        tab
        rolb
        rts
__ret_1:
        ldab    #1
        rts

;
; xxx_test returns the result in the carry flag.
;
__isdigit_test:			; return (c >= '0' && c <= '9')
	cmpb	#'0'
	bcs	__ret_c0
	cmpb	#'9'+1
	rts
__ret_c0:
        clc
        rts
;
_isdigit:
	tsta
	bne	__ret_0
        bsr     __isdigit_test
        tab
        rolb
        rts
;
_isalnum:			; isalpha(c) || isdigit(c)
	tsta
	bne	__ret_0
        bsr     __isdigit_test
        bcs     __ret_1
	bra	__isalpha_2	; Removing BRA saves 2 bytes but costs 2 cycles.
;
_isalpha:			; isupper(c) || islower(c)
	tsta
	bne	__ret_0
__isalpha_2:
	bsr	__isalpha_check
__ret_c:
        tab
        rolb
__ret:
        rts
;
__isupper_test:
        cmpb    #'A'
        bcs     __ret_c0
        cmpb    #'Z'+1
        rts
;
__isalpha_check:
        orab    #$20		; faster, but breaks AccB
__islower_test:
        cmpb    #'a'
        bcs     __ret_c0
        cmpb    #'z'+1
        rts
;
_isupper:			; (c >= 'A' && c <= 'Z')
	tsta
	bne	__ret_0
	bsr	__isupper_test
        tab
        rolb
        rts
;
_islower:			; (c >= 'A' && c <= 'Z')
	tsta
	bne	__ret_0
	bsr	__islower_test
        tab
        rolb
        rts
;
;	isspace: (c == ' '  || c == '\f' || c == '\n'
;              || c == '\r' || c == '\t' || c == '\v')
;
;    -> isspace: (c == ' ' || ('\t' <= c && c <= '\n'))
;
__isspace_test:
        cmpb    #' '
        beq     __ret_c1
	cmpb	#$09            ; \t
        bcs     __ret_c0
	cmpb	#$0D+1          ; \r
        rts
__ret_c1:
        sec
        rts
;
_isspace:
	tsta
	bne	__ret_0
        bsr     __isspace_test
        tab
        rolb
        rts
;
__ret_0:
        clra
__ret_00:			; when AccA == 0: use here, saves 2 cycles.
        tab
        rts
;
;
_ispunct:			; isprint(c) && !isspace(c) && !isalnum(c)
	tsta
	bne	__ret_0
	bsr	__isdigit_test
        bcs     __ret_00
        pshb
	bsr	__isalpha_check ; breaks AccB
        pulb
	bcs	__ret_00
	bsr	__isspace_test
	bcs	__ret_00
        bsr     __isprint_test
        tab
        rolb
        rts
;
__isprint_test:			; (c >= 0x20 && c <= 0x7E)
	cmpb	#$20
	bcs	__ret_c0
	cmpb	#$7E+1
	rts
;
_isprint:
	tsta
	bne	__ret_0
        bsr     __isprint_test
        tab
        rolb
        rts
;
_isblank:			; (c == ' ' || c == '\t')
	tsta
	bne	__ret_0
	cmpb	#$20
	beq	__ret_11
	cmpb	#$09            ; TAB
	beq	__ret_11
	clrb
	rts
__ret_11:
        ldab    #1
__ret_2:
        rts
;
; Undefined for values not in unsigned char range or EOF
;   returned unchanged here.
;
_tolower:			; isupper(c)? c + ('a' - 'A'): c;
	tsta
	bne	__ret_2
	bsr	__isupper_test
	bcc	__ret_2
	addb	#'a'-'A'
	rts
;
_toupper:			; islower(c)? c - ('a' - 'A'): c;
	tsta
	bne	__ret_2
	bsr	__islower_test
	bcc	__ret_2
	subb	#'a'-'A'
	rts
;
_iscntrl:			; (c < 0x20) || (c == 0x7F)
	tsta
	bne	__ret_0
	cmpb	#$20
	bcs	__ret_11
	cmpb	#$7F            ; DEL
        beq	__ret_11
        clrb
	rts
;
; Note:
;  This sequence assumes AccA == 0.
;  If C = 1, AccB becomes 1; if C = 0, AccB becomes 0. */
;
;	tab
;	rolb
;	rts
;
