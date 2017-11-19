start	lw	0	6	one	//r6=1;
	sw	5	7	Stack	//save	return address on stack
	add	5	6	5	//increment stack pointer
	sw	5	1	Stack	//save n on stack
	add	5	6	5	//increment stack pointer
	sw	5	2	Stack	//save r on stack
	add	5	6	5	//increment stack pointer
	beq	2	0	return	// if (r==0), return 1
	beq	1	2	return	// if (n==r). return 1
	lw	0	6	negone	// r6=-1;
	add	6	1	1	// n=n-1;
	lw	0	4	Caddr	
	jalr	4	7		//first recursion here
	lw	0	6	negone	//r6=-1
	add	2	6	2	//r=r-1
	sw	5	3	Stack	//save previous value on stack
	lw	0	6	one	//r6=1;
	add	5	6	5	//increment stack pointer
	lw	0	4	Caddr
	jalr	4	7		//second recursion here
	lw	0	6	negone	//r6=-1
	add	5	6	5	//decrement stack pointer
	lw	5	4	Stack	//get previous fuction return value back
	add	4	3	3	//add two functions
	lw	0	6	negone	//r6=-1
	add	5	6	5	//decrement stack pointer
	lw	5	2	Stack	//get previous r back
	add	5	6	5	//decrement stack pointer
	lw	5	1	Stack	//get previous n back
	add	5	6	5	//decrement stack pointer
	lw	5	7	Stack	//get return address back
	jalr	7	4
return	lw	0	3	one	//r3=1;
	lw	0	6	negone	//r6=-1
	add	5	6	5	//decrement stack pointer
	lw	5	2	Stack	//get previous r back
	add	5	6	5	//decrement stack pointer
	lw	5	1	Stack	//get previous n back
	add	5	6	5	//decrement stack pointer
	lw	5	7	Stack	//get return address back
	jalr	7	4
Caddr	.fill	start
one	.fill	1
negone	.fill	-1	
