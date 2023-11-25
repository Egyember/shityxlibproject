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
#include "ppmstuff.c"

#endif

int stringLen(const char* str){
	//local wraper needed because i had no idea tha this thing existed when i wrote the rest of the file
	return strlen(str);
};

void gravity(struct entity* target){
	target->vector[0]+=GRAVITYFOCE;
};
void pointVectorToLine(const point point, const vector vector, line *returnPointer){
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
	(*returnPointer)[0] =  A;
	(*returnPointer)[1] =  B;
	(*returnPointer)[2] =  C;

};

void pointToPointVector(const point point1, const point point2, vector* returnVector){
	//calculate a vector from the first point to the secund vector and return to pointer to a vector
	(*returnVector)[0] = point1[0] - point2[0];
	(*returnVector)[1] = point1[1] - point2[1];
};

bool doVectorsParallel(const vector vector1,const vector vector2){
	//returne true if the vectors are parallel
	float Vec1fraction = vector1[0]/ (float) vector1[1];
	float Vec2fraction = vector2[0]/ (float) vector2[1];
	float Vec2fractionAlt = vector2[1]/ (float) vector2[0];
	if (Vec1fraction == Vec2fraction || Vec1fraction == Vec2fractionAlt){
		return true;
	};
	return false;

};

//todo: make it less confusing (remove unnecessary pointers)
void getCollisionTowLine(line *line1, line *line2, point *returnprt){
	//line1 = (a,b,c)
	//line2 = (d,e,f)
	//
	//ax-by=c
	//dx-ey=f
	//
	//y = (dc-af)/((-db)-(-ae))
	//x = (c+by)/a
	int y = (*line2[0] * *line1[2]-*line1[0] * *line2[2])/(((*line2[2] * *line1[1])*-1)- (-1*(*line1[0]* *line2[1])));
	int x = (*line1[2] + *line1[1]*y)/ *line1[0];
	(*returnprt)[0] = (int) x;
	(*returnprt)[1] = (int) y;
};

void updateHitBoxStruct(struct hitBox* hitBoxptr){
	//vectors
	pointToPointVector(hitBoxptr->hitBox[0], hitBoxptr->hitBox[1], &hitBoxptr->vectors[0]);
	pointToPointVector(hitBoxptr->hitBox[1], hitBoxptr->hitBox[2], &hitBoxptr->vectors[1]);
	pointToPointVector(hitBoxptr->hitBox[2], hitBoxptr->hitBox[3], &hitBoxptr->vectors[2]);
	pointToPointVector(hitBoxptr->hitBox[3], hitBoxptr->hitBox[0], &hitBoxptr->vectors[3]);

	//lines
	//nem jó mert a map origin-hoz képest lesz igy a vonal mert a pontott nem az entitytől számolom
	//todo: fix it
	//todo: finish it
	pointVectorToLine(hitBoxptr->hitBox[0], hitBoxptr->vectors[0], &(hitBoxptr->lines[0]));
	pointVectorToLine(hitBoxptr->hitBox[1], hitBoxptr->vectors[1], &(hitBoxptr->lines[1]));
	pointVectorToLine(hitBoxptr->hitBox[2], hitBoxptr->vectors[2], &(hitBoxptr->lines[2]));
	pointVectorToLine(hitBoxptr->hitBox[3], hitBoxptr->vectors[3], &(hitBoxptr->lines[3]));
	
};


void applyVector(struct entity* target){
	if(target->hasCollision == false){
		target->x += target->vector[0];
		target->y += target->vector[1];
		return;
	};
//todo: collision 
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
//todo load map data from a custom file format
//todo: render image from loaded ppm files (with the cpu)

	struct ppmImage playerImg =loadPpmImg("./img/player.ppm");
	struct ppmImage hpImg =loadPpmImg("./img/hp.ppm");
	

	XImage *testImg = XCreateImage(d, DefaultVisual(d, ds), DefaultDepth(d,DefaultScreen(d)) , ZPixmap, 0, playerImg.data, hpImg.width, hpImg.high, 32,0 );
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

/*	mainentity.firstNode->hitBox[0] = 10;
	mainentity.firstNode->hitBox[1] = 10;
	mainentity.firstNode->hitBox[2] = -10;
	mainentity.firstNode->hitBox[3] = -10;
*/
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
