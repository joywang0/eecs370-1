#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include<stdbool.h>
#define MAXSIZE 300
#define MAXLINELENGTH 1000
#define MAXFILES 6

typedef struct FileData FileData;
typedef struct SymbolTableEntry SymbolTableEntry;
typedef struct RelocationTableEntry RelocationTableEntry;
typedef struct CombinedFiles CombinedFiles;

struct SymbolTableEntry {
	char label[7];
	char location;
	int offset;
	bool defined;
};

struct RelocationTableEntry {
	int offset;
	char inst[7];
	char label[7];
	int file;
};

struct FileData {
	int textSize;
	int dataSize;
	int symbolTableSize;
	int relocationTableSize;
	int textStartingLine; // in final executible
	int dataStartingLine; // in final executible
	int text[MAXSIZE];
	int data[MAXSIZE];
	SymbolTableEntry symbolTable[MAXSIZE];
	RelocationTableEntry relocTable[MAXSIZE];
};

struct CombinedFiles {
	int text[MAXSIZE];
	int data[MAXSIZE];
	SymbolTableEntry     symTable[MAXSIZE];
	RelocationTableEntry relocTable[MAXSIZE];
	int textSize;
	int dataSize;
	int symTableSize;
	int relocTableSize;
};

int convertNum(int num){
	if(num &(1<<15))
		num-=(1<<16);
	return num;
}

int main(int argc, char *argv[])
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr; 
	int i, j;

	if (argc <= 2) {
		printf("error: usage: %s <obj file> ... <output-exe-file>\n",
				argv[0]);
		exit(1);
	}

	outFileString = argv[argc - 1];

	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	FileData files[MAXFILES];

	//Reads in all files and combines into master
	for (i = 0; i < argc - 2; i++) {
		inFileString = argv[i+1];

		inFilePtr = fopen(inFileString, "r");
		printf("opening %s\n", inFileString);

		if (inFilePtr == NULL) {
			printf("error in opening %s\n", inFileString);
			exit(1);
		}

		char line[MAXLINELENGTH];
		int sizeText, sizeData, sizeSymbol, sizeReloc;

		// parse first line
		fgets(line, MAXSIZE, inFilePtr);
		sscanf(line, "%d %d %d %d",
				&sizeText, &sizeData, &sizeSymbol, &sizeReloc);

		files[i].textSize = sizeText;
		files[i].dataSize = sizeData;
		files[i].symbolTableSize = sizeSymbol;
		files[i].relocationTableSize = sizeReloc;

		// read in text
		int instr;
		for (j = 0; j < sizeText; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			instr = atoi(line);
			files[i].text[j] = instr;
		}

		// read in data
		int data;
		for (j = 0; j < sizeData; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			data = atoi(line);
			files[i].data[j] = data;
		}

		// read in the symbol table
		char label[7];
		char type;
		int addr;
		for (j = 0; j < sizeSymbol; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%s %c %d",
					label, &type, &addr);
			files[i].symbolTable[j].offset = addr;
			strcpy(files[i].symbolTable[j].label, label);
			files[i].symbolTable[j].location = type;
			files[i].symbolTable[j].defined=false;
		}

		// read in relocation table
		char opcode[7];
		for (j = 0; j < sizeReloc; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%d %s %s",
					&addr, opcode, label);
			files[i].relocTable[j].offset = addr;
			strcpy(files[i].relocTable[j].inst, opcode);
			strcpy(files[i].relocTable[j].label, label);
			files[i].relocTable[j].file	= i;
		}
		fclose(inFilePtr);
	} // end reading files

	// *** INSERT YOUR CODE BELOW ***
	//    Begin the linking process
	//    Happy coding!!!

	for(i=0;i<argc-2;++i){
		for(j=0;j<files[i].symbolTableSize;++j){
			if(files[i].symbolTable[j].location!='U'){
				if(!strcmp(files[i].symbolTable[j].label,"Stack")){
					printf("Stack defined\n");
					exit(1);
				}
				else{
					if(files[i].symbolTable[j].defined==false)
						files[i].symbolTable[j].defined=true;
				}
			}
		}		
	}
	int jk,mn;
	int ok=0;
	int flag[1000];
	int flag1[1000];
	int ok1=0;
	for(i=0;i<argc-2;++i){
		for(j=0;j<files[i].symbolTableSize;++j){
			if(files[i].symbolTable[j].location=='U'){
			//	printf("1");
				if(strcmp(files[i].symbolTable[j].label,"Stack")){
				//	printf("1");
					for(jk=0;jk<argc-2;jk++){
						for(mn=0;mn<files[jk].symbolTableSize;mn++){
							if(!strcmp(files[jk].symbolTable[mn].label,files[i].symbolTable[j].label)){
								if(jk!=i || mn!=j){
									//printf("%s",files[jk].symbolTable[mn].label);
									flag[ok]=1;
								//	printf("%d",flag[ok]);
								}
							}
						}	
					}
					ok++;
				}			
			}	
		}			
	}
//	printf("%d",ok);
	for(i=0;i<ok;i++){
		if(flag[i]!=1){
			printf("Undefined label,%d\n",flag[i]);
			exit(1);
		}
	}
	for(i=0;i<argc-2;++i){
		for(j=0;j<files[i].symbolTableSize;++j){
			if(files[i].symbolTable[j].location!='U'){
				for(jk=0;jk<argc-2;jk++){
					for(mn=0;mn<files[jk].symbolTableSize;mn++){
						if(!strcmp(files[jk].symbolTable[mn].label,files[i].symbolTable[j].label)){
							if((jk!=i || mn!=j) && files[jk].symbolTable[mn].location!='U'){
								flag1[ok1]=1;		
								//printf("%d\n",jk);
							}
						}
					}	
				}
				ok1++;
			}	
		}			
	}
	for(i=0;i<ok1;i++){
		if(flag1[i]==1){
			printf("duplicated\n");
			exit(1);
		}
	}



	CombinedFiles all;
	all.textSize=0;
	all.dataSize=0;
	all.symTableSize=0;
	all.relocTableSize=0;
//	int i,j;
	for(i=1;i<argc-1;++i){
		all.textSize+=files[i-1].textSize;	
		all.dataSize+=files[i-1].dataSize;
	//	all.symTableSize+=files[i-1].symbolTableSize;
	//	all.relocTableSize+=files[i-1].relocationTableSize;
	}

	int k;
	int m=0;
	int n=0;
//	m=0;
//	n=0;
	
	for(i=0;i<argc-2;++i){
		for(j=0;j<files[i].symbolTableSize;++j){
			if(files[i].symbolTable[j].location!='U'){
				strcpy(all.symTable[m].label,files[i].symbolTable[j].label);
				all.symTable[m].offset=0;
				if(files[i].symbolTable[j].location=='T'){
					all.symTable[m].location='T';
					for(k=0;k<i;++k)
						all.symTable[m].offset+=files[k].textSize;
					all.symTable[m].offset+=files[i].symbolTable[j].offset;
				}
				if(files[i].symbolTable[j].location=='D'){
					all.symTable[m].location='D';
					for(k=0;k<i;++k)
						all.symTable[m].offset+=files[k].dataSize;
					for(k=0;k<argc-2;++k)
						all.symTable[m].offset+=files[k].textSize;
					all.symTable[m].offset+=files[i].symbolTable[j].offset;
				}
				m++;
			}	
		}			
	}

	all.symTableSize=m;
	
//	printf("%c",all.symTable[0].location);
	for(i=0;i<argc-2;++i){
		for(j=0;j<files[i].relocationTableSize;++j){
			m=0;
			if(files[i].relocTable[j].label[0]>='A' && files[i].relocTable[j].label[0]<='Z'){
				if(strcmp(files[i].relocTable[j].label,"Stack")){
					for(k=0;k<all.symTableSize;++k){
						if(!strcmp(files[i].relocTable[j].label,all.symTable[k].label)){
							if(strcmp(files[i].relocTable[j].inst,".fill"))
								files[i].text[files[i].relocTable[j].offset]=(0xffff0000 & files[i].text[files[i].relocTable[j].offset]) | all.symTable[k].offset;
							else
								files[i].data[files[i].relocTable[j].offset]=all.symTable[k].offset;
						}	
					}
				//	printf("%d",files[1].data[0]);
				}
				else{
					if(strcmp(files[i].relocTable[j].inst,".fill"))
						files[i].text[files[i].relocTable[j].offset]=(0xffff0000 & files[i].text[files[i].relocTable[j].offset]) | (all.textSize+all.dataSize);	
					else
						files[i].data[files[i].relocTable[j].offset]=(0xffff0000 & files[i].data[files[i].relocTable[j].offset]) | (all.textSize+all.dataSize);	
				//	printf("%d",files[0].data[0]);
				}
			}
			else{
				m=0;
				if(strcmp(files[i].relocTable[j].inst,".fill")){
					int address=0x0000ffff & files[i].text[files[i].relocTable[j].offset];
					if(address<files[i].textSize){
						for(k=0;k<i;++k)
							m+=files[k].textSize;
						m+=address;
					}
					else{
						for(k=0;k<argc-2;++k)
							m+=files[k].textSize;
						for(k=0;k<i;++k)
							m+=files[k].dataSize;
						m+=address-files[i].textSize;
					}
					files[i].text[files[i].relocTable[j].offset]=(0xffff0000 & files[i].text[files[i].relocTable[j].offset]) | m;
				//	printf("%d",m);
				}
				else{
					int address=files[i].data[files[i].relocTable[j].offset];
					if(address<files[i].textSize){
						for(k=0;k<i;++k)
							m+=files[k].textSize;
						m+=address;
					}
					else{
						for(k=0;k<argc-2;++k)
							m+=files[k].textSize;
						for(k=0;k<i;++k)
							m+=files[k].dataSize;
						m+=files[i].relocTable[j].offset;
					}
					files[i].data[files[i].relocTable[j].offset]=m;
				}
			}
		//	printf("%d",m);
			all.relocTableSize++;
		}	
	}
//	printf("%d",files[0].data[0]);
	m=0;
	n=0;
	for(i=0;i<argc-2;++i){
		for(j=0;j<files[i].textSize;++j){	
			all.text[m]=files[i].text[j];
			m++;
		}
	}
	for(i=0;i<argc-2;++i){
		for(k=0;k<files[i].dataSize;++k){
			all.data[n]=files[i].data[k];
			n++;
		}
	}
	
	for(i=0;i<m;++i)
		fprintf(outFilePtr,"%d\n",all.text[i]);
	for(j=0;j<n;++j)
		fprintf(outFilePtr,"%d\n",all.data[j]);
//	printf("12");
} // end main
