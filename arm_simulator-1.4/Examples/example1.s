.global main
.text

main : 
	ldr r0, =tab
	ldmia r0!, {r1-r8}
	ADDS r1, r2, r3, LSL r4
	ADDS r8, r7, r6, LSL r5
	stmia r0!, {r1-r8}
	swi 0x123456
	
.data
tab:
	.word 1
	.word 2
	.word 3
	.word 4
	.word 5
	.word 6
	.word 7
	.word 8
	