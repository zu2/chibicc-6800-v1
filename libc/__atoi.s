;
;	int atoi(const char char *s)
;	{
;	   int n = 0, sign = 1;
;          while (*nptr == ' ' || *nptr == '\t') nptr++;
;          if (*nptr == '-') { sign = -1; nptr++; }
;          else if (*nptr == '+') nptr++;
;          while (*nptr >= '0' && *nptr <= '9') {
;            n = n * 10 + (*nptr++ - '0');
;          }
;          return sign * n;
;	}
;
;	AccAB: s
;

	.data
	.code
	.export	_atoi
_atoi:
	stab	@tmp1+1
	staa	@tmp1
	ldx	@tmp1
	dex
;
__atoi_skip:
	inx
	ldab	0,x
	cmpb	#' '
	beq	__atoi_skip
	cmpb	#'\t'
	beq	__atoi_skip
;
	subb	#'-'
	stab	@tmp1			; if '-' sign=0, otherwise sign!=0
	bne	__atoi_not_minus
	inx
__atoi_not_minus:
	ldab	0,x
	cmpb	#'+'
	bne	__atoi_not_plus
	inx
	ldab	0,x
__atoi_not_plus:
	clra
	staa	@tmp2+1			; low  byte of n
	staa	@tmp2			; high byte of n
	bra	__atoi_loop_start
__atoi_num_loop:
	stab	@tmp1+1			; save b
	ldab	@tmp2+1	
;	ldaa	@tmp2			; AccA keeps @tmp2
	aslb				; AccABx10: (n<<2+n)<<2
	rola
	aslb
	rola
	addb	@tmp2+1
	adca	@tmp2
	aslb
	rola
	addb	@tmp1+1
	adca	#0
	stab	@tmp2+1
	staa	@tmp2
	inx
	ldab	0,x
__atoi_loop_start:
	cmpb	#'9'+1
	bcc	__atoi_num_end
	subb	#'0'
	bcc	__atoi_num_loop
__atoi_num_end:
	ldab	@tmp2+1
	tst	@tmp1
	bne	__atoi_num_plus
	nega
	negb
	sbca	#0
__atoi_num_plus:
	rts
