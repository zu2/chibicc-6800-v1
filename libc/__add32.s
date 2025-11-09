;
;	long basic library add32
;
	.export __add32i
	.export __add32x
	.export __add32tos

	.code
;
;	__add32i	@long += #imm
;
__add32i:		; add 32bit immediate
	tsx
	ldx 0,x
	ins
	ins
        bsr __add32x
	jmp  4,x
;
;	__add32x	@long += (0-3,x)
;
__add32x:
	ldab 3,x
	addb @long+3
	stab @long+3
	ldab 2,x
	adcb @long+2
	stab @long+2
	ldab 1,x
	adcb @long+1
	stab @long+1
	ldab 0,x
	adcb @long
	stab @long
	rts
;
;	__add32tos	@long += TOS, pull TOS
;
__add32tos:
	pula
	pulb
	stab	@tmp1+1
	staa	@tmp1
	tsx
        bsr     __add32x
	ins
	ins
	ins
	ins
	ldx	@tmp1
	jmp	0,x
