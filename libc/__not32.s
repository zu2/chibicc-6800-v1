;
;	long basic library not32
;
	.export __not32

	.code
;
__not32:
	com	@long+3
	com	@long+2
	com	@long+1
	com	@long
	rts
