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
__work:	.byte	0
	.code
	.export	_rand
;
_rand:
	ldab	__seed+2	; x ^= (x << 13) & 0xFFFFFF ; x<<13 = x<<(8+5)
	ldaa	__seed+1
	aslb
	rola
	aslb
	rola
	aslb
	rola
	aslb
	rola
	aslb
	rola
	eorb	__seed+1
	eora	__seed
	stab	__seed+1
	staa	__seed
;
;	ldaa	__seed		; x ^= (x >> 17) ; x>>17 = x>>(16+1)
	lsra
	eora	__seed+2
	staa	__seed+2	
;				; now AccB == seed+1, AccA == seed+2
	staa	__work
	ldaa	__seed
	asl	__work		; x<<5
	rolb
	rola
	asl	__work
	rolb
	rola
	asl	__work
	rolb
	rola
	asl	__work
	rolb
	rola
	asl	__work
	rolb
	rola
	eorb	__seed+1
	stab	__seed+1
	eora	__seed
	staa	__seed
	ldab	__work
	eorb	__seed+2
	stab	__seed+2
;
	ldaa	__seed		; now, AccA:B = __seed:__seed+1
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
	stab	__seed+1
	rts
