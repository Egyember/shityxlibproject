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

#endif


typedef int *line[3];
//Ax - By = C
//(A;B;C)

typedef int vector[2];
//(y;x)

struct ppmHeader {
	bool type; //0 RAW_PPM / 1 ASCII_PPM
	int sizeX, sizeY, headerLen, bitdepth;
};
struct ppmImage {
	unsigned char *data;
	unsigned int high, width, bitdepth;
};

enum entityType{
	player,
	enemy,
	decor
};

struct entityStart{
	struct entity* firstNode;
	unsigned int totalNumber;

};

struct entity {
	int x,y, lastx, lasty;
	unsigned int renderDistence, ID;
	enum entityType type;
	struct ppmImage texture;
	bool hasPhysics;
	vector vector;
	bool hasCollision;
	struct entity* nextEntity; //pointer to the next entity
	struct hitBox* hitboxptr;
};

struct hitBox {
	int hitBox[4][2];// 4 corner of the rectangle (x,y) relative from the main cordinates of owner is null relative from map origine
	line lines[4];
	vector vectors[4];
	struct entity* owner;
	struct hitBox* nextHitBox;
};



