.global main
.text
main:
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
