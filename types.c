typedef unsigned char img100[100][100][3]; //rgb 
struct ppmHeader {
	bool type; //0 RAW_PPM / 1 ASCII_PPM
	int sizeX, sizeY, headerLen, bitdepth;
};
struct ppmImage {
	unsigned char *data;
	unsigned int high, width, bitdepth;
};
