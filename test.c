#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h> //main Xlib header
#include <X11/Xcms.h> //color stuff
#include <unistd.h> //sleep
#include <stdlib.h>
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
	//char tmp[3];
	union uTMP{
		char small[4];
		char big[80];
	}tmp;
	//todo: fix this

	fgets(tmp.small, 4, fptr);
	if(strcmp(tmp.small,  "P6\n")){
		printf("szar file \n");
		printf("magic byte: %s\n", tmp.small);
		exit(ERROR);
	};
	fgets(tmp.big, 80, fptr);
	if(tmp.big[0] == '#'){
		printf("skiping comment\n");
		fgets(tmp.big, 80, fptr);
	};
	if(strcmp(tmp.big,  "100 100\n")){ 
		printf("szar file \n");
		printf("img size: %s\n", tmp.big);
		exit(ERROR);
	};
	fgets(tmp.big, 80, fptr);
	if(strcmp(tmp.big,  "255\n")){ 
		printf("szar file \n");
		printf("img maxrgb: %s\n", tmp.big);
		exit(ERROR);
	};
	printf("file good\n");
	fclose(fptr);
	fptr = fopen(PATH, "rb");
	img100* outprt=(img100*)  malloc( sizeof(img100)); //alloc memory for the rgb map
	printf("allocated %lu mem at: %p\n",sizeof(img100), outprt);
	for(int x =0; x<100;x++){ //init memory
		for(int y =0;y<100;y++){
			for(int rgb =0; rgb<3;rgb++){
				(*outprt)[x][y][rgb] = 0;
			};
		};

	};


	int readbuffer[300]; //read buffer
	fseek(fptr, 61, SEEK_SET); //todo: make this not only accept files from gimp
	for(int x =0;x<100 ;x++){
	size_t readbytes = fread(
		readbuffer, 
			sizeof(unsigned char),
			300,
			fptr
		);
		printf("readbytes: %zu\n", readbytes);
		int bufferID = 0;
		for(int y = 0; y<100;y++){
			for(int rgb = 0; rgb<3; rgb++){
				(*outprt)[x][y][rgb] =  readbuffer[bufferID]; //makeing it int
				bufferID++;
			};
		};
	};
	fclose(fptr);
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
	img100* hpImg =loadimg100("./img/hp.ppm"); // másodjára szarul fut le valamiért
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
	

	free(hpImg);
	free(playerImg);
	//unload font
	XUnloadFont(d, fontId);
	//close connection&destroy window
	XDestroyWindow(d, Win);
	XCloseDisplay(d);
	return GOOD;
};
