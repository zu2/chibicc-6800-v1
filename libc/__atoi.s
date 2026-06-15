;
;	int atoi(const char char *s)
;	{
;	   int n = 0, sign = 1;
;          while (ispace(*nptr)) nptr++;
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
space_skip:
	inx
	ldab	0,x
	cmpb	#' '
	beq	space_skip
	cmpb	#$09
	bcs	space_skip_end
	cmpb	#$0d+1			; '\r'+1
	bcs	space_skip
;
space_skip_end:
	tba
	suba	#'-'
	staa	@tmp1			; if '-' sign=0, otherwise sign!=0
	beq	init
	cmpb	#'+'
	bne	not_plus
init:
	inx
	ldab	0,x
not_plus:
	clra
	staa	@tmp2+1			; low  byte of n
	staa	@tmp2			; high byte of n
	bra	loop_start
num_loop:
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
loop_start:
	cmpb	#'9'+1
	bcc	num_end
	subb	#'0'
	bcc	num_loop
num_end:
	ldab	@tmp2+1
	tst	@tmp1
	bne	num_plus
	nega
	negb
	sbca	#0
num_plus:
	rts
