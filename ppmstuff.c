#ifndef mainincludes

#define mainincludes
#include <bits/time.h>
#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h> //main Xlib header
#include <X11/Xcms.h> //color stuff
#include <unistd.h> //sleep
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "macros.h"
#include "types.c"

#endif

struct ppmHeader checkppm(char* PATH){
	struct ppmHeader hearer;
	FILE *fptr;
	fptr = fopen(PATH, "r");
	if(fptr == NULL){
		printf("szar PATH\n");
		exit(ERROR);
	};

	//testing the header
	int headerLen=0;
	//magicbyte
	char magicbyte[3]= "";
	fgets(magicbyte, 3,fptr);
	if(strcmp(magicbyte, "p6")){
		hearer.type = RAW_PPM;
	} else {
		hearer.type = ASCII_PPM;
	};
	fseek(fptr, 1, SEEK_CUR);
	headerLen +=3;
	//skip comments
	bool run= true;
#ifdef DEBUG
char debugSTR[99];
#endif
	while(run){
		char current =fgetc(fptr);
		if(current == '#' ){
			headerLen +=1;
			for(char i = 0; i != EOL; i = fgetc(fptr)){
				headerLen +=1;
#ifdef DEBUG
				debugSTR[headerLen] = i;
#endif
			};
		} else{
			run = false;
			fseek(fptr, -1, SEEK_CUR);
		};	
	};
	//image size
	char sizeStr[6]; //65536 is the max value
	for(int i=0; i<6; i++){
		char current =fgetc(fptr);
		if(current != ' '){
		sizeStr[i] = current;
		headerLen++;
		}else{
		headerLen++;
		break;
		};
	};

	hearer.sizeX = atoi(sizeStr);

	for(int i=0; i<6; i++){
		char current =fgetc(fptr);
		if(current != '\n'){
		sizeStr[i] = current;
		headerLen++;
		}else{
		headerLen++;
		break;
		};
	};
	hearer.sizeY = atoi(sizeStr);
	//bitdepth
	char bitdepthStr[6]; //65536 is the max value

	for(int i=0; i<6; i++){
		char current =fgetc(fptr);
		if(current != '\n'){
		bitdepthStr[i] = current;
		headerLen++;
		}else{
		headerLen++;
		break;
		};
	};

	hearer.bitdepth = atoi(bitdepthStr);
	hearer.headerLen = headerLen;
	fclose(fptr);
	return hearer;
};

struct ppmImage loadPpmImg(char* PATH){
	struct ppmHeader hearer = checkppm(PATH);
		//loading the rgb data to memory
	if(!(hearer.bitdepth==255)){
		printf("bad file %s\n", PATH);
		exit(ERROR);
	};
	FILE *fptr;
	fptr = fopen(PATH, "rb");
	fseek(fptr, hearer.headerLen, SEEK_SET);
	//alloc memory
	struct ppmImage outStruct;
	outStruct.data= (unsigned char*) malloc(hearer.sizeX*hearer.sizeY*4);
	//loading rgb data into memory
	unsigned char *p = outStruct.data;
	p++;
	for(int i = 0; i<100;i++){
		for(int j = 0; j<100;j++){
			*(p+1)=fgetc(fptr);
			*p=fgetc(fptr);
			*(p-1)=fgetc(fptr);
			if(p!=outStruct.data+hearer.sizeX*hearer.sizeY*4-3){
			p+=4;
			};
		};
	};
	//copy metadata
	outStruct.high=hearer.sizeX;
	outStruct.width=hearer.sizeY;
	outStruct.bitdepth=hearer.bitdepth;
	return outStruct;
};
