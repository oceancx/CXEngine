#pragma once

#include <NESupport.h>
#include "astar/heap.h"
#include "astar/astar.h"

#include "pos.h"
#ifndef SIMPLE_SERVER
#include "graphics/image.h"
#include "graphics/texture.h"
#endif // SIMPLE_SERVER


/*
游戏地图类的封装
*/
class GameMap  
{
public:
	GameMap(uint32 mapID);
	
	~GameMap();

	std::list<Pos> Move(int sx, int sy, int ex, int ey);

	NE::MAP* GetMapPtr();

	void clamp(int val, int min, int max);

	Astar* GetAstar() { return m_Astar; };

	int GetRowCount() { return m_Row; };

	int GetColCount() { return m_Col; };

	IntPos GetRandomPos();

	void Update();

	int GetWidth(){return m_Width;}

	int GetHeight(){return m_Height;}

	int GetMapWidth() { return m_MapWidth; }

	int GetMapHeight() { return m_MapHeight; }

	int GetMapOffsetX();

	int GetMapOffsetY();

	void ResetMapOffset();
#ifndef SIMPLE_SERVER
	void Draw(int playerX, int playerY);
	void DrawMask(int playerX, int playerY, int playerHeight);
	void DrawCell(int cur_x, int cur_y);
	void DrawCell();
#endif
private:
	bool CanArriveDirect(Pos src, Pos dest);

	void UpdateCell();

	int m_Width;

	int m_Height;

	int m_MapWidth;

	int m_MapHeight;

	int m_CellWidth;

	int m_CellHeight;

	/*
	障碍信息
	*/
	std::vector<std::vector<int>> m_Cell;
	
	int m_MapTileWidth;
	
	int m_MapTileHeight;
	
	int m_MapTileCoef;
	
	int m_Row;
	
	int m_Col;
	
	NE::MAP* m_XyqMap;
	
	Astar* m_Astar;
	
	int m_MapID;
	
	int m_MapOffsetX;
	
	int m_MapOffsetY;
	
	std::map<int, std::set<int>> m_UnitMasks;

#ifndef SIMPLE_SERVER
	/*
	整张地图
	*/
	std::map<int, Texture *> m_MapTiles;

	/*
	整张地图的遮罩
	*/
	std::map<int, Texture *> m_MaskTiles;

	/*
	障碍Icon
	*/
	Texture* m_CellPic;
#endif // SIMPLE_SERVER
};


