.global main
.text
main:
    mov r0, #0xFF000
    mov r4, #1
    ldr r1, =donnee
    ldr r2, =fin
    add r4, #0
    ldr r5, [r1, +r4, lsl #2]
    ldmia r1!, {r6, r7}
    swi 0x123456

    MOVS r1, #0xFFFFFFFF
    MOVS r2, #1
    MOV R3, r2, LSL #31
    SUBS r4, r1, r3
    swi 0x123456

.data
donnee:
    .word 0x11223344
    .word 0x55667788
fin:
