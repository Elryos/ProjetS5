.global main
.text

main : 
	mov r5, #7 	@r5 = 7
	mov r3, #3	@r3 = 3
	mov r4, r5      @r4 = 7
	mov r6, #6      @r6 = 6
	sub r5, r5, r3	@r5 = 4
	sub r3, r5, #2	@r3 = 2

	rsb r3, r5, #5	@r3 = 1

	mov r1, #1	@r1 = 1
	mov r3, #0	@r3 = 0

	and r5, r1, r3	@r5 = 0
	and r5, r3, r3  @r5 = 0
	and r5, r1, r1  @r5 = 1
	and r4, r4, r6  @r4 = 6

	mov r4, #7	@r4 = 7
	eor r5, r1, r3	@r5 = 1
	eor r5, r3, r3	@r5 = 0
	eor r5, r1, r1	@r5 = 0
	eor r4, r4, r6	@r4 = 1

	mov r4, #7
	orr r5, r1, r3	@r5 = 1
	orr r5, r3, r3	@r5 = 0
	orr r5, r1, r1	@r5 = 1
	orr r4, r4, r6  @r4 = 7

	bic r5, r1, r3	@r5 = 1
	bic r5, r3, r3	@r5 = 0
	bic r5, r1, r1	@r5 = 0
	bic r4, r4, r6  @r4 = 1

	mvn r5, r3 	@r5 = 0xFFFFFFFF
	mvn r5, r1	@r5 = 0xFFFFFFFE
	mvn r4, r6	@r4 = 0xFFFFFFF9

	lsl r1, r1, #4  @r1 = 16
	lsr r1, r1, #3  @r1 = 2
	mov r4, #-16
	asr r4, r4, #3  @r4 = -2

	swi 0x123456
.data