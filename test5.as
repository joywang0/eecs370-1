	lw	0	1	input           r1 = memory[input]
        lw      0       4	SubAdr        prepare to call sub4n. r4=addr(sub4n)
	sw	0	4	Local
	beq	0	0	Socal
	nor	1	1	1
Socal	noop
Local	add	1	1	1
	jalr	4	7                     call sub4n; r7=return address; r3=answer
	halt                
input	.fill 10
Hell	.fill 2
	.fill 3
beef	.fill Hold
Hold	.fill 5
pork	.fill input
local	.fill Local   
