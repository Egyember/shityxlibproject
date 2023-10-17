typedef int *line[3];

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
	int x,y;
	unsigned int renderDistence, ID;
	enum entityType type;
	struct ppmImage texture;
	bool hasPhysics;
	int vector[2];
	bool hasCollision;
	int hitBox[4][2];// 4 corner of the rectangle (x,y)
	struct entity* nextEntity; //pointer to the next entity
};
