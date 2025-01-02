;
;   AccAB = (AccAB)
;
	.export __load8u
	.export __load8s
	.export __load16
	.export __load32
	.code
__load8u:
	pshb
	psha
	tsx
	ldx 0,x
	ins
	ins
	ldab 0,x
	clra
	rts
__load8s:
	pshb
	psha
	tsx
	ldx 0,x
	ins
	ins
	clra
	ldab 0,x
	bpl __load8s2
	coma
__load8s2:
	rts
__load16:
	pshb
	psha
	tsx
	ldx 0,x
	ins
	ins
	ldab 1,x
	ldaa 0,x
	rts
__load32:
	pshb
	psha
	tsx
	ldx 0,x
	ins
	ins
	ldab 3,x
	stab @long+3
	ldab 2,x
	stab @long+2
	ldab 1,x
	stab @long+1
	ldab 0,x
	stab @long+0
	rts
