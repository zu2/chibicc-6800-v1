	.export _isdigit
	.export _islower
	.export _isupper
	.export _isalpha
	.export _isalnum
	.export _isxdigit
	.export _isspace
	.export _isprint
	.export _ispunct
	.export _ispunct
	.export _iscntl
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
	bne	__ret0
	cmpb	#'A'
	bcs	_isdigit
	cmpb	#'f'+1
	bcc	_isdigit
	cmpb	#'F'+1
	bcs	__ret1
	cmpb	#'a'
	bcc	__ret1
;
_isdigit:			; return (c >= '0' && c <= '9')
	tsta
	bne	__ret0
	cmpb	#'0'
	bcs	__ret0
	cmpb	#'9'+1
	bcc	__ret0
;	clra			; At this point, AccA is 0.
	ldab	#1
	rts
;
_isalnum:			; isalpha(c) || isdigit(c)
	bsr	_isdigit
	bne	__ret
_isalpha:			; isupper(c) || islower(c)
	bsr	_islower
	bne	__ret
_isupper:			; (c >= 'A' && c <= 'Z')
	eorb	#$20		; Change case: A-Z to a-z, and vice versa.
_islower:			; (c >= 'a' && c <= 'z')
	tsta
	bne	__ret0
	cmpb	#'a'
	bcs	__ret0
	cmpb	#'z'+1
	bcc	__ret0
;	clra			; At this point, AccA is 0.
__ret1:
	ldab	#1
__ret:
	rts
__ret0:
	clra
	clrb
	rts
;
;	isspace: (c == ' '  || c == '\f' || c == '\n'
;              || c == '\r' || c == '\t' || c == '\v')
_isspace:
	tsta
	bne	__ret0
	cmpb	#' '
	beq	__ret1
	cmpb	#'\f'
	beq	__ret1
	cmpb	#'\n'
	beq	__ret1
	cmpb	#'\r'
	beq	__ret1
	cmpb	#'\t'
	beq	__ret1
	cmpb	#'\v'
	beq	__ret1
;	clra			; At this point, AccA is 0.
	clrb
	rts
;
_ispunct:			; isprint(c) && !isspace(c) && !isalnum(c)
	bsr	_isalnum
	bne	__ret0
	bsr	_isspace
	bne	__ret0
;
_isprint:			; (c >= 0x20 && c <= 0x7E)
	tsta
	bne	__ret0
	cmpb	#$20
	bcs	__ret0
	cmpb	#$7E+1
	bcs	__ret0
;	clra			; At this point, AccA is 0.
	ldab	#1
	rts
;
_iscntl:			; (c < 0x20) || (c == 0x7F)
	tsta
	bne	__ret0
	cmpb	#$20
	bcs	__ret0
	cmpb	#$7F
	bne	__ret0
	ldab	#1
	rts
;
_isblank:			; (c == ' ' || c == '\t')
	tsta
	bne	__ret0
	cmpb	#$20
	beq	__ret1
	cmpb	#'\t'
	beq	__ret1
;	clra			; At this point, AccA is 0.
	clrb
	rts
;
_tolower:			; isupper(c)? c + ('a' - 'A'): c;
	bsr	_isupper
	bne	__ret
	addb	#'a'-'A'
	rts
;
_toupper:			; islower(c)? c - ('a' - 'A'): c;
	bsr	_islower
	bne	__ret
	subb	#'a'-'A'
	rts
;


