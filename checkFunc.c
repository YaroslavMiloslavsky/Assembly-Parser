#include "global.h"
#include "extern.h"

/*this part writes the actual memory into the main memory be it a number or a word with labels and opcodes*/

/*this function writes data wheter it's a string or a data */
void write_num(int num){
	int flag_minus=0,i=13;
	int temp=num;
	if(num<-8192 || num>8191){
		fprintf(stderr,"error line %d: overflow - %d \n",lineCount,num);
		errFlag=1;
		return;
	}
	if (num<0){
		flag_minus=1;
		temp=(num*-1)-1;
	}
	do{
		mem[IC].memLine[i]=temp%2;
		--i;
		temp=temp/2;
	}while(temp!=0 && i>=0);
	if(flag_minus==1){
		for(i=0; i<14;i++){
			if(mem[IC].memLine[i]!=1)
				mem[IC].memLine[i]=1;
			else
				mem[IC].memLine[i]=0;
		}
	}
	IC++;
}

/*this function writes command lines and labels into the main memory*/
void write_cmd(int value, int i, int j){
	int flag_minus=0;
	int temp=value;
	if(j-i==11 && (value>2047 || value<-2047)){
		fprintf(stderr,"error line %d: overflow - %d \n",lineCount,value);
		errFlag=1;
		return;
	}
	if (value<0){
		flag_minus=1;
		temp=(value*-1)-1;
	}
	do{
		mem[IC].memLine[j]=temp%2;
		--j;
		temp=temp/2;
	}while(temp!=0 && j>=i);
	if(flag_minus==1){
		for(j=0; j<12;j++){
			if(mem[IC].memLine[j]!=1)
				mem[IC].memLine[j]=1;
			else
				mem[IC].memLine[j]=0;
		}
	}
}