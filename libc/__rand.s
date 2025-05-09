;
;	int rand(void)
;		make random value in range 0-RAND_MAX(32767)
;
;	int srand(unsigned int seed)
;

	.data
__seed:	.byte	$de
	.byte	$ad
	.byte	$be
	.code
	.export	_rand
;
; 24bit random number generator
; https://wimcouwenberg.wordpress.com/2020/11/15/a-fast-24-bit-prng-algorithm-for-the-6502-processor/
;
_prng24:
	ldab	__seed
	aslb
	eorb	__seed+1
	stab	__seed+1
	rolb
	eorb	__seed+2
	stab	__seed+2
	eorb	__seed
	stab	__seed
	ldab	__seed+1
	rorb
	eorb	__seed+2
	stab	__seed+2
	eorb	__seed+1
	stab	__seed+1
	rts
_rand:
	bsr	_prng24
	ldaa	__seed
	lsra
	rorb
	rts
;
_srand:
	tstb
	bne	__seed_not_zero
	tsta
	bne	__seed_not_zero
	ldab	#$de
	stab	__seed+2
	ldab	#$ad
	stab	__seed+1
	ldab	#$be
	stab	__seed
	rts
__seed_not_zero:
	stab	__seed+1
	staa	__seed
	eorb	__seed
	stab	__seed+2
	rts
