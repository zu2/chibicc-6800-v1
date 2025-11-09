;
;	long basic library
;
	.export __load32
	.export __load32i
	.export __load32x
	.export __load32bx
	.export __load32dx
	.export __store32x
	.export __store32bx
	.export __store32dx
	.export __load32x_push
        .export __load32bx_push
	.export __push32
	.export __push32x
	.export __push32bx
	.export __push32dx

	.code
__load32:		; load 32bit from (AccAB)
        jsr __tfr_dx
        bra __load32x
;
__load32i:		; load 32bit immediate
	tsx
	ldx 0,x
	ins
	ins
        bsr __load32x
	jmp  4,x
;
__load32bx:             ; load 32bit from b,x
        clra
__load32dx:		; load 32bit from d,x
	jsr  __adx
__load32x:		; load 32bit from (0-3,x)
	ldab 3,x
	stab @long+3
	ldab 2,x
	stab @long+2
	ldab 1,x
	stab @long+1
	ldab 0,x
	stab @long+0
	rts
;
;	__store32x	store long to 0-3,x
;	__store32bx	store long to b,x  (mess IX)
;	__store32dx	store long to d,x  (mess IX)
;
__store32bx:
        clra
__store32dx:
	jsr __adx
__store32x:
	ldab @long+3
	stab 3,x
	ldab @long+2
	stab 2,x
	ldab @long+1
	stab 1,x
	ldab @long+0
	stab 0,x
	rts
;
;	__load32x & __push32
;
__load32x_push:
	jsr __load32x
        bra __push32
__load32bx_push:
        jsr __load32bx
;       bra __push32
;
;	__push32	push @long
;
__push32:
	tsx
	ldx 0,x
	ins
	ins
	ldab @long+3
	pshb
	ldab @long+2
	pshb
	ldab @long+1
	pshb
	ldab @long
	pshb
	jmp 0,x
;
;	__push32x	push 0-3,x
;	__push32bx	push b,x
;	__push32dx	push d,x
;
__push32bx:
        clra
__push32dx:
	jsr	__adx
__push32x:
	pulb
	stab	@tmp1
	pulb
	stab	@tmp1+1
	ldab	3,x
	pshb
	ldab	2,x
	pshb
	ldab	1,x
	pshb
	ldab	0,x
	pshb
	ldx	@tmp1
	jmp	0,x
