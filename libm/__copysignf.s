	.zp
	.data
	.export _copysignf
;
;	copy sign TOS -> @long
;
_copysignf:
	tsx
	ldab	2,x
	andb	#$80
	ldaa	@long
	anda	#$7F
	aba
	staa	@long
	rts
