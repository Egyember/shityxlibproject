#include <X11/X.h>
#include <stdio.h>
#include <X11/Xlib.h> //main Xlib header
#include <X11/Xcms.h> //color stuff
#include <unistd.h> //sleep

#define GOOD 0
#define ERROR -1



int stringLen(char* str){
	int len = 0;
	while (*str != '\0') {
		str++;
		len++;
	}
	//printf("len: %d",len);
	return len;
};


int main(){
	printf("hello word!\n");
	//connect to the x server ($DISPLAY env)
	Display *d;
	d = XOpenDisplay(NULL);
	if (d==NULL){
		printf("We are fucked");
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
	Window Win = XCreateSimpleWindow(d, RootWindow(d, ds), 2, 1, 300, 300, 1, WhitePixel(d, ds), WhitePixel(d, ds));
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
	char **fontlist = XListFonts(d, "*", 10, &numberOfFonts);
	if(numberOfFonts==0){
		printf("missing font\n");
		return ERROR;
	};	
	printf("printing fonts \n");
	printf("%d\n", numberOfFonts);
	for(int i;numberOfFonts>i;i++){
		printf("font[%d]%s \n", i,fontlist[i]);
	};
	sleep(1);

	//load font
	Font fontId = XLoadFont(d, fontlist[0]);
	if (fontId == BadName || fontId == BadAlloc) { printf("bad name/BadAlloc \n");};
	
	// get XFontStruct
	XFontStruct *fontStruct = XQueryFont(d, fontId);

	/*
	//testing fontid
	if (fontId == fontStruct->fid){printf("good\n");};
	*/

	//stirng width
	char testString[] = "árvíztürő tűkörfúrógép";
	int testStringWidth;
	testStringWidth = XTextWidth(fontStruct, testString, stringLen(testString));
	printf("A testString %d px széles\n", testStringWidth);
	
	//drawing string
		//set font for GCs
	XSetFont(d, gcRed, fontId);
	XSetFont(d, gcBlack, fontId);
		//start drawing
	XDrawString(d, Win, gcBlack, 10, 10, testString, stringLen(testString));
		//flush Xbuffer
	XFlush(d);
	sleep(10);


	//unload font
	XUnloadFont(d, fontId);
	//close connection&destroy window
	XDestroyWindow(d, Win);
	XCloseDisplay(d);
	return GOOD;
};
