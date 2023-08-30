#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h> //main Xlib header
#include <X11/Xcms.h> //color stuff
#include <unistd.h> //sleep
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "macros.h"
#include "types.c"

int stringLen(char* str){
	int len = strlen(str); //local wraper needed because i had no idea tha this thing existed when i wrote the rest of the file
	return len;
};

img100* loadimg100(char* PATH){
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
	if(!strcmp(magicbyte, "p6")){
	printf("wrogn file (raw ppm expected)\n");
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
	char sizeXStr[6]; //65536 is the max value
	for(int i=0; i<6; i++){
		char current =fgetc(fptr);
		if(current != ' '){
		sizeXStr[i] = current;
		headerLen++;
		}else{
		headerLen++;
		break;
		};
	};

	char sizeYStr[6]; //65536 is the max value

	for(int i=0; i<6; i++){
		char current =fgetc(fptr);
		if(current != '\n'){
		sizeYStr[i] = current;
		headerLen++;
		}else{
		headerLen++;
		break;
		};
	};
	int sizeX = atoi(sizeXStr);
	int sizeY = atoi(sizeYStr);
	if(!(sizeX == 100 & sizeY == 100)){
		printf("expected image size 100x100 got %dx%d\n", sizeX, sizeY);
		exit(ERROR);
	};
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

	int bitdepth = atoi(bitdepthStr);
	if(bitdepth != 255){
		printf("expected image bitdepth is 255 got %d\n", bitdepth);
		exit(ERROR);

	};
	printf("ppm header len %d\n", headerLen);
	//alloc memory
	img100* outprt = malloc(sizeof(img100));

	return outprt;
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
	
	//set gc fireground color	
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
		printf("missing font\n");
		exit(ERROR);
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
//next task load an image and put it to the sceen
//todo: design map and entity data structures
//todo load map data from a custom file format
//todo: render image from loaded ppm files (with the cpu)

	img100* playerImg =loadimg100("./img/player.ppm");
	/*img100* hpImg =loadimg100("./img/hp.ppm"); // másodjára szarul fut le valamiért
//sometime the values wrong todo: find the bug
	printf("hp0 0 red: %hhu\n", (*hpImg)[0][0][0]);
	printf("hp0 0 green: %hhu\n", (*hpImg)[0][0][1]);
	printf("hp0 0 blue: %hhu\n", (*hpImg)[0][0][2]);
	printf("hp50 50 red: %hhu\n", (*hpImg)[50][50][0]);
	printf("hp50 50 green: %hhu\n", (*hpImg)[50][50][1]);
	printf("hp50 50 blue: %hhu\n", (*hpImg)[50][50][2]);
	printf("hp50 50 red: %hhu\n", (*hpImg)[50][50][0]);
	printf("hp50 50 green: %hhu\n", (*hpImg)[50][50][1]);
	printf("hp50 50 blue: %hhu\n", (*hpImg)[50][50][2]);
	

	free(hpImg);*/
	free(playerImg);
	//unload font
	XUnloadFont(d, fontId);
	//close connection&destroy window
	XDestroyWindow(d, Win);
	XCloseDisplay(d);
	return GOOD;
};
