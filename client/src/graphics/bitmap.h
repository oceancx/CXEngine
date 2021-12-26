#pragma once


#include "define_types.h"

#define BITMAPFILE_ID  0x4D42
#define BITMAPFILE_PAL_SIZE  256

#pragma pack(push) 
#pragma pack(1)   

struct BitmapFileHeader		//size = 14
{
	uint16 bfType;			//must be 0x4D42
	uint32 bfSize;			//fileSize = header + infoHeader + pal + imageData
	uint16 bfReserved1;
	uint16 bfReserved2;
	uint32 bfOffBits;		//imageData offset 
};


struct BitmapInfoHeader				//size = 40
{
	uint32		biSize;				//40
	uint32      biWidth;
	uint32      biHeight;
	uint16      biPlanes;			//must be 1
	uint16      biBitCount;			//8	16	24	32
	uint32      biCompression;		//BI_RBG
	uint32      biSizeImage;		//biWidth*biHeight*biBitCount/8
	uint32      biXPelsPerMeter;
	uint32      biYPelsPerMeter;
	uint32      biClrUsed;			//can be 256
	uint32      biClrImportant;		//can be 256
};

struct BitmapFile
{
	BitmapFileHeader header;
	BitmapInfoHeader infoHeader;
	uint32 palettes[256];		//size = 1024	rgba
	uint8* imageData;
};
#pragma pack(pop)

class Bitmap

{
public:
	Bitmap();
	~Bitmap();
	static bool CreateBitmap(std::string path, int w, int h, char* pixels, int colorBitCount);
	static bool CreateBitmapFile(std::string path, int w, int h, uint32 color, int colorBitCount);

	static bool Load(std::string path, BitmapFile& bitmapFile);
	static bool Save(std::string path, const BitmapFile& bitmapFile);
	

};

