.global main
.text

@factoriel d un entier
	 
main :
	mov r5, #8
	
fact :
	
	sub r5,#1
	mov r1,r5
	add r5,#1
	
	mov r0,r5

boucle :	
	cmp r1,#0
	beq fintq
	bl mult
suit:
	sub r1,#1
	bl boucle
	
fintq :
	mov r7,r0
	swi 0x123456	
	
mult :
	mov r2,#0
	mov r3,r1	
	b condtq

tq :
	add r2,r0
	sub r3,#1
	
condtq :
	cmp   r3,#0
	bne   tq
	mov   r0,r2
	bl suit
