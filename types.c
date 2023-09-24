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

struct entity {
	int x,y;
	unsigned int renderDistence, ID;
	enum entityType type;
	struct ppmImage texture;
	bool hasPhysics;
	int vector[2];
	bool hasCollision;
	int hitBox[4];// from x,y cordinate x1,y1,x2,y2 
	struct entity* nextEntity; //point to the next entity
};
