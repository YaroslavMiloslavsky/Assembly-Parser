#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MEM_SIZE 2000/*memory size, we cant know the exact size*/
#define MAX_LINE_LEN 100/*max line lenght inside files*/
#define START_ADDR 100/*the IC*/
#define MAX_STR_LEN 20/*assumed string names length*/

typedef enum {IMM=0,LABL,JMP,REG} op_type;/*the operators types for meory words*/

typedef enum {DATA=0,LABEL} label_type;/*labels types */

typedef enum {INT=0,STRING,ENTRY,EXTERN,MISS} data_type;/*data types*/

/*the main memory array*/
struct memStruct{
	int memLine[15];/*the 14 bites of a word + the address flag*/
	char label[MAX_STR_LEN];/*name of the label*/
};

/*each line in the memory*/
struct cmdLineStruct{
	char cmd[5];/*the opecode name*/
	int op_code,op[3];/*the opcode number, max 3 operators, op has each operator */
	op_type type[3];/*each of the max 3 operator types*/
};

typedef struct labelStruct * ptr ;/*pointer for struct*/

/*struct to hold lables*/
typedef struct labelStruct{
	char name[MAX_STR_LEN];/*label name*/
	int addr;/*label adress*/
	label_type l_type;/*label type*/
	data_type d_type;/*label data type*/
}item;





int is_cmd(char name[]);/*checks if opcode*/
void write_num(int num);/*writes numbers into memory*/
void check_label(char line[],char label[],int lenLine, int *pos);/*checks the labels in the line*/
void line_parser(char line[], int lenLine);/*analizes the data in a given line*/
void file_parser(FILE *fp1);/*analizes each files*/
void check_cmd(char line[],char token[],int lenLine, int *pos);/*checks for command lines and labels*/
void check_cmd_line(struct cmdLineStruct cmdLine, int opCount);/*checks and writes each line of word with the label and the correct data*/
void write_cmd(int value, int firstIndex, int lastIndex);
void mem_to_sbin(FILE *fp);/*converts binary to special binary*/
void check_write_data(FILE *fp,FILE *fp1);/*updates the addresses during the second run and wrties lables into memory*/