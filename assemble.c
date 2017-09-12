/* Assembler code fragment for LC-2K */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000

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
	int bnum,offsetnum;
	char* labelindex[1000]={0};
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
    while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)!=0) {
        /* reached end of file */
		if(strcmp(label,"") && (label[0]=='0' || label[0]=='1' || label[0]=='2' || label[0]=='3' || label[0]=='4' || label[0]=='5' || label[0]=='6' || label[0]=='7' || label[0]=='8' || label[0]=='9' || strlen(label)>6)){
			printf("error: label not valid");
			exit(1);		
		}
		labelindex[i]=label;
	//	printf("%s\n",labelindex[i]);
		i++;
    }

	
    /* this is how to rewind the file ptr so that you start reading from the
        beginning of the file */
    rewind(inFilePtr);

    /* after doing a readAndParse, you may want to do the following to test the
        opcode */
		int PC=0;
	while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)!=0){ 
			if (!strcmp(opcode, "add")) {
   		/* do whatever you need to do for opcode "add" */
			bnum=(atoi(arg0)<<19)+(atoi(arg1)<<16)+atoi(arg2);
			//num=convertNum(bnum);
			//printf("%d\n",bnum);
			fprintf(outFilePtr,"%d\n",bnum);
		}
		else if(!strcmp(opcode,"nor")){
			bnum=(1<<22)+(atoi(arg0)<<19)+(atoi(arg1)<<16)+atoi(arg2);
			fprintf(outFilePtr,"%d\n",bnum);
		}
		else if(!strcmp(opcode,"lw")){
			if(isNumber(arg2)==1){
				offsetnum=convertNum(atoi(arg2));
				//printf("%d",offsetnum);
				bnum=(1<<23)+(atoi(arg0)<<19)+(atoi(arg1)<<16)+offsetnum;
				fprintf(outFilePtr,"%d\n",bnum);
			}
			else{
				for(i=0;i<1000;i++){
					if(!strcmp(labelindex[i],label)){
						offsetnum=convertNum(i);
						break;
					}		
				}
				bnum=(1<<23)+(atoi(arg0)<<19)+(atoi(arg1)<<16)+offsetnum;
				fprintf(outFilePtr,"%d\n",bnum);	
			}		
		}
		else if(!strcmp(opcode,"sw")){
			if(isNumber(arg2)==1){
				offsetnum=convertNum(atoi(arg2));
				//printf("%d",offsetnum);
				bnum=(3<<22)+(atoi(arg0)<<19)+(atoi(arg1)<<16)+offsetnum;
				fprintf(outFilePtr,"%d\n",bnum);
			}
			else{
				for(i=0;i<1000;i++){
					if(!strcmp(labelindex[i],label)){
						offsetnum=convertNum(i);
						break;
					}		
				}
				bnum=(3<<22)+(atoi(arg0)<<19)+(atoi(arg1)<<16)+offsetnum;
				fprintf(outFilePtr,"%d\n",bnum);	
			}
		}
		else if(!strcmp(opcode,"beq")){
				if(isNumber(arg2)==1){
						offsetnum=convertNum(atoi(arg2));
						bnum=(4<<22)+(atoi(arg0)<<19)+(atoi(arg1)<<16)+offsetnum;
						fprintf(outFilePtr,"%d\n",bnum);
				}
				else{
						for(i=0;i<1000;i++){
							if(!strcmp(labelindex[i],label)){
									offsetnum=convertNum(i-1-PC);
									break;
							}
						}
							bnum=(4<<22)+(atoi(arg0)<<19)+(atoi(arg1)<<16)+offsetnum;
							fprintf(outFilePtr,"%d\n",bnum);
				}
		}
		PC++;	
	}
	

    
		
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

