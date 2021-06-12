#include "global.h"
#include "extern.h"


/*this part updates the label addresses from the first run
for each label in the first run, a flag raied, which indicates which address should be replaced during the second run
if any errors encounterd and error flag will be raised, the errors printed and the program aborted*/

/*this is the function that checks each label for the flags after the first run and writes updated address in the main memory*/
void check_write_data(FILE *fp,FILE *fp1){
	int i,j,m,k,flag;
	for(i=0 ; i<DC ; i++){
		for(j=i ; j<DC ;j++){
			if(i!=j && !strcmp(lab[i].name,lab[j].name)){/*if the same label is encountered*/
				fprintf(stderr,"%s same label\n",lab[i].name);
				errFlag=1;
			}
		}
	}
	i=IC;
	for(IC=0;IC<i;IC++){
		if(mem[IC].memLine[14]==1){
			for(flag=0,j=0;j<DC && flag==0; j++){
				if(strcmp(mem[IC].label,lab[j].name)==0){
					flag=1;
					if(lab[j].d_type==EXTERN){
						fprintf(fp,"%s",lab[j].name);
						for(m=0;m<MAX_STR_LEN-strlen(lab[j].name);m++)
							fputc(' ',fp);
						fprintf(fp,"  %d",IC+100);
						fputc('\n',fp);
						mem[IC].memLine[13]=1;
					}
					else{
						write_cmd(lab[j].addr,0,11);
						mem[IC].memLine[12]=1;
					
						
					}
				}	
			}
			if(!flag){
					fprintf(stderr,"label %s not found\n",mem[IC].label);
					errFlag=1;
					return;
			}
		}
	}
	for(k=flag=0;fp1!=NULL && k<entryCount;k++){
		for(j=0;j<DC ;j++){
							if(strcmp(lab[j].name,entry[k].name)==0){
								flag++;
								fprintf(fp1,"%s",entry[k].name);
								for(m=0;m<MAX_STR_LEN-strlen(entry[k].name);m++)
									fputc(' ',fp1);
								fprintf(fp1,"  %d",lab[j].addr);
								fputc('\n',fp1);
							}
						}
					}
					if(flag!=entryCount){
					fprintf(stderr,"label %s not found\n",mem[IC].label);
					errFlag=1;
					return;
			}
	IC=i;
}


/*helping function which converts a binary code into a special binary code*/

void mem_to_sbin(FILE *fp){
	int i,j;
	for(i=0;i<IC;i++){
		fprintf(fp,"#%d     ",i+100);
		for(j=0; j<14; j++)
			if(mem[i].memLine[j] == 1)
				fputc('/',fp);
			else
				fputc('.',fp);
		fputc('\n',fp);
	}
}