.global main
.text

main:


	movs r0, #0
	movs r1, #0xFFFFFFFF
	movs r0, #0x80000000
	movs r2, r1
	adds r3, r1, r2
	adds r4, r0, r0

	swi 0x123456
