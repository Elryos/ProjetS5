.global main
.text
main:
    mov r0, #0x12000000
    mov r1, #0x2000
    str r0, [r1]
    ldr r2, [r1]
    ldrb r0, [r1]
    ldrh r0, [r1]

.data
limite:
    .word 0x12345678
