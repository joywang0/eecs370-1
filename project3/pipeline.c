#include<studio.h>
#include<stdlib.h>
#define NUMMEMORY 65536 /* maximum number of data words in memory */
#define NUMREGS 8 /* number of machine registers */
#define ADD 0
#define NOR 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5 /* JALR will not implemented for Project 3 */
#define HALT 6
#define NOOP 7

#define NOOPINSTRUCTION 0x1c00000

typedef struct IFIDStruct {
	int instr;
	int pcPlus1;
} IFIDType;

typedef struct IDEXStruct {
	int instr;
	int pcPlus1;
	int readRegA;
	int readRegB;
	int offset;
} IDEXType;

typedef struct EXMEMStruct {
	int instr;
	int branchTarget;
	int aluResult;
	int readRegB;
} EXMEMType;

typedef struct MEMWBStruct {
	int instr;
	int writeData;
} MEMWBType;

typedef struct WBENDStruct {
	int instr;
	int writeData;
} WBENDType;

typedef struct stateStruct {
	int pc;
	int instrMem[NUMMEMORY];
	int dataMem[NUMMEMORY];
	int reg[NUMREGS];
	int numMemory;
	IFIDType IFID;
	IDEXType IDEX;
	EXMEMType EXMEM;
	MEMWBType MEMWB;
	WBENDType WBEND;
	int cycles; /* number of cycles run so far */
} stateType;

void run(stateType *state,stateType *newState);
void printState(stateType *statePtr);
int field0(int instruction);
int field1(int instruction);
int field2(int instruction);
int opcode(int instruction);
void printInstruction(int instr);
int convertNum(int num);

int main(int argc, char* argv[]){
    char line[MAXLINELENGTH];
    stateType state;
    stateType newState;
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
        if (sscanf(line, "%d", state.instrMem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
		state.dataMem[state.numMemory]=state.instrMem[state.numMemory];
		printf("memory[%d]=%d\n",state.numMemory,state.dataMem[state.numMemory]);
   	}

	printf("%d memory words\n",state.numMemory+1);
	printf("instruction memory:\n");
	
	int i;
	for(i=0;i<state.numMemory;++i){
		printf("instrMem[%d]: ",i);
		printInstruction(state.instrMem[i]);
	}
	state.IFID.instr=NOOPINSTRUCTION;
	state.IDEX.instr=NOOPINSTRUCTION;
	state.EXMEM.instr=NOOPINSTRUCTION;
	state.MEMWB.instr=NOOPINSTRUCTION;
	state.WBEND.instr=NOOPINSTRUCTION;
	state.pc=0;
	state.cycle=0;
	for(i=0;i<NUMREGS;i++){
		state.reg[i]=0;	
	}
	newstate=state;
	printf("\n");
	run(&state,&newstate);
	return 0;
}
void run(stateType *state,stateType *newState){
	while (1) {

		printState(state);

		/* check for halt */
		if (opcode(state->MEMWB.instr) == HALT) {
			printf("machine halted\n");
			printf("total of %d cycles executed\n", state->cycles);
			exit(0);
		}	

		newState = state;
		newState->cycles++;

	/* --------------------- IF stage --------------------- */
	newState->IFID.instr=state->instrMem[state->pc];
	newState->IFID.pcPlus1=state->pc+1;
	newState->pc=state->pc+1;
	/* --------------------- ID stage --------------------- */
	newState->IDEX.instr=state->IFID.instr;
	newState->IDEX.pcPlus1=state->IFID.pcPlus1;
	newState->IDEX.readRegA=state->reg[field0(state->IFID.instr)];
	newState->IDEX.readRegB=state->reg[field1(state->IFID.instr)];

	//sign extend
	newState->IDEX.offset=convertNum(field2(state->IFID.instr));

	//lw hazard
	if(opcode(state->IDEX.instr)==2){
		if((field1(state->IDEX.instr)==field0(state->IFID.instr)) || (field1(state->IDEX.instr))==filed1(state->IFID.instr)){
			newState->pc--;
			newState->IDEX.instr=NOOPINSTRUCTION;
			newState->IFID=state->IFID;
		}	
	}

	/* --------------------- EX stage --------------------- */
	newState->EXMEM.instr=state->IDEX.instr;
	newState->EXEM.branchTarget=state->IDEX.offset+state->IDEX.pcPlus1;

	int dest1,result1,dest2,result2,dest3,result3;
	int op=opcode(state->IDEX.instr);
	int EXMEMop=opcode(state->EXMEM.instr);
	int MEMWBop=opcode(state->MEMWB.instr);
	int WBENDop=opcode(state->WBEND.instr);
	int regA=field0(state->IDEX.instr);
	int regB=field1(state->IDEX.instr);
	int resultA;
	int resultB;
	if(EXMEMop==0 || EXMEMop==1){
		dest1=field2(state->EXMEM.instr);
		result1=state->EXMEM.aluResult;
	}

	if(MEMWBop==0 || MEMWBop==1){
		dest2=field2(state->MEMWB.instr);
		result2=state->EXMEM.writeData;
	}
	else if(MEMWBop==2){
		dest2=field1(state->MEMWB.instr);
		result2=state->EXMEM.writeData;
	}

	if(WBENDop==0 || WBENDop==1){
		dest3=field2(state->WBEND.instr);
		result3=state->WBEND.writeData;
	}
	else if(WBENDop==2){
		dest3=field1(state->WBEND.instr);
		result3=state->WBEND.writeData;
	}
	
	if(regA==dest1)
		resultA=result1;
	else if(regA==dest2)
		resultA=result2;
	else if(regA==dest3)
		resultA=result3;
	else
		resultA=state->IDEX.readRegA;
	
	if(regB==dest1)
		resultB=result1;
	else if(regB==dest2)
		resultB=result2;
	else if(regB==dest3)
		resultB=result3;
	else
		resultB=state->IDEX.readRegB;

//	newState.readRegA=resultA;
	newState->EXMEM.readRegB=resultB;

	if(op==0)
		newState->EXMEM.aluResult=resultA+resultB;
	else if(op==1)
		newState->EXMEM.aluResult=~(resultA | resultB);
	else if(op==2 || op==3)
		newState->EXMEM.aluResult=resultA+state->IDEX.offset;
	else if(op==4){
		if(resultA==resultB)
			newState->EXMEM.aluResult=1;
		else
			newState->EXMEM.aluResult=0;
	}
	/* --------------------- MEM stage --------------------- */
	op=opcode(state->EXMEM.instr);
	newState->EXMEM.instr=state->EXMEM.instr;
	if(op==0 || op==1)
		newState->MEMWB.writeData=state->EXMEM.aluResult;
	else if(op==2)
		newState->MEMWB.writeData=state->dataMem[state->EXMEM.aluResult];
	else if(op==3)
		newState->dataMem[state->EXMEM.aluResult]=state->EXMEM.readRegB;
	else if(op==4 && State->EXMEM.aluResult==1){
		newState->pc=state->EXMEM.branchTarget;
		newState->IFID.instr=NOOPINSTRUCTION;
		newState->IDEX.instr=NOOPINSTRUCTION;
		newState->EXMEM.instr=NOOPINSTRUCTION;
	}

	

	/* --------------------- WB stage --------------------- */
	op=opcode(state->MEMWB.instr);
	newState->WBEND.instr=state->MEMWB.instr;
	newState->WBEND.writeData=state->MEMWB.writeData;
	if(op==0 || op==1)
		newState->reg[field2(state->MEMWB.instr)]=state->MEMWB.writeData;	
	else if(op==2)
		newState->reg[field1(state->MEMWB.instr)]=state->MEMWB.writeData;
		state = newState; /* this is the last statement before end of the loop.
			It marks the end of the cycle and updates the
			current state with the values calculated in this
			cycle */
	}
}

int convertNum(int num){
	if(num&(1<<15))
		num-=(1<<16);
	return (num);	
}

void
printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate before cycle %d starts\n", statePtr->cycles);
    printf("\tpc %d\n", statePtr->pc);

    printf("\tdata memory:\n");
	for (i=0; i<statePtr->numMemory; i++) {
	    printf("\t\tdataMem[ %d ] %d\n", i, statePtr->dataMem[i]);
	}
    printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
	    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
    printf("\tIFID:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->IFID.instr);
	printf("\t\tpcPlus1 %d\n", statePtr->IFID.pcPlus1);
    printf("\tIDEX:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->IDEX.instr);
	printf("\t\tpcPlus1 %d\n", statePtr->IDEX.pcPlus1);
	printf("\t\treadRegA %d\n", statePtr->IDEX.readRegA);
	printf("\t\treadRegB %d\n", statePtr->IDEX.readRegB);
	printf("\t\toffset %d\n", statePtr->IDEX.offset);
    printf("\tEXMEM:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->EXMEM.instr);
	printf("\t\tbranchTarget %d\n", statePtr->EXMEM.branchTarget);
	printf("\t\taluResult %d\n", statePtr->EXMEM.aluResult);
	printf("\t\treadRegB %d\n", statePtr->EXMEM.readRegB);
    printf("\tMEMWB:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->MEMWB.instr);
	printf("\t\twriteData %d\n", statePtr->MEMWB.writeData);
    printf("\tWBEND:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->WBEND.instr);
	printf("\t\twriteData %d\n", statePtr->WBEND.writeData);
}

int
field0(int instruction)
{
	return( (instruction>>19) & 0x7);
}

int
field1(int instruction)
{
	return( (instruction>>16) & 0x7);
}

int
field2(int instruction)
{
	return(instruction & 0xFFFF);
}

int
opcode(int instruction)
{
	return(instruction>>22);
}

void
printInstruction(int instr)
{

	char opcodeString[10];

	if (opcode(instr) == ADD) {
		strcpy(opcodeString, "add");
	} else if (opcode(instr) == NOR) {
		strcpy(opcodeString, "nor");
	} else if (opcode(instr) == LW) {
		strcpy(opcodeString, "lw");
	} else if (opcode(instr) == SW) {
		strcpy(opcodeString, "sw");
	} else if (opcode(instr) == BEQ) {
		strcpy(opcodeString, "beq");
	} else if (opcode(instr) == JALR) {
		strcpy(opcodeString, "jalr");
	} else if (opcode(instr) == HALT) {
		strcpy(opcodeString, "halt");
	} else if (opcode(instr) == NOOP) {
		strcpy(opcodeString, "noop");
	} else {
		strcpy(opcodeString, "data");
    }
    printf("%s %d %d %d\n", opcodeString, field0(instr), field1(instr),
		field2(instr));
}

