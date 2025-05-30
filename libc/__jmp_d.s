;
;   jmp [AccD]
;
	.export __jmp_d
	.code
__jmp_d:
        pshb
        psha
        rts
