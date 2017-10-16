	lw	0	0	zero        
	lw      0       2       mcand   load reg2 with 32766
        lw      0       3       mplier  load reg3 with 10383
	lw      0       4       most    load reg4 with 0b1000000000000000(32768)
	nor     3       3       3       r3=~(r3);
	lw      0       5       i       load reg5, r5=i;i=1;
	lw      0       6       zero    r6=0;
loop	lw 	0	1	zero	r1=0;
loop    nor     5       5       5       i=~(i);
        nor     3       5       6   	r6=r3 & r5;(10383 & i)
        beq	0	6	else	if(r6==0) goto else
if	add	2	1	1	if (r6!=0) r1+=mcand;
else	nor	5	5	5	i=i;
	add	5	5	5	i<<1;
	add	2	2	2	mcand<<1;
	beq	4	5	done	if(most==i),we are done.
	beq	0	0	loop
done    halt                            end of program
mcand   .fill   32766
mplier  .fill   10383
most    .fill   32768                   
i       .fill   1
zero    .fill   0
