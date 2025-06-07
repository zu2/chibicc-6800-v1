;
;	ins / popx / stx bp
;
	.export __popx_stxbp
	.export __ins8_popx_stxbp
	.export __ins7_popx_stxbp
	.export __ins6_popx_stxbp
	.export __ins5_popx_stxbp
	.export __ins4_popx_stxbp
	.export __ins3_popx_stxbp
	.export __ins2_popx_stxbp
	.export __ins1_popx_stxbp
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
;
__ins8_popx_stxbp:
        ins
__ins7_popx_stxbp:
        ins
__ins6_popx_stxbp:
        ins
__ins5_popx_stxbp:
        ins
__ins4_popx_stxbp:
        ins
__ins3_popx_stxbp:
        ins
__ins2_popx_stxbp:
        ins
__ins1_popx_stxbp:
        ins
__popx_stxbp:
        bsr __popx
        stx @bp
        rts
