/* EECS 370 LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);

int
main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
            state.numMemory++) {

        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
		
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }
	state.pc=0;
	int time=0;
	int i;
	for(i=0;i<NUMREGS;i++){
		state.reg[i]=0;	
	}
	printf("\n");
	printState(&state);
//printf("%d",state.mem[state.pc]>>22);
	while(state.mem[state.pc]!=25165824){
		int a=(state.mem[state.pc]&(0x7<<19))>>19;
		int b=(state.mem[state.pc]&(0x7<<16))>>16;
		//printf("%d",b);
		int op=state.mem[state.pc]>>22;
		//printf("%d",op);
		if(op==0x0){
			int destReg=state.mem[state.pc]&(0x7);
			state.reg[destReg]=state.reg[a]+state.reg[b];
			state.pc++;
		}
		else if(op==0x1){
			int destReg=state.mem[state.pc]&(0x7);
			state.reg[destReg]=~(state.reg[a] | state.reg[b]);
			state.pc++;
		}
		else if(op==0x2){
			int offset=convertNum(state.mem[state.pc]&(0x0000ffff));	
			state.reg[b]=state.mem[offset+state.reg[a]];
			state.pc++;
		}
		else if(op==0x3){
			int offset=convertNum(state.mem[state.pc]&(0x0000ffff));
			state.mem[offset+state.reg[a]]=state.reg[b];
			state.pc++;
		}
		else if(op==0x4){
			int offset=convertNum(state.mem[state.pc]&(0x0000ffff));
			if(state.reg[a]==state.reg[b])
				state.pc=state.pc+1+offset;
			else
				state.pc++;
		}
		else if(op==0x5){
			state.reg[b]=state.pc+1;
			state.pc=state.reg[a];
		}
		else if(op==0x6){
			printState(&state);
			state.pc++;
		}

		else if(op==0x7){
			state.pc++;
		}
		printState(&state);
		time++;
	}
	state.pc++;
	printf("machine halted\n");
	printf("total of %d instructions executed\n",time+1);
	printf("final state of machine:\n");
	printState(&state);
    return(0);
}

int convertNum(int num){
	if(num &(1<<15))
		num-=(1<<16);
	return (num);
}

void
printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i=0; i<statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i=0; i<NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

