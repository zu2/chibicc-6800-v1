;
;       Compare a signed 16-bit value in AccAB with zero.
;       No subtraction is assumed. Reads N (and Z) only, never V or C.
;
	.export __eq16_zero
	.export __ne16_zero
	.export	__lt16s_zero
	.export	__ge16s_zero
	.export	__gt16s_zero
	.export	__le16s_zero
	.code
;
;	if AccAB==0 then AccAB=1 else 0
;
__eq16_zero:
	aba
	adca #0
	nega
	ldaa #0
	ldab #1
	sbcb #0
	rts
;
;	if AccAB!=0 then AccAB=1 else 0
;
__ne16_zero:
	aba
	adca #0
	nega
	ldaa #0
	tab
	adcb #0
	rts
;
;	AccAB < 0 ?
;
__lt16s_zero:
	tsta
        bmi     __true_z
        clrb
        clra
        rts
;
;	AccAB >= 0 ?
;
__ge16s_zero:
	tsta
        bpl     __true_z
        clrb
        clra
        rts
;
;	AccAB <= 0 ?
;
__le16s_zero:
	tsta
        bmi     __true_z
	bne	__false_z
        tstb
        beq     __true_z
__false_z:
        clrb
        clra
        rts
;
;	AccAB > 0 ?
;
__gt16s_zero:
	tsta
        bmi     __false_z
	bne	__true_z
        tstb
        beq     __false_z
__true_z:
	clra
	ldab #1
	rts
