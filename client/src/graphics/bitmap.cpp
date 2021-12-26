#include "Bitmap.h"
#include <memory>

Bitmap::Bitmap()
{

}

Bitmap::~Bitmap()
{
}

bool Bitmap::CreateBitmap(std::string path, int w, int h, char* pixels, int colorBitCount){
	
	BitmapFile file;
	memset(&file, 0, sizeof(BitmapFile));

	int lineSize = (w * colorBitCount + 31) / 32 * (colorBitCount / 8);
	int dataSize = h * lineSize;
	int headSize = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
	int patSize = colorBitCount == 8 ? BITMAPFILE_PAL_SIZE * sizeof(uint32) : 0;

	file.infoHeader.biSize = sizeof(BitmapInfoHeader);
	file.infoHeader.biPlanes = 1;
	file.infoHeader.biCompression = 0;
	file.infoHeader.biWidth = w;
	file.infoHeader.biHeight = h;
	file.infoHeader.biBitCount = colorBitCount;
	file.infoHeader.biSizeImage = dataSize;
	file.infoHeader.biClrUsed = colorBitCount == 8 ? 256 : 0;
	file.infoHeader.biClrImportant = file.infoHeader.biClrUsed;

	file.header.bfType = BITMAPFILE_ID;
	file.header.bfSize = headSize + patSize + dataSize;
	file.header.bfOffBits = (headSize + patSize + 3) / 4 * 4;

	memset(file.palettes, 0xff, sizeof(file.palettes));
	file.imageData = new uint8[file.infoHeader.biSizeImage];
	memset(file.imageData, 0xff, file.infoHeader.biSizeImage);

	int pixbytes = colorBitCount / 8;
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			int dst = i * lineSize + j * pixbytes;
			for (int p = 0; p < pixbytes; p++)
			{
				file.imageData[dst + p] = pixels[dst + p];
			}
		}
	}
	bool ok = Save(path, file);
	delete file.imageData;
	file.imageData = nullptr;

	return ok;
}

//argb
bool Bitmap::CreateBitmapFile(std::string path, int w, int h, uint32 color = 0xffffffff, int colorBitCount = 32)
{

	BitmapFile file;
	memset(&file, 0, sizeof(BitmapFile));

	int lineSize = (w*colorBitCount + 31) / 32 * 4;
	int dataSize = h * lineSize;
	int headSize = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
	int patSize = colorBitCount == 8 ? BITMAPFILE_PAL_SIZE * sizeof(uint32) : 0;

	file.infoHeader.biSize = sizeof(BitmapInfoHeader);
	file.infoHeader.biPlanes = 1;
	file.infoHeader.biCompression = 0;
	file.infoHeader.biWidth = w;
	file.infoHeader.biHeight = h;
	file.infoHeader.biBitCount = colorBitCount;
	file.infoHeader.biSizeImage = dataSize;
	file.infoHeader.biClrUsed = colorBitCount == 8 ? 256 : 0;
	file.infoHeader.biClrImportant = file.infoHeader.biClrUsed;

	file.header.bfType = BITMAPFILE_ID;
	file.header.bfSize = headSize + patSize + dataSize;
	file.header.bfOffBits = (headSize + patSize + 3) / 4 * 4;

	memset(file.palettes, 0xff, sizeof(file.palettes));
	file.imageData = new uint8[file.infoHeader.biSizeImage];
	memset(file.imageData, 0xff, file.infoHeader.biSizeImage);

	int pixbytes = colorBitCount / 8;
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			int dst = i * lineSize + j * pixbytes;
			uint32 tmpc = color;
			for (int p = 0; p < pixbytes; p++)
			{
				file.imageData[dst + p] = (tmpc & 0xff);
				tmpc = tmpc >> 8;
			}
		}
	}

	bool ok = Save(path, file);
	delete file.imageData;
	file.imageData = nullptr;

	return ok;
}


bool Bitmap::Load(std::string path, BitmapFile& bitmapFile)
{
	using std::cout;
	using std::endl;

	std::fstream f(path, std::ios::in | std::ios::binary);
	if (f.rdstate() == f.failbit)
	{
		cout << "open file error!" << endl;
		return false;
	}

	f.read((char*)&bitmapFile.header, sizeof(BitmapFileHeader));
	if (bitmapFile.header.bfType != BITMAPFILE_ID)
	{
		f.close();
		return false;
	}

	f.read((char*)&bitmapFile.infoHeader, sizeof(BitmapInfoHeader));

	if (bitmapFile.infoHeader.biBitCount == 8)
	{
		memset(bitmapFile.palettes, 0, sizeof(bitmapFile.palettes));
		f.read((char*)bitmapFile.palettes, BITMAPFILE_PAL_SIZE * sizeof(uint32));
	}

	f.seekg(bitmapFile.header.bfOffBits, std::ios::beg);
	bitmapFile.imageData = new uint8[bitmapFile.infoHeader.biSizeImage];
	memset(bitmapFile.imageData, 0, bitmapFile.infoHeader.biSizeImage);
	f.read((char*)bitmapFile.imageData, bitmapFile.infoHeader.biSizeImage);
	f.close();

	return true;
}

bool Bitmap::Save(std::string path, const BitmapFile& file)
{
	std::fstream f(path, std::ios::out | std::ios::binary);
	if (f.rdstate() == f.failbit)
	{
		std::cout << "open file error!" << std::endl;
		return false;
	}

	f.write((char*)&file.header, sizeof(BitmapFileHeader));
	f.write((char*)&file.infoHeader, sizeof(BitmapInfoHeader));

	if (file.infoHeader.biBitCount == 8)
	{
		f.write((char*)file.palettes, BITMAPFILE_PAL_SIZE * sizeof(uint32));
	}

	f.seekp(file.header.bfOffBits, std::ios::beg);
	f.write((char*)file.imageData, file.infoHeader.biSizeImage);
	f.close();
	return true;
}

