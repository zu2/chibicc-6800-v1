;
;	@long << TOS
;	@long >> TOS
;
	.export __shl32
	.export __shr32u
	.export __shr32s
	.code

;	Is the number of shifts correct?
count_check:		; tos >= 31?
	tst 2,x		; tst reset C
	bne check_ret
	tst 3,x
	bne check_ret
	tst 4,x
	bne check_ret
	ldab 5,x
	cmpb #32	; if tos>32 then C=0 else C=1
check_ret:
	rts		; if C==1  b has 5,x

	
__shl32:
	tsx
	bsr count_check
	bcc zero
shlloop:
	lsl @long+3
	rol @long+2
	rol @long+1
	rol @long
	decb
	bne shlloop
	rts
zero:
	clrb
	stab @long+3
	stab @long+2
	stab @long+1
	stab @long
	rts
;
__shr32u:
	tsx
	bsr count_check
	bcc zero
shruloop:
	lsr @long
	ror @long+1
	ror @long+2
	ror @long+3
	decb
	bne shruloop
	rts
;
__shr32s:
	tsx
	bsr count_check
	bcc zero
shrsloop:
	asr @long
	ror @long+1
	ror @long+2
	ror @long+3
	decb
	bne shrsloop
	rts
