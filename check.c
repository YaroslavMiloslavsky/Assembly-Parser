#include "global.h"
#include "extern.h"

/*this part of the program checks the data types of command lines and label declarations and writes the word with the correct type to the memory*/

/*A,R,E the last 2 bites of the word , A-absolute R-relocatable E-extern*/
#define A 0
#define E 1
#define R 2

/*stores the commands and theis data*/
struct cmdStruct { /* stores commands such as mov, cmp, inc etc. */
	char name[5]; /* 4 chars + '0' */
	int code; /* code between 0 and 15 */
	int oper; /* how many operands for this command (0-2) */
};
/*the table of the data itself*/
struct cmdStruct cmds[] = {
		{"mov", 0,   2},
		{"cmp", 1,   2},
		{"add", 2,   2},
		{"sub", 3,   2},
		{"not", 4,   1},
		{"clr", 5,   1},
		{"lea", 6,   2},
		{"inc", 7,   1},
		{"dec", 8,   1},
		{"jmp", 9,   1},
		{"bne", 10,  1},
		{"red", 11,  1},
		{"prn", 12,  1},
		{"jsr", 13,  1},
		{"rts", 14,  0},
		{"stop", 15, 0}
};


/*checks if the given argument is an opecode*/
/*return -1 if not an opecode*/
int is_cmd(char name[]){
	int i;
	for (i=0; i<16; i++){
		if((strcmp((cmds[i].name),name))==0)
			return i;
	}
	return -1;
}


/*this function checks the labels and allocates the list for each new data added with the correct labels 
if any errors detected the error flag will be raised and the function will print the error and abort*/
void check_label(char line[],char label[],int lenLine, int *pos){
	int i, j=0, ch=0;
	char token[MAX_STR_LEN];
	if(label[0]!='\0'){/*checks if the label exist*/
		if(!DC)
				lab=(ptr)malloc(sizeof(item));/*allocate if no data before*/
		else
				lab=(ptr)realloc(lab,sizeof(item)*(DC+1));/*reallocate the list and expand it if data exist, to add the new data*/
		if(!lab){
			fprintf(stderr,"cannot allocate mem\n");
			errFlag=1;
			return;
		}
		lab[DC].addr=START_ADDR+IC;
		strcpy(lab[DC].name,label);
		for(ch=line[*pos]; (ch==' ' || ch=='\t') && *pos<lenLine; ch=line[*pos], (*pos)++);
		if(*pos==lenLine){
			lab[DC].l_type=LABEL;
			lab[DC].d_type=MISS;/*label without any data type*/
			DC++;
			return;
		}
	}else{
		ch=line[*pos];
		(*pos)++;
	}
	for(i=1,token[0]=ch; ch!=' ' && ch!='\t' && *pos<=lenLine; ch=line[*pos],token[i]=ch, i++,(*pos)++);/*save in temp cmdLine after label*/
	if (i<MAX_STR_LEN ){
		i--;
		token[i]='\0';
	}
	if(is_cmd(token)!=-1){
		lab[DC].l_type=LABEL;
		lab[DC].d_type=MISS;/*label without any data type*/
		DC++;
		check_cmd(line,token,lenLine,pos);
		return;
	}
	if (token[0]=='.' && i>=4 && i<=7){
		if((strcmp(token,".data"))==0){
				if(label[0]!='\0'){
					lab[DC].l_type=DATA;
					lab[DC].d_type=INT;/*data of type int*/
					DC++;
				}
				for(ch=line[*pos];*pos<=lenLine; (*pos)++, ch=line[*pos]){
					if(ch!=' ' && ch!='\t' && j<MAX_STR_LEN){
						if((ch>='0' && ch<='9') || (ch=='-' && j==0)){
							token[j]=ch;
							j++;
						}else{
							if((ch==',' || ch=='\n' || *pos==lenLine) && j!=0){
								token[j]='\0';
								j=0;
								write_num(atoi(token));/*writes the number into the main memory*/
							}else{
								if(ch!=' ' && ch!='\t')
								fprintf(stderr, "error line: %d unknown sign %c\n", lineCount,ch);
								errFlag=1;
								return;
							 }
						 }
					}
				}

		}else{
			if(((strcmp(token,".ext"))==0) || ((strcmp(token,".extern"))==0)){
				if(!DC)
						lab=(ptr)malloc(sizeof(item));/*allocate if no data before*/
				else
						lab=(ptr)realloc(lab,sizeof(item)*(DC+1));/*reallocate the list and expand it if data exist, to add the new extern data*/
				if(!lab){
					fprintf(stderr,"cannot allocate mem\n");
					errFlag=1;
					return;
				}
				lab[DC].l_type=DATA;
				lab[DC].d_type=EXTERN;/*the data type extern*/
				for(ch=line[*pos];(ch==' ' || ch=='\t') && *pos<lenLine;(*pos)++,ch=line[*pos]);/*checks for errors and inccorect symbols*/
				for(i=0; ch!=' ' && ch!='\t' && *pos<lenLine;  (*pos)++ ,ch=line[*pos]){/* save in temp cmdLine after label*/
					if((isalpha(ch)!=0 && i==0) || ((isalpha(ch)!=0 || isdigit(ch)!=0) && i!=0)){
						token[i]=ch;
						i++;
					}
					else{
						fprintf(stderr, "error line: %d wrong symbol\n", lineCount);
						errFlag=1;
						return;
					}
				}
				if (i<MAX_STR_LEN && i!=0)
					token[i]='\0';
				else{
					fprintf(stderr, "error line: %d empty .extern\n", lineCount);
					errFlag=1;
					return;
				}
				for(ch=line[++(*pos)];ch!='\0' && *pos<=lenLine;(*pos)++){
					if(ch!=' ' && ch!='\t'){
						fprintf(stderr, "error line: %d after label wrong symbol\n", lineCount);
						errFlag=1;
						return;
					}
				}
				if (i<MAX_STR_LEN)
					token[i]='\0';
				lab[DC].addr=START_ADDR+IC;
				strcpy(lab[DC].name,token);
				extCount++;
				DC++;
				return;
			}else{
				if((strcmp(token,".string"))==0){/*from here until the next type, checks for correct string with correct symbols*/
					if(label[0]!='\0'){
						lab[DC].l_type=DATA;
						lab[DC].d_type=STRING;
						DC++;
					}
					for(; (ch==' ' || ch=='\t') && *pos<lenLine; ch=line[*pos], (*pos)++);
					if (ch=='"'){
						for(ch=line[*pos],(*pos)++; ch!='"' && *pos<=lenLine;ch=line[*pos], (*pos)++)
							write_num(ch);
						write_num(0);
						if(ch!='"'){
							fprintf(stderr, "error line: %d string must end with \"\n", lineCount);
							errFlag=1;
						}
					}else{
						fprintf(stderr, "error line: %d string must start with \"\n", lineCount);
						errFlag=1;
					}
					ch=line[*pos];
					for(;ch!='\0' && *pos<=lenLine;(*pos)++){
						ch=line[*pos];
						if(ch!=' ' && ch!='\t'){
							fprintf(stderr, "error line: %d after string unknown command - %c \n", lineCount,ch);
							errFlag=1;
							return;
						}
					}
				}else{
					if((strcmp(token,".entry"))==0){/*from here until the next type if any, checks for correct entry data with correct symbols*/
						if(!entryCount)
							entry=(ptr)malloc(sizeof(item));
						else
							entry=(ptr)realloc(entry,sizeof(item)*(entryCount+1));
						if(!entry){
							fprintf(stderr,"cannot allocate mem\n");
							errFlag=1;
							return;
						}
						entry[entryCount].l_type=DATA;
						entry[entryCount].d_type=ENTRY;
						for(ch=line[*pos];(ch==' ' || ch=='\t') && *pos<lenLine;(*pos)++,ch=line[*pos]);
						for(i=0; ch!=' ' && ch!='\t' && *pos<lenLine;  (*pos)++ ,ch=line[*pos]){/* save in temp cmdLine after label*/
							if((isalpha(ch)!=0 && i==0) || ((isalpha(ch)!=0 || isdigit(ch)!=0) && i!=0)){
								token[i]=ch;
								i++;
							}
							else{
								fprintf(stderr, "error line: %d wrong symbol\n", lineCount);
								errFlag=1;
								return;
							}
						}
						if (i<MAX_STR_LEN && i!=0)
							token[i]='\0';
						else{
							fprintf(stderr, "error line: %d empty .entry\n", lineCount);
							errFlag=1;
							return;
						}	

						for(;ch!='\0' && *pos<=lenLine;(*pos)++){
							ch=line[*pos];
							if(ch!=' ' && ch!='\t'){
								fprintf(stderr, "error line: %d after label unknown command - %c \n", lineCount,ch);
								errFlag=1;
								return;
							}
						}
						entry[entryCount].addr=START_ADDR+IC;
						strcpy(entry[entryCount].name,token);
						entryCount++;
						return;
					}else{
						fprintf(stderr,"error line: %d unknown command after label %s\n",lineCount,token);
						errFlag=1;
						return;
					}
				}
			}
		}
	}else{
		if(label[0]!='\0'){
			fprintf(stderr,"error line: %d undefined action after label\n",lineCount);
			errFlag=1;
		}else{
			fprintf(stderr,"error line: %d undefined action\n",lineCount);
			errFlag=1;
		}
	}
}

/*this function recives a line from line parser and a token, if the token is a command it will be written into the line struct
if no syntax error accours, the cmdLine into check cmdLine 
if any errors detected the error flag will be raised and the function will print the error and abort*/
void check_cmd(char line[],char token[],int lenLine, int *pos){
	struct cmdLineStruct cmdLine;
	int i,opCount=0,flagHooks=0,flagComma=0;
	if ((i=is_cmd(token))==-1)
		return;
	strcpy(cmdLine.cmd,token);
	cmdLine.op_code=i;

	for(; *pos<lenLine && (line[*pos]==' ' || line[*pos]=='\t'); (*pos)++);
	if(line[*pos]=='#' || (isalpha(line[*pos]))!=0){
		while(*pos<lenLine && opCount<3){
			for(; *pos<lenLine && (line[*pos]==' ' || line[*pos]=='\t'); (*pos)++);
			if (line[*pos]=='#'){
				for(i=0, (*pos)++; *pos<lenLine && line[*pos]!=',' && line[*pos]!=')' && line[*pos]!='\n' && line[*pos]!=' ' ; (*pos)++){
					if((isdigit(line[*pos]))!=0 || (i==0 && line[*pos]=='-')){
						token[i]=line[*pos];
						i++;
					}else{
						fprintf(stderr, "error line %d: after # must be integer\n", lineCount);
						errFlag=1;
						return;
					 }
				}
				if(i==0)
					fprintf(stderr, "error line %d: after # must be integer\n", lineCount);
				else{
					token[i]='\0';
					cmdLine.op[opCount]=(atoi(token));
					cmdLine.type[opCount]=IMM;
					opCount++;
				}
			}else{
				for(i=0; *pos<lenLine && line[*pos]!=' ' && line[*pos]!='\t' && line[*pos]!=',' && line[*pos]!='(' && ((isalpha(line[*pos]))!=0 || (isdigit(line[*pos]))!=0); token[i]=line[*pos] , i++, (*pos)++ );
				token[i]='\0';
				if(i==2 && token[0]=='r' && (isdigit(token[1]))!=0){
					if(atoi(token+1)>=8){
						fprintf(stderr,"error line %d: unknown register\n",lineCount);
						errFlag=1;
					}
					cmdLine.op[opCount]=(atoi(token+1));
					cmdLine.type[opCount]=REG;
					opCount++;
				
				}else{
					if((isalpha(token[0]))!=0){
						strcpy(mem[IC+opCount+1].label,token);
						cmdLine.type[opCount]=LABL;
						opCount++;
					}else{
						fprintf(stderr, "error line: %d unknown operator %s\n", lineCount,token);
						errFlag=1;
					}
				 }	
			 }
			 for(; *pos<lenLine && (line[*pos]==' ' || line[*pos]=='\t'); (*pos)++ );
			 if(line[*pos]==','){
				 (*pos)++;
				 flagComma++;
			 }
			 else
				 if(line[*pos]=='('){
					 flagHooks++;
					 (*pos)++;
				 }
				 else
					 if(line[*pos]==')'){
						 flagHooks--;
						 (*pos)++;
				 }

		}
	}else
		if(*pos<lenLine && line[*pos]!=' ' && line[*pos]!='\t' && line[*pos]!='\0'){
			fprintf(stderr, "error line: %d wrong sign %c \n", lineCount,line[*pos]);
			errFlag=1;
			return;
		}
	if(opCount!=0 && flagComma>=opCount){
		fprintf(stderr, "error line: %d expected operator after ,\n", lineCount);
		errFlag=1;
		return;
	}
	if(!flagHooks)
		check_cmd_line(cmdLine,opCount);
	else
		if(flagHooks>0){
			fprintf(stderr, "error line: %d expected )\n", lineCount);
			errFlag=1;
			return;
		}
		else{
			fprintf(stderr, "error line: %d expected (\n", lineCount);
			errFlag=1;
			return;
		}
}

/*this function writes each word into the main memory 
if any errors detected the error flag will be raised and the function will print the error and abort
all the labels are not written in the memory if are found, instead a flag which indicates a label is raised,
during the second run each raised flag is replaced with the correct label address*/
void check_cmd_line(struct cmdLineStruct cmdLine, int opCount){
	if(cmds[cmdLine.op_code].oper != opCount && (opCount!=1 && opCount!=3)){
		fprintf(stderr,"error line: %d incorrect operands count\n",lineCount);
		errFlag=1;
		return;
	}		
	
	if(opCount == 0){
		if(!strcmp(cmdLine.cmd,"stop") || !strcmp(cmdLine.cmd,"rts")){
				write_cmd(cmdLine.op_code,4,7);
				IC++;		
		}else{
			fprintf(stderr,"error line: %d wrong number of arguments\n",lineCount);
			errFlag=1;
			return;
		}	
	}else if(opCount == 1 || opCount == 3){
		if(!strcmp(cmdLine.cmd,"not") || !strcmp(cmdLine.cmd,"clr") || !strcmp(cmdLine.cmd,"inc") || !strcmp(cmdLine.cmd,"dec") || !strcmp(cmdLine.cmd,"red")){
				if(cmdLine.type[0] != LABL && cmdLine.type[0] != REG){
					fprintf(stderr,"error line: %d second argument wrong\n",lineCount);
					errFlag=1;
					return;
				}
				write_cmd(cmdLine.op_code,4,7);
					
				if(cmdLine.type[0] == LABL){
					write_cmd(LABL,10,11);
					IC++;
					write_cmd(0,0,11);
					write_cmd(1,14,14);
					IC++;
				}
				if(cmdLine.type[0] == REG){
					write_cmd(REG,10,11);
					IC++;
					write_cmd(cmdLine.op[0],8,11);
					IC++;
				}
		}else if(!strcmp(cmdLine.cmd,"jmp") || !strcmp(cmdLine.cmd,"bne")|| !strcmp(cmdLine.cmd,"jsr")){
			if(opCount==1){
				if(cmdLine.type[0] != LABL && cmdLine.type[0] != REG){
					fprintf(stderr,"error line: %d first parameter can't be assaigned\n",lineCount);
					errFlag=1;
					return;
				}
				if(cmdLine.type[0] == LABL)
				{
					write_cmd(cmdLine.op_code,4,7);
					write_cmd(LABL,10,11);
					IC++;
					write_cmd(0,0,11);
					write_cmd(1,14,14);
					IC++;
				}else if(cmdLine.type[0] == REG){
					write_cmd(cmdLine.op_code,4,7);
					write_cmd(cmdLine.type[0],10,11);
					IC++;
					write_cmd(cmdLine.op[0],0,5);
					IC++;
				}	

			}else if(opCount == 3){/*jump with parameters*/
				if(cmdLine.type[0]!=LABL){
					fprintf(stderr,"error line: %d first parameter must be LABEL\n",lineCount);
					errFlag=1;
					return;
				}
				if(cmdLine.type[1]!=LABL && cmdLine.type[1]!=IMM && cmdLine.type[1]!=REG){
					fprintf(stderr,"error line: %d first operator wrong\n",lineCount);
					errFlag=1;
					return;
				}
				if(cmdLine.type[2]!=LABL && cmdLine.type[2]!=IMM && cmdLine.type[2]!=REG){
					fprintf(stderr,"error line: %d second operator wrong\n",lineCount);
					errFlag=1;
					return;
				}
			
				write_cmd(cmdLine.op_code,4,7);
				write_cmd(JMP,10,11);
				if(cmdLine.type[1] == LABL){
					write_cmd(LABL,0,1);
					if(cmdLine.type[2] == LABL){
						write_cmd(LABL,2,3);
						IC++;
						write_cmd(0,0,11);
						write_cmd(1,14,14);
						IC++;
						write_cmd(0,0,11);
						write_cmd(1,14,14);
						IC++;
						write_cmd(0,0,11);
						write_cmd(1,14,14);
						IC++;
					}else if(cmdLine.type[2] == REG){
						write_cmd(REG,2,3);
						IC++;
						write_cmd(0,0,11);
						write_cmd(1,14,14);
						IC++;
						write_cmd(0,0,11);
						write_cmd(1,14,14);
						IC++;
						write_cmd(cmdLine.op[2],6,11);
						IC++;
					}else{
						write_cmd(cmdLine.type[2],2,3);
						IC++;
						write_cmd(0,0,11);
						write_cmd(1,14,14);
						IC++;
						write_cmd(0,0,11);
						write_cmd(1,14,14);
						IC++;
						write_cmd(cmdLine.op[2],0,11);
						IC++;
					}
				}else if(cmdLine.type[1] == REG){
					write_cmd(REG,0,1);
					if(cmdLine.type[2]==LABL){
						write_cmd(LABL,2,3);
						IC++;
						write_cmd(0,0,11);
						write_cmd(1,14,14);
						IC++;
						write_cmd(cmdLine.op[1],0,5);
						IC++;
						write_cmd(0,0,11);
						write_cmd(1,14,14);
						IC++;
					}else if(cmdLine.type[2]==REG){
						write_cmd(REG,2,3);
						IC++;
						write_cmd(0,0,11);
						write_cmd(1,14,14);
						IC++;
						write_cmd(cmdLine.op[1],0,5);
						write_cmd(cmdLine.op[2],6,11);
						IC++;
					}else{
						write_cmd(cmdLine.type[2],2,3);
						IC++;
						write_cmd(0,0,11);
						write_cmd(1,14,14);
						IC++;
						write_cmd(cmdLine.op[1],0,5);
						IC++;
						write_cmd(cmdLine.op[2],0,11);
						IC++;
					}
				}else if(cmdLine.type[1] == IMM){
					write_cmd(IMM,0,1);
					if(cmdLine.type[2] == LABL){
						write_cmd(LABL,2,3);
						IC++;
						write_cmd(0,0,11);
						write_cmd(1,14,14);
						IC++;
						write_cmd(cmdLine.op[1],0,11);
						IC++;
						write_cmd(0,0,11);
						write_cmd(1,14,14);
						IC++;
					}else if(cmdLine.type[2] == REG){
						write_cmd(REG,2,3);
						IC++;
						write_cmd(0,0,11);
						write_cmd(1,14,14);
						IC++;
						write_cmd(cmdLine.op[1],0,11);
						IC++;
						write_cmd(cmdLine.op[2],6,11);
						IC++;
					}else{
						write_cmd(cmdLine.type[2],2,3);
						IC++;
						write_cmd(0,0,11);
						write_cmd(1,14,14);
						IC++;
						write_cmd(cmdLine.op[1],0,11);
						IC++;
						write_cmd(cmdLine.op[2],0,11);
						IC++;
					}
				}

			}
		}else if(!strcmp(cmdLine.cmd,"prn")){
			if(cmdLine.type[0] != IMM && cmdLine.type[0] != REG && cmdLine.type[0] != LABL){
				fprintf(stderr,"error line: %d second argument wrong\n",lineCount);
				errFlag=1;
				return;
			}
			write_cmd(cmdLine.op_code,4,7);

			if(cmdLine.type[0] == IMM){
				write_cmd(IMM,10,11);
				IC++;
				write_cmd(cmdLine.op[0],0,11);
				IC++;
			}else if(cmdLine.type[0] == REG){
				write_cmd(REG,10,11);
				IC++;
				write_cmd(cmdLine.op[0],6,11);
				IC++;
			}else{
				write_cmd(LABL,10,11);
				IC++;
				write_cmd(0,0,11);
				write_cmd(1,14,14);
				IC++;
			}
		}else{
			fprintf(stderr,"error line: %d wrong number of arguments\n",lineCount);
			errFlag=1;
			return;
		}	

	}else if(opCount == 2){
		if(!strcmp(cmdLine.cmd,"mov") || !strcmp(cmdLine.cmd,"add") || !strcmp(cmdLine.cmd,"sub")){
			if(cmdLine.type[0] != REG && cmdLine.type[0] != LABL && cmdLine.type[0] != IMM){
				fprintf(stderr,"error line: %d first argument wrong\n",lineCount);
				errFlag=1;
				return;
			}if(cmdLine.type[1] != LABL && cmdLine.type[1] != REG){
				fprintf(stderr,"error line: %d second argument wrong\n",lineCount);
				errFlag=1;
				return;
			}
			write_cmd(cmdLine.op_code,4,7);
			if(cmdLine.type[0] == REG && cmdLine.type[1] == REG){
				write_cmd(REG,10,11);
				write_cmd(REG,8,9);
				IC++;
				write_cmd(cmdLine.op[0],0,5);
				write_cmd(cmdLine.op[1],6,11);
				IC++;
			}else if(cmdLine.type[0] == REG && cmdLine.type[1] == LABL){
				write_cmd(LABL,10,11);
				write_cmd(REG,8,9);
				IC++;
				write_cmd(cmdLine.op[0],0,5);
				IC++;
				write_cmd(0,0,11);
				write_cmd(1,14,14);
				IC++;
			}else if(cmdLine.type[0] == LABL && cmdLine.type[1] == LABL){
				write_cmd(LABL,8,9);
				write_cmd(LABL,10,11);
				IC++;
				write_cmd(0,0,11);
				write_cmd(1,14,14);
				IC++;
				write_cmd(0,0,11);
				write_cmd(1,14,14);
				IC++;
			}else if(cmdLine.type[0] == LABL && cmdLine.type[1] == REG){
				write_cmd(LABL,8,9);
				write_cmd(REG,10,11);
				IC++;
				write_cmd(0,0,11);
				write_cmd(1,14,14);
				IC++;
				write_cmd(cmdLine.op[1],6,11);
				IC++;
			}else if(cmdLine.type[0] == IMM && cmdLine.type[1] == REG){
				write_cmd(IMM,8,9);
				write_cmd(REG,10,11);
				IC++;
				write_cmd(cmdLine.op[0],0,11);
				IC++;
				write_cmd(cmdLine.op[1],6,11);
				IC++;
			}else if(cmdLine.type[0] == IMM && cmdLine.type[1] == LABL){
				write_cmd(IMM,8,9);
				write_cmd(LABL,10,11);
				IC++;
				write_cmd(cmdLine.op[0],0,11);	
				IC++;
				write_cmd(0,0,11);
				write_cmd(1,14,14);
				IC++;
			}
			
		}else if(!strcmp(cmdLine.cmd,"cmp")){
				if(cmdLine.type[0] != REG && cmdLine.type[0] != LABL && cmdLine.type[0] != IMM){
					fprintf(stderr,"error line: %d first argument wrong\n",lineCount);
					errFlag=1;
					return;
				}
				if(cmdLine.type[1] != REG && cmdLine.type[1] != LABL && cmdLine.type[1] != IMM){
				fprintf(stderr,"error line: %d second argument wrong\n",lineCount);
				errFlag=1;
				return;
				}
				write_cmd(cmdLine.op_code,4,7);

				if(cmdLine.type[0] == LABL || cmdLine.type[1] == LABL){
					write_cmd(cmdLine.type[0],8,9);
					write_cmd(cmdLine.type[1],10,11);
					IC++;

					if(cmdLine.type[0] == LABL){
						write_cmd(0,0,11);
						write_cmd(1,14,14);
						IC++;

						if(cmdLine.type[1] == REG){
							write_cmd(cmdLine.op[1],6,11);
							IC++;
						}else if(cmdLine.type[1] == IMM){
							write_cmd(cmdLine.op[1],0,11);	
							IC++;
						}
					}
					if(cmdLine.type[1] == LABL){
						if(cmdLine.type[0] == REG){
							write_cmd(cmdLine.op[0],0,5);
							IC++;
						}else if(cmdLine.type[0] == IMM){
							write_cmd(cmdLine.op[0],0,11);		
							IC++;
						}
						write_cmd(0,0,11);
						write_cmd(1,14,14);
						IC++;
					}
				}else if(cmdLine.type[0] == REG && cmdLine.type[1] == REG){
					write_cmd(cmdLine.type[0],8,9);
					write_cmd(cmdLine.type[1],10,11);
					IC++;
					write_cmd(cmdLine.op[0],0,5);
					write_cmd(cmdLine.op[1],6,11);
					IC++;
				}else{
					write_cmd(cmdLine.type[0],8,9);
					write_cmd(cmdLine.type[1],10,11);
					IC++;
					if(cmdLine.type[0]==REG){
						write_cmd(cmdLine.op[0],0,5);
						IC++;
						write_cmd(cmdLine.op[1],0,11);
						IC++;
					}	else if(cmdLine.type[1]==REG){
						write_cmd(cmdLine.op[0],0,11);
						IC++;
						write_cmd(cmdLine.op[1],6,11);
						IC++;
					}else{
						write_cmd(cmdLine.op[0],0,11);
						IC++;
						write_cmd(cmdLine.op[1],0,11);
						IC++;
					}
				}
		}else if(!strcmp(cmdLine.cmd,"lea")){
				if(cmdLine.type[0] != LABL){
					fprintf(stderr,"error line: %d first argument wrong\n",lineCount);
					errFlag=1;
					return;
				}
				if(cmdLine.type[1] != REG && cmdLine.type[1] != LABL){
				fprintf(stderr,"error line: %d second argument wrong\n",lineCount);
				errFlag=1;
				return;
				}
				write_cmd(cmdLine.op_code,4,7);
				write_cmd(cmdLine.type[0],8,9);
				
				if(cmdLine.type[1] == LABL){
					write_cmd(cmdLine.type[1],10,11);
					IC++;
					write_cmd(0,0,11);
					write_cmd(1,14,14);
					IC++;
					write_cmd(0,0,11);
					write_cmd(1,14,14);
					IC++;
				}else{
					write_cmd(cmdLine.type[1],10,11);
					IC++;
					write_cmd(cmdLine.op[1],6,11);
					IC++;
				}
			}else{
			fprintf(stderr,"error line: %d wrong number of arguments\n",lineCount);
			errFlag=1;
			return;
		}	
	}
}
