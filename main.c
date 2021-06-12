#include "global.h"

#define FILE_READ ".as"
#define FILE_OBJECT ".ob"
#define FILE_EXTERN ".ext"
#define FILE_ENTRY ".ent"

/*
project goal is to translate assembly files to special binary files.
Input files are given in command arguments after ./ex14
the program recieves files with .as ending (no need to write .as in input)
if translation encounters error it will send them to stderr and stop the translation at end of file.
if no errors 3 files will be made for each input file:
1) file.ob - object code
2) file.ent - entries label codes
3) file.ext - external label codes

data types : for the main memory we used and array
for other memory structures such as entry and labels we used allocated lists  */

void file_parser(FILE *fp1);/*analizes the file that is sent to the program*/
void check_write_data(FILE *fp,FILE *fp1);/*checks the data and rewrites the correct addresses for labels*/
void mem_to_sbin(FILE *fp);/*translates the memory from binary to sepcial binary*/


struct memStruct mem[MEM_SIZE];/*this is the struct to hold the main memory */
int IC,DC,lineCount,DC,entryCount,errFlag,extCount;/*extern integers for different uses*/
ptr lab,entry;/*pointers to structs to hold labels and entry memory*/


int main (int argc, char *argv[]) {
	FILE *fp1,*fp2; /* fp1 for read fp2 for write */
	int i;
	char fileName[4][50];

	if (argc == 1) {
		fprintf (stderr,"No input file.\n");
		return 1;
	}
	for (i=1;i<argc;i++) {
		printf("start\n\n");
		strcpy (&fileName[0][0],*(argv+i));
		strcat (&fileName[0][0], FILE_READ); /* append .as to file name */
		strcpy (&fileName[1][0],*(argv+i));
		strcat (&fileName[1][0], FILE_OBJECT); /* append .ob to file name */
		strcpy (&fileName[2][0],*(argv+i));
		strcat (&fileName[2][0], FILE_EXTERN); /* append .ext to file name */
		strcpy (&fileName[3][0],*(argv+i));
		strcat (&fileName[3][0], FILE_ENTRY); /* append .ent to file name */
		

		if (!(fp1 = fopen(&fileName[0][0], "r"))) { /* open .as file for read */
			fprintf (stderr, "can't open %s\n", &fileName[0][0]);
			return 1;
		}
	file_parser(fp1);
	fclose(fp1);
	if(!errFlag){

	if(DC){
		if (!(fp1 = fopen(&fileName[2][0], "w"))) { /* open .ext file for write */
				fprintf (stderr, "can't open %s\n", &fileName[2][0]);
				return 1;
		}
		printf ("creating extern filename: %s\n", &fileName[2][0]);
		if(entryCount){
			if (!(fp2 = fopen(&fileName[3][0], "w"))) { /* open .ent file to write */
				fprintf (stderr, "can't open %s\n", &fileName[3][0]);
				return 1;
			}
			printf ("creating entry filename: %s\n", &fileName[3][0]);
			check_write_data(fp1,fp2);
			
		}
		else{
			fp2=NULL;
			check_write_data(fp1,NULL);
		}
		if(errFlag){
			if(fp1){
				fprintf(stderr,"error found removing %s\n",&fileName[2][0]);
				remove(&fileName[2][0]);
				fclose(fp1);
			}
			if(fp2){
				fprintf(stderr,"error found removing %s\n",&fileName[3][0]);
				remove(&fileName[3][0]);
				fclose(fp2);
			}
			
		}
		else{
			if(fp2)
				fclose(fp2);
			if(fp1)
				fclose(fp1);
		}
	
	}else{
		if(entry){
			fprintf(stderr,"undefined entry\n");
			errFlag=1;
		}
	}
	if(!errFlag){

		if (!(fp2 = fopen(&fileName[1][0], "w"))) { /* open .ob file to write */
				fprintf (stderr, "can't open %s\n", &fileName[1][0]);
				return 1;
			}else{
				printf ("creating object filename: %s\n", &fileName[1][0]);
				mem_to_sbin(fp2);
				fclose(fp2);	
			}

			
	}else
		fprintf(stderr,"errors detected\n");

	memset(mem,0,sizeof(mem));
	free(lab);
	free(entry);
	putchar('\n');

}else{
	fprintf(stderr, "error, no files are being made\n");
	free(lab);
	free(entry);
	memset(mem,0,sizeof(mem));
	putchar('\n');
}
	printf("end\n\n");
	}

	return 0;
}