Beeq	sw	0	3	Stack
	lw	0	4	Stack
start	add	1	1	1
	nor	1	2	2
	beq	2	2	Queue
	beq	0	1	end
end	halt
A	.fill	3
Queue	.fill	start
