.global main
.text
main:
	mov r1, #3
	mov r2, #8
	add r0, r2, r1, lsl r1
    swi 0x123456
