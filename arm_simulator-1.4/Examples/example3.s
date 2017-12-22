.global main
.text
main:
    mov r0, #0x12
    mov r1, #0x2000
    str r0, [r1]
    add r0, #0x1200
    str r0, [r1, #4]
    add r0, #0x120000
    str r0, [r1, #8]
    add r0, #0x12000000
    str r0, [r1, #12]
    add r7, r1, #12
    ldmia r1, {r2-r4}
    ldmib r1, {r0,r5,r6}
    ldmda r7, {r2-r4}
    ldmdb r7, {r0,r5,r6}
    swi 0x123456

.data
limite:
    .word 0x12345678
