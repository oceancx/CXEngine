#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <map>
#include <memory>
#include <set>

namespace NE {

	enum EFileType
	{
		FILE_TYPE_UNKNOWN = 0,
		FILE_TYPE_SPRITE,
		FILE_TYPE_OGGS,
		FILE_TYPE_MP3,
		FILE_TYPE_RIFF,
		FILE_TYPE_TEXT
	};

	using PalMat = std::vector<uint16_t>;
	using PalMatVec = std::vector<PalMat>;

	struct PalSchemePart
	{
		uint16_t from;
		uint16_t to;
		PalMat mat;
	};

	struct PalMatRef
	{
		uint8_t seg;
		uint8_t mat_i;
	};

	struct PalSegment
	{
		uint8_t from;
		uint8_t to;
		PalMatVec mat;
	};

	struct PalSchemes
	{
		uint32_t pack;
		uint32_t wasid;
		std::vector<PalSegment> schemes;
	};

	int check_file_type(char* data,size_t size);

	struct Sprite
	{
		struct Sequence
		{
			int KeyX;
			int KeyY;
			int Width;
			int Height;
			uint32_t Format;

			std::vector<uint32_t> Src;
			bool IsBlank;
		};

		int PalIndex;
		std::string PalID;
		std::string ID;
		std::string Path;
		std::vector<Sequence> Frames;
		void SaveImage(const char* filename,int index);
		uint32_t FrameWASOffset;
		bool FrameLoaded;

		int GroupFrameCount;
		int GroupCount;
		int Width;
		int Height;
		int KeyX;
		int KeyY;

	};


	class WAS
	{
	public:

		struct Header
		{
			uint16_t Flag;
			uint16_t Len;
			int16_t GroupCount;
			int16_t GroupFrameCount;
			int16_t Width;
			int16_t Height;
			int16_t KeyX;
			int16_t KeyY;
		};

		struct FrameHeader
		{
			int32_t KeyX;
			int32_t KeyY;
			int32_t Width;
			int32_t Height;
		};

		WAS(std::string filename);

		WAS(std::string path, uint32_t offset);

		void Decode();

		~WAS();

		Header mHeader;


		std::vector<uint32_t> mFrameIndecies;

		std::string mPath;

		std::vector<uint8_t> m_FileData;
		std::uint64_t m_FileSize;
		size_t m_FileOffset = 0;
		uint16_t m_Palette16[256];
		uint32_t m_Palette32[256];
	};

	
	class WDF
	{
	public:
		struct Header
		{
			//0x57444650:WDFP	0x57444658:WDFX		0x57444648:WDFH
			uint32_t flag;

			uint32_t number;

			uint32_t offset;
		};

		struct Index
		{
			uint32_t hash;
			uint32_t offset;
			uint32_t size;
			uint32_t spaces;
		};

		struct PalMatrix
		{
			uint16_t from;
			uint16_t to;
			std::vector<uint16_t> mat;
		};


	public:
		WDF(std::string path);
		~WDF();

		void DataHandler(uint8_t *pData, uint32_t* pBmpStart, int pixelOffset, int pixelLen, int y, bool& copyline);

		WAS GetWAS(uint32_t id);
		void SaveWAS(uint32_t id, const char* path);

		Sprite* LoadSprite(uint32_t id, std::vector<PalSchemePart>* patMatrix =nullptr );
		void UnLoadSprite(uint32_t id);

		Sprite* LoadSpriteHeader(uint32_t id, std::vector<PalSchemePart>* patMatrix = nullptr);
		bool LoadSpriteData(Sprite* sprite, std::vector<PalSchemePart>* patMatrix = nullptr);

		std::vector<uint32_t> GetAllWASIDs()
		{
			std::vector<uint32_t> ids;
			for (uint32_t i = 0; i < mIndencies.size(); i++)
			{
				ids.push_back(mIndencies[i].hash);
			}
			return ids;
		}

		std::vector<Sprite *> LoadAllSprite();

		void LoadFileData(uint32_t id, uint8_t*& pData, size_t& size);
		

		Sprite* UnpackSprite(uint32_t id, std::vector<PalSchemePart> pal);
		/*void UnpackOggs(uint32_t id =0) {};
		void UnpackRIFF(uint32_t id = 0) {};
		void UnpackText(uint32_t id = 0) {};*/
	public:
		std::vector<Index> mIndencies;
		std::map<uint32_t, uint32_t> mIdToPos;

		uint16_t m_Palette16[256];
		uint32_t m_Palette32[256];
		std::string m_Path;
		std::string m_WDFDir;
		std::string m_FileName;
		uint32_t m_FileDataOffset;
		uint32_t m_WASNumber;
		uint32_t m_FileType;

		std::vector<uint8_t> m_FileData;
		std::uint64_t m_FileSize;

		std::map<uint32_t, Sprite> m_Sprites;
		std::map<uint32_t, bool> m_SpritesLoading;
		std::map<uint32_t, bool> m_SpritesLoaded;

	};

	class MAP
	{
	public:
		struct MapHeader
		{
			uint32_t		Flag;
			uint32_t		Width;
			uint32_t		Height;
		};

		struct MapUnitHeader
		{
			uint32_t		Flag;
			uint32_t		Size;
		};

		struct MaskHeader
		{
			uint32_t	Flag;
			int	Size;
			MaskHeader()
				:Flag(0),
				Size(0)
			{

			}
		};

		struct BaseMaskInfo
		{
			int	StartX;
			int	StartY;
			uint32_t	Width;
			uint32_t	Height;
			uint32_t	Size;
			BaseMaskInfo()
				:StartX(0),
				StartY(0),
				Width(0),
				Height(0),
				Size(0)
			{

			}
		};

		struct MaskInfo : BaseMaskInfo
		{
			std::vector<uint32_t> Data;
			std::set<int> OccupyUnits;
			bool bHasLoad = false;
			bool bLoading = false;
		};

		struct MapUnit
		{
			std::vector<uint8_t>  Cell;
			std::vector<uint8_t> JPEGRGB24;
			uint32_t Size;
			uint32_t Index;
			bool bHasLoad = false;
			bool bLoading = false;
			uint32_t JpegSize;
			uint32_t JpegOffset;
			std::set<int> OwnMasks;
		};

		MAP(std::string filename);

		~MAP();

		void DecodeMapUnits();
		void DecodeMapMasks();

		void ReadUnit(int index);

		void ReadUnit(int row, int col) { ReadUnit(row*m_ColCount + col); };

		void SaveUnit(int index);

		void ReadMask(int index);

		void PrintCellMap();

		void SaveImageFile(const char* filename, int width, int height, int pixelDepth, char* data);

		int MapWidth() { return m_MapWidth; };
		int MapHeight() { return m_MapHeight; };
		int SliceWidth() { return m_Width; };
		int SliceHeight() { return m_Height; };
		int Row() { return m_RowCount; };
		int Col() { return m_ColCount; };
		int UnitSize() { return m_UnitSize; };
		int MaskSize() { return m_MaskSize; };

		int GetMaskWidth(int index) { return m_MaskInfos[index].Width; };
		int GetMaskHeight(int index) { return m_MaskInfos[index].Height; };

		MaskInfo& GetMask(int index) { return m_MaskInfos[index]; };
		MapUnit& GetUnit(int index) { return m_MapUnits[index]; };
		bool HasUnitLoad(int index) { return m_MapUnits[index].bHasLoad; };
		bool IsUnitLoading(int index) { return m_MapUnits[index].bLoading; };

		uint32_t* GetMaskBitmap(int index) { return m_MaskInfos[index].Data.data(); };
		uint8_t* GetUnitBitmap(int index) { return m_MapUnits[index].JPEGRGB24.data(); };
		size_t GetUnitBitmapSize(int index) { return m_MapUnits[index].JPEGRGB24.size(); };
	private:

		void ByteSwap(uint16_t& value);

		size_t DecompressMask(void* in, void* out);

		void MapHandler(uint8_t* Buffer, uint32_t inSize, uint8_t* outBuffer, uint32_t* outSize);

		bool ReadJPEG(uint32_t& offset, uint32_t size, uint32_t index);

		bool ReadCELL(uint32_t& offset, uint32_t size, uint32_t index);

		bool ReadBRIG(uint32_t& offset, uint32_t size);


		std::string m_FileName;

		int m_Width;

		int m_Height;

		int m_MapWidth;

		int m_MapHeight;

		int m_BlockWidth;

		int m_BlockHeight;

		uint32_t m_RowCount;

		uint32_t m_ColCount;

		MapHeader m_Header;

		std::vector<uint32_t> m_UnitIndecies;

		uint32_t m_UnitSize;

		MaskHeader m_MaskHeader;

		std::vector<uint32_t> m_MaskIndecies;

		uint32_t m_MaskSize;

		std::vector<MapUnit> m_MapUnits;

		std::vector<MaskInfo> m_MaskInfos;

		std::vector<uint8_t> m_FileData;

		std::uint64_t m_FileSize;

		std::vector<std::vector<uint8_t>> m_CellData;
	};

	uint32_t RGB565to888(uint16_t color, uint8_t alpha);
	uint8_t MixAlpha(uint8_t color, uint8_t alpha);
	uint16_t ChangeColorPal(uint16_t color, std::vector<uint16_t> c);
	uint16_t Alpha565(uint16_t Src, uint16_t Des, uint8_t Alpha);
}
using Sprite = NE::Sprite;