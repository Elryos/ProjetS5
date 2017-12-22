.global main
.text
main:
	mov r1, #1
	mov r2, #2
test :
	mov r3, #3
	mov r4, #4
	mov r5, #5
	b test
    swi 0x123456
