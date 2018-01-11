.global main
.text

main : 
	movs r1, #0xFFFFFFFF
	adds r2, r1, #1
	swi 0x123456
.data
