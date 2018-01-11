.global main
.text

main : 
	MOVS r1, #0xFFFFFFFF
	ADDS r1, r1, r1
	MOVS r2, #0xFFFFFFFF
	swi 0x123456
.data
