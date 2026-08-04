;
;	8-bit rel op
;
;       Compare a signed 8-bit value in AccB with zero.
;       Entry: N and Z reflect AccB (from tstb or ldab).
;       Reads N and Z only, never V or C.
;
	.export __eq8_zero
	.export __ne8_zero
	.export	__lt8s_zero
	.export	__ge8s_zero
	.export	__le8s_zero
	.export	__gt8s_zero
	.code
;
;	if AccB=0 then AccAB=1 else 0
;
__eq8_zero:
	clra
	negb
	ldab #1
	sbcb #0
	rts
;
;	if AccB!=0 then AccAB=0 else 1
;
__ne8_zero:
	clra
	negb
	tab
	adcb #0
	rts
;
__lt8s_zero:
	tstb
        blt     __true_z
        clrb
        clra
        rts
;
__ge8s_zero:
	tstb
        bge     __true_z
__false_z:
        clrb
        clra
        rts
;
__le8s_zero:
	tstb
        ble     __true_z
        clrb
        clra
        rts
;
__gt8s_zero:
	tstb
        ble     __false_z
__true_z:
	clra
	ldab #1
	rts
