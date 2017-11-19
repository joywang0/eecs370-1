/* Assembler code fragment for LC-2K */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//123
#define MAXLINELENGTH 1000

struct symbol{
	char* name;
	char type;
	int line;
}symbol;
struct relocation{
	char* opcode;
	char* name;
	int line;
}relocation;
int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int
    convertNum(int num)
    {
        /* convert a 16-bit number into a 32-bit Linux integer */
        if (num & (1<<15) ) {
            num -= (1<<16);
        }
        return(num);
    }
int
main(int argc, char *argv[])
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
	int offsetnum;
	int bnum[1000];
	char* labelindex[1000]={0};
	int j;
	for(j=0;j<1000;j++){
		labelindex[j]=(char*)malloc(1000*sizeof(char));
		strcpy(labelindex[j],"");
	}
    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    /* here is an example for how to use readAndParse to read a line from
        inFilePtr */
	int i=0;
	int data[1000];
	int datanum=0;
//	printf("1");
	struct symbol symbolTable[1000];
	struct relocation relocationTable[1000];
	for(j=0;j<1000;++j){
		//printf("1");
		symbolTable[j].name=(char*)malloc(1000*sizeof(char));

		symbolTable[j].type='U';
		symbolTable[j].line=0;
		relocationTable[j].name=(char*)malloc(1000*sizeof(char));

		relocationTable[j].opcode=(char*)malloc(1000*sizeof(char));

		relocationTable[j].line=0;
	}
	int symbolnum=0;
	int linenum1=0;
	int linenum2=0;
    while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)!=0) {
        /* reached end of file */
		if(strcmp(label,"") && (label[0]=='0' || label[0]=='1' || label[0]=='2' || label[0]=='3' || label[0]=='4' || label[0]=='5' || label[0]=='6' || label[0]=='7' || label[0]=='8' || label[0]=='9' || strlen(label)>6)){
			printf("error: label not valid\n");
			exit(1);		
		}
		strcpy(labelindex[i],label);
		if(label[0]>='A' && label[0]<='Z'){
			if(!strcmp(opcode,".fill")){
				//printf("1");
				strcpy(symbolTable[symbolnum].name,label);
				symbolTable[symbolnum].line=linenum2;
				symbolTable[symbolnum].type='D';
				//printf("%s %s %d",label,symbolTable[symbolnum].name,symbolnum);
				symbolnum++;	
			}
			else{
				strcpy(symbolTable[symbolnum].name,label);
				symbolTable[symbolnum].line=linenum1;
				symbolTable[symbolnum].type='T';
				symbolnum++;
			}
		}
		if(!strcmp(opcode,".fill"))
			linenum2++;
		else
			linenum1++;
//		printf("%s\n",labelindex[0]);
		
		i++;
    }
//	printf("%s\n",labelindex[2]);
	int m,n;
	for(m=0;m<1000;m++){
		for(n=m+1;n<1000;n++){
			if(!strcmp(labelindex[m],labelindex[n]) && strcmp(labelindex[m],"") ){
				printf("error: duplicate labels\n");
				exit(1);
			}	
		}
	}
    /* this is how to rewind the file ptr so that you start reading from the
        beginning of the file */
    rewind(inFilePtr);

    /* after doing a readAndParse, you may want to do the following to test the
        opcode */
		int PC=0;
		int flag;
		int textnum=0;
	//	int j;
		int ifsymbol;
		int rnum=0;
	while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)!=0){ 
		flag=0;
		ifsymbol=0;
		if (!strcmp(opcode, "add")) {
   		/* do whatever you need to do for opcode "add" */
			bnum[textnum]=(atoi(arg0)<<19)+(atoi(arg1)<<16)+atoi(arg2);
			//num=convertNum(bnum);
			//printf("%d\n",bnum);
		//	fprintf(outFilePtr,"%d\n",bnum);
			textnum++;
		}
		else if(!strcmp(opcode,"nor")){
			bnum[textnum]=(1<<22)+(atoi(arg0)<<19)+(atoi(arg1)<<16)+atoi(arg2);
			textnum++;
		//	fprintf(outFilePtr,"%d\n",bnum);
		}
		else if(!strcmp(opcode,"lw")){
			if(isNumber(arg2)==1){
				offsetnum=atoi(arg2);
				if(offsetnum>=((1<<15)) || offsetnum<(-(1<<15))){
					printf("error: offset number do not fit in 16 bits\n");
					exit(1);
				}
				if(offsetnum>=0)
					bnum[textnum]=(1<<23)+(atoi(arg0)<<19)+(atoi(arg1)<<16)+offsetnum;
				else
					bnum[textnum]=(1<<23)+(atoi(arg0)<<19)+(atoi(arg1)<<16)+(unsigned int)offsetnum-(65535<<16);
				textnum++;
				//fprintf(outFilePtr,"%d\n",bnum);
			}
			else{
				relocationTable[rnum].opcode="lw";
				strcpy(relocationTable[rnum].name,arg2);
				relocationTable[rnum].line=textnum;
				rnum++;

				for(i=0;i<1000;i++){
					if(!strcmp(labelindex[i],arg2)){
						//printf("%d",isNumber("SubAdr"));
					
						offsetnum=i;
						flag=1;
						break;
					}
				}
				if(flag!=1){
					if(arg2[0]>='A' && arg2[0]<='Z'){
						for(j=0;j<1000;++j){
							if(!strcmp(symbolTable[j].name,arg2)){
								ifsymbol=1;
								break;
							}
						}
							if(ifsymbol==0){
								strcpy(symbolTable[symbolnum].name,arg2);
								symbolTable[symbolnum].type='U';
								symbolTable[symbolnum].line=0;
								symbolnum++;
							}
						offsetnum=0;
					}
					else{
						printf("error: undefined local labels\n");
						exit(1);
					}
				}
				if(offsetnum>=((1<<15)) || offsetnum<(-(1<<15))){
					printf("error: offset number do not fit in 16 bits\n");
					exit(1);
				}

				bnum[textnum]=(1<<23)+(atoi(arg0)<<19)+(atoi(arg1)<<16)+offsetnum;
				textnum++;
			//	fprintf(outFilePtr,"%d\n",bnum);	
			}		
		}
		else if(!strcmp(opcode,"sw")){
			if(isNumber(arg2)==1){
				offsetnum=atoi(arg2);
				//printf("%d",offsetnum);
				if(offsetnum>=((1<<15)) || offsetnum<(-(1<<15))){
					printf("error: offset number do not fit in 16 bits\n");
					exit(1);
				}
				if(offsetnum>=0)
					bnum[textnum]=(1<<22)+(1<<23)+(atoi(arg0)<<19)+(atoi(arg1)<<16)+offsetnum;
				else
					bnum[textnum]=(3<<22)+(atoi(arg0)<<19)+(atoi(arg1)<<16)+(unsigned int)offsetnum-(65535<<16);
				textnum++;
			//	fprintf(outFilePtr,"%d\n",bnum);
			}
			else{
				relocationTable[rnum].opcode="sw";
				strcpy(relocationTable[rnum].name,arg2);
				relocationTable[rnum].line=textnum;
				rnum++;
				for(i=0;i<1000;i++){
					if(!strcmp(labelindex[i],arg2)){
						offsetnum=i;
						flag=1;
						break;
					}
				}
				if(flag!=1){

					if(arg2[0]>='A' && arg2[0]<='Z'){
						offsetnum=0;
						for(j=0;j<1000;++j){
							if(!strcmp(symbolTable[j].name,arg2)){
								ifsymbol=1;
								break;
							}
						}
							if(ifsymbol==0){
								//printf("%s\n",symbolTable[0].name);
								strcpy(symbolTable[symbolnum].name,arg2);
								symbolTable[symbolnum].type='U';
								symbolTable[symbolnum].line=0;
								symbolnum++;
							}
					}
					else{
						printf("error: undefined labels\n");
						exit(1);
					}
				}

				if(offsetnum>=((1<<15)) || offsetnum<(-(1<<15))){
					printf("error: offset number do not fit in 16 bits\n");
					exit(1);
				}

				bnum[textnum]=(3<<22)+(atoi(arg0)<<19)+(atoi(arg1)<<16)+offsetnum;
				//fprintf(outFilePtr,"%d\n",bnum);
				textnum++;
			}
		}
		else if(!strcmp(opcode,"beq")){
				if(isNumber(arg2)==1){
					offsetnum=atoi(arg2);
					if(offsetnum>=((1<<15)) || offsetnum<(-(1<<15))){
						printf("error: offset number do not fit in 16 bits\n");
						exit(1);
					}
					if(offsetnum>=0)
						bnum[textnum]=(4<<22)+(atoi(arg0)<<19)+(atoi(arg1)<<16)+offsetnum;
					else
						bnum[textnum]=(4<<22)+(atoi(arg0)<<19)+(atoi(arg1)<<16)+(unsigned int)offsetnum-(65535<<16);	
					textnum++;
			//	fprintf(outFilePtr,"%d\n",bnum);
				}
				else{
				//	relocationTable[rnum].opcode="beq";
				//	relocationTable[rnum].name=arg2;
				//	relocationTable[rnum].line=textnum;
				//	rnum++;
						//printf("%s\n",labelindex[2]);
						for(i=0;i<1000;i++){
							if(!strcmp(labelindex[i],arg2)){
									offsetnum=(i-1-PC);
									flag=1;
									break;
							}
						}
						if(flag!=1){
							printf("error: undefined labels\n");
							exit(1);
						}	

						if(offsetnum>=(1<<15) || offsetnum<(-(1<<15))){
							printf("error: offset number do not fit in 16 bits\n");								exit(1);
						}
		
						if(offsetnum>=0)
							bnum[textnum]=(4<<22)+(atoi(arg0)<<19)+(atoi(arg1)<<16)+offsetnum;
						else
							bnum[textnum]=(4<<22)+(atoi(arg0)<<19)+(atoi(arg1)<<16)+(unsigned int)offsetnum-(65535<<16);
						textnum++;
					//	fprintf(outFilePtr,"%d\n",bnum);
				}
		}
		else if(!strcmp(opcode,"jalr")){
			bnum[textnum]=(5<<22)+(atoi(arg0)<<19)+(atoi(arg1)<<16);
			textnum++;
		//	fprintf(outFilePtr,"%d\n",bnum);
		}
		else if(!strcmp(opcode,"halt")){
			bnum[textnum]=(6<<22);
			textnum++;
		//	fprintf(outFilePtr,"%d\n",bnum);
		}
		else if(!strcmp(opcode,"noop")){
			bnum[textnum]=(7<<22);
			textnum++;
		//	fprintf(outFilePtr,"%d\n",bnum);
		}
		else if(!strcmp(opcode,".fill")){
			if(isNumber(arg0)==1){
				data[datanum]=atoi(arg0);
				datanum++;
			//	fprintf(outFilePtr,"%d\n",bnum);
			}
			else{
				relocationTable[rnum].opcode=".fill";
				strcpy(relocationTable[rnum].name,arg0);
				relocationTable[rnum].line=datanum;
				rnum++;
				for(i=0;i<1000;i++){
					if(!strcmp(labelindex[i],arg0)){	
						data[datanum]=i;
						datanum++;
						flag=1;
						break;
					}
				}
				if(flag==0){
					if(arg0[0]>='A' && arg0[0]<='Z'){
						data[datanum]=0;
						datanum++;
						for(j=0;j<1000;++j){
							if(!strcmp(symbolTable[j].name,arg0)){
								ifsymbol=1;
								break;
							}
						}
						if(ifsymbol==0){
							strcpy(symbolTable[symbolnum].name,arg0);
							symbolTable[symbolnum].type='U';
							symbolTable[symbolnum].line=0;
							symbolnum++;
						}
					}
					else{
						printf("Undefined local labels\n");
						exit(1);
					}
				}
			//	fprintf(outFilePtr,"%d\n",bnum);
			}
		}
		else{
			printf("error: unrecognized opcodes.\n");
			exit(1);
		}
	//	datanum++;
	//	textnum++;
	//	symbolnum++;
		PC++;	
	}
	fprintf(outFilePtr,"%d %d %d %d\n",textnum,datanum,symbolnum,rnum);
//	if(linenum1!=textnum || linenum2!=textnum)
//		printf("wrong");
	for(i=0;i<textnum;++i)
		fprintf(outFilePtr,"%d\n",bnum[i]);
	for(i=0;i<datanum;++i)
		fprintf(outFilePtr,"%d\n",data[i]);
    for(i=0;i<symbolnum;++i)
		fprintf(outFilePtr,"%s %c %d\n",symbolTable[i].name,symbolTable[i].type,symbolTable[i].line);
	for(i=0;i<rnum;++i)
		fprintf(outFilePtr,"%d %s %s\n",relocationTable[i].line,relocationTable[i].opcode,relocationTable[i].name);
    return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int
readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
        char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
        /* reached end of file */
        return(0);
    }

    /* check for line too long (by looking for a \n) */
    if (strchr(line, '\n') == NULL) {
        /* line too long */
        printf("error: line too long\n");
        exit(1);
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n\r ]", label)) {
        /* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
        opcode, arg0, arg1, arg2);
    return(1);
}

int
isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}

