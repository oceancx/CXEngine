//
//  image.h
//  mhxy
//
//  Created by oceancx on 16/8/15.
//  Copyright (c) 2016年 oceancx. All rights reserved.
//
#pragma once
#include "define_types.h"

#pragma pack(2)
typedef struct tagBITMAP_FILEHEADER{
    uint16 bfType;
    uint32 bfSize;
    uint16 bfReser;
    uint16 bfReserv;
	uint32 bfOffBits;
}BITMAP_FILEHEADER;

typedef struct tagBITMAP_INFOHEADER{
    uint32 biSize;
    int biWidth;
    int biHeight;
	uint16 biPlanes;
	uint16 biBitCount;
	uint32 biCompression;
	uint32 biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
	uint32 biClrUsed;
	uint32 biClrImportant;
}BITMAP_INFOHEADER;

typedef struct tag_RGBQUAD{
	uint8 rgbBlue;
	uint8 rgbGreen;
	uint8 rgbRed;
	uint8 rgbReserved;
}RGB_QUAD;

typedef struct tag_BITMAP_INFO{
    BITMAP_INFOHEADER bmiHeader;
    RGB_QUAD bmiColors[1];
}BITMAP_INFO;

typedef struct BITMAP_FILE_TAG{
    BITMAP_FILEHEADER bitmapfileheader;
    BITMAP_INFOHEADER bitmapinfoheader;
    RGB_QUAD palette[256];
    uint8 *buffer;
}BITMAP_FILE,*BITMAP_FILE_PTR;
#pragma pack()


