.global main
.text

main : 
	mov r5, #7
	add r5, r5, r5
	add r3, r5, #2
	swi 0x123456
.data
