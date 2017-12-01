/* EECS 370 LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000
enum actionType
        {cacheToProcessor, processorToCache, memoryToCache, cacheToMemory,
         cacheToNowhere};
typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

typedef struct cacheStruct{
	int cache[256][256];
	int tag[256];
	int LRU[256];
	int dirty[256];
	int valid[256];
	int size;
}cacheType;

//void printState(stateType *);
void printAction(int address, int size, enum actionType type);

int convertNum(int num);

int blockoffset(int address,int blockSizeInWords){
	if(blockSizeInWords==1)
		return 0;
	else
		return address&(0xffffffff>>(32-(int)log2(blockSizeInWords)));
}

int index(int address,int blockSizeInWords,int numberOfSets){
	if(numberOfSets==1)
		return 0;
	else
		return (address&(0xffffffff>>(32-(int)log2(blockSizeInWords)-(int)log2(numberOfSets))))>>(int)log2(blockSizeInWords); 
}

int tag1(int address,int blockSizeInWords,int numberOfSets){
	return address>>((int)log2(blockSizeInWords)+(int)log2(numberOfSets));
}

int load(int addr,stateType *state,int blockSizeInWords,int numberOfSets,int blocksPerSet,cacheType *mycache){
	int blockOffset=blockoffset(addr,blockSizeInWords);
	int setindex=index(addr,blockSizeInWords,numberOfSets);
	int tag=tag1(addr,blockSizeInWords,numberOfSets);
	int low=blocksPerSet*(setindex);
	int up=blocksPerSet*(setindex+1);
	int memory=state->mem[addr];
	int hit=0;
	int i,index;
	for(i=low;i<up;++i){
		if(tag==mycache->tag[i] && mycache->valid[i]==1){
				hit=1;
				index=i;
				break;	
		}	
	}
	if(hit==1){
		for(int d=low;d<up;++d){
			if(mycache->LRU[d]<mycache->LRU[index]){
				if(mycache->LRU[d]!=blocksPerSet)
					mycache->LRU[d]++;
			}
		}
		mycache->LRU[index]=0;
		printAction(addr,1,cacheToProcessor);
		return memory;
	}
	else{
		int ifEmptyBlock=0;
		for(i=low;i<up;++i){
			if(mycache->valid[i]==0){
				mycache->valid[i]=1;
				index=i;
				ifEmptyBlock=1;
				break;
			}	
		}	
		if(ifEmptyBlock==1){
			for(i=low;i<up;++i){
				if(mycache->LRU[i]<mycache->LRU[index])	
					mycache->LRU[i]++;
			}
			mycache->LRU[index]=0;
			mycache->tag[index]=tag;
			int j=0;
			while((addr-j)%blockSizeInWords!=0)
				++j;
			for(i=0;i<blocksPerSet;++i)
				mycache->cache[index][i]=state->mem[addr-j+i];
			printAction(addr-j,blockSizeInWords,memoryToCache);
			printAction(addr,1,cacheToProcessor);
			return memory;
		}
		else{
			for(i=low;i<up;++i){
				if(mycache->LRU[i]==blocksPerSet-1)
					index=i;
			}
			if(mycache->dirty[index]==1){
				int first;
				first=((mycache->tag[index]<<(int)log2(numberOfSets))+index/blocksPerSet)<<(int)log2(blockSizeInWords);
				for(i=0;i<blockSizeInWords;++i)
					state->mem[first+i]=mycache->cache[index][i];
				mycache->dirty[index]=0;
				printAction(first,blockSizeInWords,cacheToMemory);
			}
			else{
				int first;
				first=((mycache->tag[index]<<(int)log2(numberOfSets))+index/blocksPerSet)<<(int)log2(blockSizeInWords);
				printAction(first,blockSizeInWords,cacheToNowhere);
			}
			for(i=low;i<up;++i){
				if(mycache->LRU[i]<mycache->LRU[index])
					mycache->LRU[i]++;
			}
			mycache->LRU[index]=0;
			mycache->tag[index]=tag;
			int j=0;
			while((addr-j)%blockSizeInWords!=0)
				++j;
			for(i=0;i<blocksPerSet;++i)
				mycache->cache[index][i]=state->mem[addr-j+i];
			printAction(addr-j,blockSizeInWords,memoryToCache);
			printAction(addr,1,cacheToProcessor);
			return memory;
		}
	}
}


void store(int addr,stateType *state,int blockSizeInWords,int numberOfSets,int blocksPerSet,cacheType *mycache,int memory){
	int blockOffset=blockoffset(addr,blockSizeInWords);
	int setindex=index(addr,blockSizeInWords,numberOfSets);
	int tag=tag1(addr,blockSizeInWords,numberOfSets);
	int low=blocksPerSet*(setindex);
	int up=blocksPerSet*(setindex+1);
//	int memory=state->reg[addr];
	int hit=0;
	int i,index;
	for(i=low;i<up;++i){
		if(tag==mycache->tag[i] && mycache->valid[i]==1){
				hit=1;
				index=i;
				break;	
		}	
	}
	if(hit==1){
		for(int d=low;d<up;++d){
			if(mycache->LRU[d]<mycache->LRU[index]){
				if(mycache->LRU[d]!=blocksPerSet)
					mycache->LRU[d]++;
			}
		}
		mycache->LRU[index]=0;
		mycache->dirty[index]=1;
		mycache->cache[index][blockOffset]=memory;
		printAction(addr,1,processorToCache);
	}
	else{
		int ifEmptyBlock=0;
		for(i=low;i<up;++i){
			if(mycache->valid[i]==0){
				mycache->valid[i]=1;
				index=i;
				ifEmptyBlock=1;
				break;
			}	
		}	
		if(ifEmptyBlock==1){
			mycache->dirty[index]=1;
			for(i=low;i<up;++i){
				if(mycache->LRU[i]<mycache->LRU[index])	
					mycache->LRU[i]++;
			}
			mycache->LRU[index]=0;
			mycache->tag[index]=tag;
			int j=0;
			while((addr-j)%blockSizeInWords!=0)
				++j;
			for(i=0;i<blocksPerSet;++i)
				mycache->cache[index][i]=state->mem[addr-j+i];
			mycache->cache[index][blockOffset]=memory;
			printAction(addr-j,blockSizeInWords,memoryToCache);
			printAction(addr,1,processorToCache);
		}
		else{
			for(i=low;i<up;++i){
				if(mycache->LRU[i]==blocksPerSet-1)
					index=i;
			}
			if(mycache->dirty[index]==1){
				int first;
				first=((mycache->tag[index]<<(int)log2(numberOfSets))+index/blocksPerSet)<<(int)log2(blockSizeInWords);
				for(i=0;i<blockSizeInWords;++i)
					state->mem[first+i]=mycache->cache[index][i];
				printAction(first,blockSizeInWords,cacheToMemory);	
				
			}
			else{
				int first;
				first=((mycache->tag[index]<<(int)log2(numberOfSets))+index/blocksPerSet)<<(int)log2(blockSizeInWords);
				printAction(first,blockSizeInWords,cacheToNowhere);	
			}
			for(i=low;i<up;++i){
				if(mycache->LRU[i]<mycache->LRU[index])
					mycache->LRU[i]++;
			}
			mycache->LRU[index]=0;
			mycache->tag[index]=tag;
			mycache->dirty[index]=1;
			int j=0;
			while((addr-j)%blockSizeInWords!=0)
				++j;
			for(i=0;i<blocksPerSet;++i)
				mycache->cache[index][i]=state->mem[addr-j+i];
			mycache->cache[index][blockOffset]=memory;
			printAction(addr-j,blockSizeInWords,memoryToCache);
			printAction(addr,1,processorToCache);
		}
		
	}
	
}

void printAction(int address, int size, enum actionType type)
{
    printf("@@@ transferring word [%d-%d] ", address, address + size - 1);
    if (type == cacheToProcessor) {
        printf("from the cache to the processor\n");
    } else if (type == processorToCache) {
        printf("from the processor to the cache\n");
    } else if (type == memoryToCache) {
        printf("from the memory to the cache\n");
    } else if (type == cacheToMemory) {
        printf("from the cache to the memory\n");
    } else if (type == cacheToNowhere) {
        printf("from the cache to nowhere\n");
    }
}

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc < 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }
	int blockSizeInWords=atoi(argv[2]);
	int numberOfSets=atoi(argv[3]); 
	int blocksPerSet=atoi(argv[4]);
	int i,j;
	cacheType mycache;
	mycache.size=blockSizeInWords*numberOfSets*blocksPerSet;
	for(i=0;i<numberOfSets*blocksPerSet;++i){
		mycache.LRU[i]=blocksPerSet-1;
		mycache.valid[i]=0;
		mycache.dirty[i]=0;
		mycache.tag[i]=-1;
		for(j=0;j<blockSizeInWords;++j){
			mycache.cache[i][j]=0;	
		}			
	}
    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
            state.numMemory++) {

        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
		
        //printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }
	state.pc=0;
	int time=0;
	for(i=0;i<NUMREGS;i++){
		state.reg[i]=0;	
	}
//	printf("\n");
//	printState(&state);
//printf("%d",state.mem[state.pc]>>22);
	while(1){
		int a=(state.mem[state.pc]&(0x7<<19))>>19;
		int b=(state.mem[state.pc]&(0x7<<16))>>16;
		//printf("%d",b);
		int op=state.mem[state.pc]>>22;
		int flag=load(state.pc,&state,blockSizeInWords,numberOfSets,blocksPerSet,&mycache);
		if(flag==25165824)
			break;
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
			state.reg[b]=load(state.reg[a]+offset,&state,blockSizeInWords,numberOfSets,blocksPerSet,&mycache);
			state.pc++;
		}
		else if(op==0x3){
			int offset=convertNum(state.mem[state.pc]&(0x0000ffff));
			store(offset+state.reg[a],&state,blockSizeInWords,numberOfSets,blocksPerSet,&mycache,state.reg[b]);
		//	state.mem[offset+state.reg[a]]=state.reg[b];
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
		//	printState(&state);
			state.pc++;
		}

		else if(op==0x7){
			state.pc++;
		}
	//	printState(&state);
		time++;
	}
	state.pc++;
//	printf("machine halted\n");
//	printf("total of %d instructions executed\n",time+1);
//	printf("final state of machine:\n");
//	printState(&state);
    return(0);
}

int convertNum(int num){
	if(num &(1<<15))
		num-=(1<<16);
	return (num);
}

/*void
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
}*/

