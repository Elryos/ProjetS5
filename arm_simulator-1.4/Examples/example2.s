.global main
.text

fin:
	swi 0x123456

test :  
	mov r4, #4
	bl fin
	mov r5, #5


main:
	mov r1, #1
	mov r2, #2
	b test
	mov r3, #3



