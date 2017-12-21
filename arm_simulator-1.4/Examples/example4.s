.global main
.text
main:
<<<<<<< HEAD
	mov r0, #0
	mov r1, #1
	mov r2, r1
	add r1, r1
	bl 0x24
    swi 0x123456
=======
    mov r0, #0x12000000
    mov r1, #0x2000
ici:str r0, [r1]
    ldr r2, [r1]
    ldrb r0, [r1]
    ldrh r0, [r1]
    mov r0, r2
    mov r3, #0x5000
    add r2, r3
    bl ici
    swi 0x123456
.data
donnee:
    .word 0x11223344
    .word 0x55667788
fin:
>>>>>>> 7bdae88734079149f4d90e914a0fbc7f03b43291
