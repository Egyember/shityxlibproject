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

int stringLen(char* str){
	int len = strlen(str); //local wraper needed because i had no idea tha this thing existed when i wrote the rest of the file
	return len;
};

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
void gravity(struct entity* target){
	target->vector[0]+=GRAVITYFOCE;
};
void pointVectorToLine(int point[2], int vector[2], line returnPointer){
//vec(V1, V2)
//P(X0,Y0)
//V2x - V1y = V2X0 - V1Y0
//Ax - By = C
//returnPointer[0]=A
//returnPointer[1]=B
//returnPointer[2]=C
	int A = vector[1];
	int B = vector[0];
	int C = vector[1]*point[0]-vector[1]*point[1];
	(*returnPointer)[0] = A;
	(*returnPointer)[1] = B;
	(*returnPointer)[2] = C;

};

void getCollisionTowLine(line line1, line line2, int *returnprt[2]){
//line1 = (a,b,c)
//line2 = (d,e,f)
//
//ax-by=c
//dx-ey=f
//
//y = (dc-af)/((-db)-(-ae))
//x = (c+by)/a



};
void applyVector(struct entity* target){
	target->x += target->vector[0];
	target->y += target->vector[1];	
/*
//only for testing
	target->vector[0] = 0;	
	target->vector[1] = 0;	
*/
	};
void calculateTick(struct entityStart firstEntity){
	if(firstEntity.firstNode == NULL){
		exit(ERROR);
	};
	struct entity* currentEntity =firstEntity.firstNode;
	bool run =true;
	while(run){
		if(currentEntity->hasPhysics == true){
			gravity(currentEntity);
		};
		applyVector(currentEntity);
		currentEntity = currentEntity->nextEntity;
		if(currentEntity == NULL){
			run = false;
		};
	};
};

int main(){
	printf("hello word!\n");
	//connect to the x server ($DISPLAY env)
	Display *d;
	d = XOpenDisplay(NULL);
	if (d==NULL){
		printf("We are fucked");
		exit(ERROR);

	};
	printf("successful connection\n");
	//test
	
	//connection number (this is the df for the connection)
	int ConnectionNumber = XConnectionNumber(d);
	printf("ConnectionNumber: %d\n", ConnectionNumber);
	
	//screen count
	printf("XScreenCount: %d\n", ScreenCount(d));

	//default screen
	int ds =DefaultScreen(d);
	printf("ds: %d\n", ds);

	//DefaultDepth
	printf("DefaultDepth: %d\n", DefaultDepth(d,0));
	//create a window
	Window Win = XCreateSimpleWindow(d, RootWindow(d, ds), 2, 1, 400, 400, 1, WhitePixel(d, ds), WhitePixel(d, ds));
	XSelectInput(d, Win, StructureNotifyMask);
	XMapRaised(d, Win);
	for(;;) {
		XEvent e;
		XNextEvent(d, &e);
		if (e.type == MapNotify){
			break;
		}}
	
	//alloc colormap
	Colormap MyColorMap = XCreateColormap(d, Win, DefaultVisual(d, ds), 0);
	XInstallColormap(d, MyColorMap);

	//alloc color
	XColor RedColor;
	char ColorName[] = "rgb:ff/00/00";
	XParseColor(d, MyColorMap, ColorName, &RedColor);
	RedColor.flags = DoRed | DoGreen | DoBlue;
	XAllocColor(d, MyColorMap, &RedColor);

	//alloc GC
	GC gcBlack = XCreateGC(d, Win, 0, NULL);
	GC gcRed = XCreateGC(d, Win, 0, NULL);
	
	//set gc foreground color	
	XSetForeground(d, gcBlack, BlackPixel(d, ds)); 
	XSetForeground(d, gcRed, RedColor.pixel);
	XSetLineAttributes(d, gcRed, 10, LineSolid, CapButt, JoinRound);
	XSetLineAttributes(d, gcBlack, 10, LineSolid, CapButt, JoinRound);
	
	//drawing
	XDrawLine(d, Win, gcBlack, 10, 10, 200, 200);
	

	XDrawLine(d, Win, gcRed, 300, 10, 10, 300);
	XFlush(d);

	//text
	//
	//obtaining font names
	int numberOfFonts;
	char **fontlist = XListFonts(d, "-misc-unifont-medium-*iso8859-2", 200, &numberOfFonts); //I fucking hate font handeling in X11
	if(numberOfFonts==0){
		printf("missing font useing fallback\n");
		fontlist = XListFonts(d, "*iso8859-2", 200, &numberOfFonts); //I fucking hate font handeling in X11
	};	
	printf("printing fonts \n");
	printf("%d\n", numberOfFonts);
	for(int i;numberOfFonts>i;i++){
		printf("font[%d]%s \n", i,fontlist[i]);
	};
	//sleep(1);

	//load font
	Font fontId = XLoadFont(d, fontlist[0]);
	if (fontId == BadName || fontId == BadAlloc){ 
		printf("bad name/BadAlloc \n");
		exit(ERROR);
		;};
	
	// get XFontStruct
	XFontStruct *fontStruct = XQueryFont(d, fontId);

	/*
	//testing fontid
	if (fontId == fontStruct->fid){printf("good\n");};
	*/

	//stirng width
	char testString[] = "árvíztürő tűkörfúrógép"; // todo: convert it to ISO 8856-2 or ISO 10xxx the fuck know the number
	printf("Teszt string: %s\n", testString);
	int testStringWidth;
	testStringWidth = XTextWidth(fontStruct, testString, stringLen(testString));
	printf("A testString %d px széles\n", testStringWidth);
	
	//drawing string
		//set font for GCs
	XSetFont(d, gcRed, fontId);
	XSetFont(d, gcBlack, fontId);
		//start drawing
	XDrawString(d, Win, gcBlack, 30, 30, testString, stringLen(testString));
		//flush Xbuffer
	XFlush(d);
	//sleep(10);
//todo: design map and entity data structures
//todo load map data from a custom file format
//todo: render image from loaded ppm files (with the cpu)

	struct ppmImage playerImg =loadPpmImg("./img/player.ppm");
	struct ppmImage hpImg =loadPpmImg("./img/hp.ppm");
	

	XImage *testImg = XCreateImage(d, DefaultVisual(d, ds), DefaultDepth(d,DefaultScreen(d)) , ZPixmap, 0, hpImg.data, hpImg.width, hpImg.high, 32,0 );
	printf("bits_per_pixel: %d\n", testImg->bits_per_pixel);
	printf("sizeof us %lu\n", sizeof(unsigned char));
	XPutImage(d, Win, DefaultGC(d, DefaultScreen(d)), testImg, 0, 0, 100,100,100,100);
	sleep(10);
	
	//main loop
	struct entityStart mainentity;
	mainentity.firstNode = (struct entity*) malloc(sizeof(struct entity));
	mainentity.totalNumber = 1;
	mainentity.firstNode->texture = loadPpmImg("./img/player.ppm");
	mainentity.firstNode->type = player;
	mainentity.firstNode->hasPhysics = true;
	mainentity.firstNode->hasCollision = true;
	mainentity.firstNode->nextEntity = NULL;
	mainentity.firstNode->hitBox[0] = 10;
	mainentity.firstNode->hitBox[1] = 10;
	mainentity.firstNode->hitBox[2] = -10;
	mainentity.firstNode->hitBox[3] = -10;
	mainentity.firstNode->ID = 1;
	mainentity.firstNode->x = 0;
	mainentity.firstNode->y = 0;
	mainentity.firstNode->vector[0] = 0;
	mainentity.firstNode->vector[1] = 0;


  time_t start,end;
  double dif;

	while(true){
		time(&start);
		calculateTick(mainentity);
		printf("x: %d\ny: %d\n", mainentity.firstNode->x, mainentity.firstNode->y);
		time (&end);
		dif = difftime (end,start);
		sleep((1.0/60.0)-dif);
		printf("sleept %f\n", (1.0/60.0)-dif);

	};

	free(hpImg.data);
	free(playerImg.data);
	//unload font
	XUnloadFont(d, fontId);
	//close connection&destroy window
	XDestroyWindow(d, Win);
	XCloseDisplay(d);
	return GOOD;
};
