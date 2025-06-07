;
;	popx()
;
;	+0: return address
;	+2: pop data
;
	.export __popx
	.code
;
__popx:
	tsx
	ldx  2,x
	stab @tmp1+1
	staa @tmp1
	pula
	pulb
	ins
	ins
	pshb
	psha
	ldab @tmp1+1
	ldaa @tmp1
	rts
