.global main
.text

main:
	MOVS r0, #0
	MOVS r1, #-1
	MOVS r2, #30
	MOVS r3, #4
	ADDS r4, r0, r3, LSL #1
	ADDS r5, r4, LSL r2
	swi 0x123456
