#include "global.h"
#include "extern.h"
/*this part parses the given arguments, wheter they are files or lines*/

/*analaizes the files and sends each line to the line parser*/
void file_parser(FILE *fp1){
	char line [MAX_LINE_LEN];
	int ch,i,lenLine;
	ch=' ';
	errFlag=IC=lineCount=DC=entryCount=extCount=0;
	lab=entry=NULL;
	while (ch!=-1) {
		lineCount++;
		while(ch==' ' || ch=='\t')
			ch = getc(fp1);
		for (i=0; ch!='\n' && ch!=-1 && i<MAX_LINE_LEN;i++ ){
			line[i] = (char) ch;
			ch = getc(fp1);
		}
		lenLine = i;
		line[i] ='\0';
		if(i!=0 && line[0]!=';' && line[0]!='\0' && line[0]!='\n')
			line_parser(line,lenLine);
		while (ch!='\n' && ch!=-1)
				ch = getc(fp1);
		if(ch=='\n')
			ch = getc(fp1);
	}

}

/*analaizes the line it recieves and sends to check for labels and command lines*/
void line_parser(char line[], int lenLine){
	char token[MAX_STR_LEN];
	int pos=0,i;
	for (i =0; pos<lenLine && (line[pos] != '.' && line[pos] != ' ' && line[pos]!='\t' && line[pos]!=','); pos++){  /* save in token[] first cmdLine in line*/
		if(i<MAX_STR_LEN && ((i==0 && isalpha(line[pos])!=0) || ((isalpha(line[pos])!=0 || isdigit(line[pos])!=0) && i!=0) || (i!=0 && pos+1<=lenLine && line[pos]==':' && (pos+1==lenLine || line[pos+1]==' '|| line[pos+1]=='\t')))){
			token[i]=line[pos];
			i++;
		}else{
			fprintf(stderr, "error line %d: unknown command or label\n", lineCount);
			errFlag=1;
		}
	}
	token[i]='\0';
	if (i==0 || ((isalpha(token[0]))!=0 && token[i-1]==':')){
		if(i!=0)
			token[i-1]='\0';
		check_label(line,token,lenLine, &pos);
		return;
	}
	if(is_cmd(token)!=-1){
		check_cmd(line,token,lenLine, &pos);
		return;
	}
	fprintf(stderr, "error line %d: unknown command or label\n", lineCount);
	errFlag=1;
}