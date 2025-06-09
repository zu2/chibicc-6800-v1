;
;	pul @bp & rts
;
	.export __pulbp_rts
	.code
;
__pulbp_rts:
        pulb
        stab @bp
        pulb
        stab @bp+1
        rts

