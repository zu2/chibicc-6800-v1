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
        inc @long+3
        bne ret
        inc @long+2
        bne ret
        inc @long+1
        bne ret
        inc @long
ret:
	rts
;
;	__dec32        	@long--
;
__dec32:
	tst @long+3
	bne dec32_3
	tst @long+2
	bne dec32_2
	tst @long+1
	bne dec32_1
	dec @long
dec32_1:dec @long+1
dec32_2:dec @long+2
dec32_3:dec @long+3
        rts
