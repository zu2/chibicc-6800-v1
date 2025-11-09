;
;	long basic library add32
;
	.export __inc32
	.export __dec32

	.code
;
;
;	__inc32	        @long++
;
__inc32:
        ldx @long+2
        inx
        stx @long+2
        bne __inc32_ret
        ldx @long
        inx
        stx @long
__inc32_ret:
	rts
;
;	__dec32        	@long--
;
__dec32:
        ldx @long+2
        dex
        stx @long+2
        inx
        bne __dec32_ret
        ldx @long
        dex
        stx @long
__dec32_ret:
        rts
