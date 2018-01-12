.global main
.text

main:
	MOVS r0, #0xFFFFFFFF
	MOVS r1, #0x1
	ADDS r2, r1, r0
	ADCS r2, r1, #0
	SUBS r3, r0, r1, LSL #31

    swi 0x123456
.data
