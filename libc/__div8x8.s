;
;	AccB = AccB / 2,s
;
	.export __div8x8
	.export __div8x8u
	.export __mod8x8u
	.code
__mod8x8u:
	tsx
	ldaa 2,x
	psha
	bsr __div8x8u
	ins
	tab
	rts
__div8x8u:
__div8x8:
	tsx
        ldaa 2,x
	staa @tmp1	; divisor
	ldx #8		; loop counter
	clra		; carry is also cleared
loop:
        aslb            ; dividend
        rola
	cmpa @tmp1	; AccA >= divisor?
	bcs next
	suba @tmp1
        incb
next:
	dex
	bne loop
	rts
