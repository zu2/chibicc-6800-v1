;
;	MC6800 floating point arithmetic library
;
;	Copyright (c) 2025 by ZUKERAN, shin
;	Released under the MIT license
;
;	https://github.com/zu2/chibicc-6800-v1?tab=License-1-ov-file#readme
;

	.data
	.export	__NAN
	.export	__INFINITY
__NAN:
	.byte	$7F,$C0,$00,$00
__INFINITY:
	.byte	$7F,$80,$00,$00
