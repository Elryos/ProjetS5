.global main
.text

func:	
		LDR R0, =don
		MOV R1, #0
		MOV R5, #0
for:	CMP	R5,  #3
		BGT fin
		LDRB R4, [R0, R5]
		MOV R6, R5, LSL #3
		ADD R1, R1, R4, LSL R6
		ADD R5, #1
        B for
fin:	STR R1, [R0, #4]
    	LDMIA SP!, { R4, R5 }
        MOV PC, LR   

main:
		MOV R4, #4
		MOV R5, #5	
		MOV SP, #0x2900
		STMDB SP!, { R4, R5 } 
		BL func
		LDR R3, [R0, #4]
        swi 0x123456

.data
don:
    .word 0x12345678

