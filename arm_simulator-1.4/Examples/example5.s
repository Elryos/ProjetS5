.global main
.text

@syracuse sur un entier

main:

MOV r5,#15

tq:

	CMP r5,#1
	BEQ finsi

	ANDS r6,r5,#1

	BEQ alors
	
	ADD r5,r5,r5,LSL #1
	ADD r5,#1
	BAL tq
	
alors:	mov r5,r5,LSR #1
	BAL tq
	
finsi: swi 0x123456
