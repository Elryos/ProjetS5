.global main
.text

main : 
	add r5, #2
	add r3, r5, r5
	add r5, #3
	add r3, r5, r3
	swi 0x123456
.data
