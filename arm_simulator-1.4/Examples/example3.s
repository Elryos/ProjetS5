.global main
.text

main : 

    MOVS r1, #0xFFFFFFFF
    MOVS r2, #1
    MOV R3, r2, LSL #31
    SUBS r4, r1, r3
    

    swi 0x123456

.data
