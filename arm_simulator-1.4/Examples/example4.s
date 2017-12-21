.global main
.text
main:
	mov r0, #0
	mov r1, #1
	mov r2, r1
	add r1, r1
	bl 0x24
    swi 0x123456