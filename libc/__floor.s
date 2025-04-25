;
;	MC6800 floating point arithmetic library
;
;	Copyright (c) 2025 by ZUKERAN, shin
;	Released under the MIT license
;
;	https://github.com/zu2/chibicc-6800-v1?tab=License-1-ov-file#readme
;
;	Note: This program was created for testing chibicc-6800-v1, 
;	and does not pay attention to speed, accuracy, or exception handling.
;
;
	.export	_floor
	.export	_ceil
	.data
__floor_or_ceil:
	.byte	0	; floor:0, ceil:$80
;
	.code
;
;	@long = floor(@long)
;		parameter passed by @long
;
;	@long = ceil(@long)
;		parameter passed by @long
;
_floor:
	clr	__floor_or_ceil
	bra	__floor_common
_ceil:
	ldab	#$80
	stab	__floor_or_ceil
__floor_common:
	jsr	__f32iszero
	bne	_floor_non_zero
	rts
_floor_non_zero:
	ldaa	@long+1
	ldab	@long
	asla
	rolb
	cmpb	#150
	bcc	__floor_ret
	subb	#126		; if exp<=126 return +0.0/+1.0 or -1.0/0.0
	bhi	__floor_trunc
;
	ldx	#0
	stx	@long+2
	ldaa	__floor_or_ceil
	bmi	__ceil_lt1
;
__floor_lt1:			; floor(x): if x < 1.0
	ldaa	@long
	bpl	__floor_p0
	ldx	#$BF80		; -1.0
__floor_p0:
	stx	@long		; +0.0
__floor_ret:
	rts
;
__ceil_lt1:
	ldaa	@long
	bpl	__ceil_p1
	ldx	#$8000		; -0.0
	bra	__floor_p0
__ceil_p1:
	ldx	#$3F80		; +1.0
	bra	__floor_p0
;	
__floor_trunc:
	subb	#24
	ldaa	#$ff
	staa	__work+2
	staa	__work+1
__floor_loop:
	asla
	rol	__work+2
	rol	__work+1
	incb
	bne	__floor_loop
	clrb			; Check if any bit in the mask is 1
	pshb
	ldab	@long
	eorb	__floor_or_ceil
	pulb
	bpl	__floor_mask
;
	tab
	comb
	andb	@long+3
	stab	__work
;
	ldab	__work+2
	comb
	andb	@long+2
	orab	__work
	stab	__work
;
	ldab	__work+1
	comb
	andb	#$7F
	andb	@long+1
	orab	__work		; if b!=0, if any bit in the mask is 1
;
__floor_mask:
	anda	@long+3
	staa	@long+3
	ldaa	__work+2
	anda	@long+2
	staa	@long+2
	ldaa	__work+1
	anda	@long+1
	staa	@long+1
;
	tstb			; Check if any of the masked bits are set.
	beq	__floor_ret
	ldab	__floor_or_ceil
	bmi	__floor_p1	; ceil
;
__floor_m1:
	clrb			; add -1.0
	pshb
	pshb
	ldab	#$80
	pshb
	ldab	#$bf
	pshb
	jsr	__addf32tos
	rts
;
__floor_p1:
	clrb			; add +1.0
	pshb
	pshb
	ldab	#$80
	pshb
	ldab	#$3f
	pshb
	jsr	__addf32tos
	rts
