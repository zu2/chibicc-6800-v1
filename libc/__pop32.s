;
;	long basic library pop32
;
	.export __pop32

	.code
;
;	__pop32		pul @long
;
__pop32:
	tsx
	ldx 0,x
	ins
	ins
	pulb
	stab @long
	pulb
	stab @long+1
	pulb
	stab @long+2
	pulb
	stab @long+3
	jmp 0,x
