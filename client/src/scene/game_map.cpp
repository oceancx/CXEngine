#include "game_map.h"

#include "window.h"
#include <functional>
#include <thread>
#include "profile.h"
#include "file_loading.h"
#include "cxrandom.h"
#include "file_system.h"
#include "cxmath.h"

#ifndef SIMPLE_SERVER
#include "sprite_renderer.h"
#include <stb_image.h>
#endif
#include "logger.h"


GameMap::GameMap(uint32 mapID)
	:m_XyqMap(nullptr)
{
	m_MapID = mapID;

	std::string fileName = FileSystem::GetMapPath(std::to_string(mapID));

	m_Astar = nullptr;

	m_MapOffsetX = m_MapOffsetY = 0;
	auto* iothread = file_loading_thread();
	if (!iothread->IsFileLoadOk(fileName.c_str()))
	{
		iothread->PostTask(fileName.c_str(), [this](const char* path)->bool
		{
				m_XyqMap = new NE::MAP(path);

				m_MapWidth = m_XyqMap->MapWidth();

				m_MapHeight = m_XyqMap->MapHeight();

				m_Width = m_XyqMap->SliceWidth();
				m_Height = m_XyqMap->SliceHeight();

				m_Row = m_XyqMap->Row();
				m_Col = m_XyqMap->Col();
				cxlog_info("init GameMap %d %d ", m_Row, m_Col);

				m_CellWidth = m_Col * 16;
				m_CellHeight = m_Row * 12;
				m_Cell.resize(m_CellWidth, { 0 });

				for (int i = 0; i < m_CellWidth; i++)
				{
					m_Cell[i].resize(m_CellHeight, 0);
				}
				UpdateCell();
			return true;
		}
		);

		m_MapTileWidth = 320;
		m_MapTileHeight = 240;
		m_MapTileCoef = WINDOW_INSTANCE->GetWidth() / 2 / m_MapTileWidth + 1;

#ifndef SIMPLE_SERVER
		m_MapTiles.clear();

		m_MaskTiles.clear();
		m_CellPic = new Texture(FileSystem::GetAbsPath("res/icon/x.png"));
#endif
		
	}
}

void GameMap::clamp(int val, int min, int max)
{
	if (val < min) val = min;
	if (val > max) val = max;
}

NE::MAP* GameMap::GetMapPtr()
{
	return m_XyqMap;
}

bool GameMap::CanArriveDirect(Pos src, Pos dest)
{
	int dx = static_cast<int> (dest.x - src.x);
	int dy = static_cast<int>(dest.y - src.y);
	if (dx == 0)
	{
		int opt = dy < 0 ? -1 : 1;
		for (int i = 0; i<dy*opt; i++)
		{
			int cellX = static_cast<int>(src.x);
			int cellY = static_cast<int>(src.y + i * opt);
			if (m_Cell[cellX][cellY] >= 1)
				return false;
		}
		return true;
	}

	if (dy == 0)
	{
		int opt = dx < 0 ? -1 : 1;
		for (int i = 0; i<dx*opt; i++)
		{
			int cellX = static_cast<int>(src.x + i * opt);
			int cellY = static_cast<int>(src.y);
			if (m_Cell[cellX][cellY] >= 1)
				return false;
		}
		return true;
	}

	double ratio = dy / dx;
	int opt = dx < 0 ? -1 : 1;
	for (int i = 0; i < dx * opt; ++i)
	{
		int cellX = static_cast<int>(src.x + opt * i);
		int cellY = static_cast<int>(src.y + ratio * i*opt);
		if (m_Cell[cellX][cellY] >= 1)
			return false;
	}

	ratio = dx / dy;
	opt = dy < 0 ? -1 : 1;
	for (int i = 0; i < dy * opt; ++i)
	{
		int cellX = static_cast<int>(src.x + ratio * opt*i);
		int cellY = static_cast<int>(src.y + i * opt);
		if (m_Cell[cellX][cellY] >= 1)
			return false;
	}

	return true;

}
void GameMap::UpdateCell()
{
	int x = 0, y = 0, p, q;
	for (int i = 0; i < m_Row; i++)
	{
		for (int j = 0; j < m_Col; j++)
		{
			int index = i * m_Col + j;
			y = 12 * i;
			for (p = 0; p < 12; p++)
			{
				x = 16 * j;
				for (q = 0; q < 16; q++)
				{
					m_Cell[x++][y] = m_XyqMap->GetUnit(i*m_Col + j).Cell[p * 16 + q];
				}
				y++;
			}
		}
	}
	m_Astar = new Astar(m_MapWidth, m_MapHeight, m_Cell);
}

IntPos GameMap::GetRandomPos()
{
	IntPos dest;
	do
	{
		dest.x = Random::GetInstance()->NextInt(0, m_CellWidth - 1);
		dest.y = Random::GetInstance()->NextInt(0, m_CellHeight - 1);
	} while (m_Cell[dest.x][dest.y] >= 1);
	return dest;
}

std::list<Pos> GameMap::Move(int sx, int sy, int ex, int ey)
{
	if (!m_Astar)return{};
	//printf(" GameMap:sx %d sy %d ex %d ey %d\n", sx, sy, ex, ey);
	if (sx < 0 || sy < 0 || ex < 0 || ey < 0) return{};
	if (sx >= m_CellWidth || ex >= m_CellWidth || sy >= m_CellHeight || ey >= m_CellHeight) return{};
	if (sx == ex && sy == ey) return{};
	if (m_Cell[sx][sy] > 0 || m_Cell[ex][ey] > 0) return{};

	if (m_Astar->PathFinding(sx, sy, ex, ey))
	{
		auto path = m_Astar->GetMoveList();
		std::vector<Pos> moveList(path.begin(), path.end());

		// Smooth move List
		/*
		算法：
		1.第一个节点为当前节点，查看下一个节点
		2.如果可以无障碍直接到达，则下一个节点为第一个节点的后继
		3.如果不可到达，当前节点为后继节点
		*/
		int currentNode = 0;
		int nextNode = 1;
		std::list<Pos> smoothMoveList;

		if (moveList.size()>0)
		{
			smoothMoveList.push_back(moveList[currentNode]);
			while (nextNode != moveList.size())
			{
				int lastNextNode = nextNode;

				while (nextNode != moveList.size() && CanArriveDirect(moveList[currentNode], moveList[nextNode]))
				{
					lastNextNode = nextNode;
					nextNode++;
				}
				currentNode = lastNextNode;
				smoothMoveList.push_back(moveList[currentNode]);
			}
		}
		return smoothMoveList;
	}
	else {
		return{};
	}
}

GameMap::~GameMap()
{
	auto* iothread = file_loading_thread();
	if (iothread)
	{
		std::string fileName = FileSystem::GetMapPath(std::to_string(m_MapID));
		iothread->RemoveTaskState(fileName.c_str());
	}

#ifndef SIMPLE_SERVER
	for (auto& it : m_MapTiles)
	{
		std::string unitpath(std::to_string(m_MapID));
		unitpath.append("/");
		unitpath.append(std::to_string(it.first));
		if (iothread)iothread->RemoveTaskState(unitpath.c_str());
		delete it.second;
}
	m_MapTiles.clear();

	for (auto& it : m_MaskTiles)
	{
		std::string maskpath(std::to_string(m_MapID));
		maskpath.append("/mask/");
		maskpath.append(std::to_string(it.first));
		if (iothread)iothread->RemoveTaskState(maskpath.c_str());
		delete it.second;
	}
	m_MaskTiles.clear();
#endif

	

	if (m_Astar != nullptr)
	{
		delete m_Astar;
		m_Astar = nullptr;
	}

	if (m_XyqMap != nullptr) {
		delete m_XyqMap;
		m_XyqMap = nullptr;
	}
}

void GameMap::Update()
{
	auto* iothread = file_loading_thread();
	std::string fileName = FileSystem::GetMapPath(std::to_string(m_MapID));
	if (!iothread->IsFileLoadOk(fileName.c_str()))return;
}
int GameMap::GetMapOffsetX()
{
	return m_MapOffsetX;
}
int GameMap::GetMapOffsetY()
{
	return m_MapOffsetY;
}

void GameMap::ResetMapOffset()
{
	m_MapOffsetX = 0;
	m_MapOffsetY = 0;
}


#ifndef SIMPLE_SERVER
void GameMap::Draw(int playerX, int playerY)
{
	auto* iothread = file_loading_thread();
	std::string fileName = FileSystem::GetMapPath(std::to_string(m_MapID));
	if (!iothread->IsFileLoadOk(fileName.c_str()))return;

	SpriteRenderer* renderer = SPRITE_RENDERER_INSTANCE;
	int screenWidth = WINDOW_INSTANCE->GetWidth();
	int screenHeight = WINDOW_INSTANCE->GetHeight();
	int halfScreenWidth = screenWidth / 2;
	int halfScreenHeight = screenHeight / 2;

	int mapOffsetX = halfScreenWidth - playerX;
	int mapOffsetY = halfScreenHeight - playerY;

	mapOffsetX = GMath::Clamp(mapOffsetX, -m_Width + screenWidth, 0);
	mapOffsetY = GMath::Clamp(mapOffsetY, -m_Height + screenHeight, 0);
	m_MapOffsetX = mapOffsetX;
	m_MapOffsetY = mapOffsetY;

	int startRow, endRow, startCol, endCol;
	startRow = playerY / m_MapTileHeight - 3;
	startRow = startRow < 0 ? 0 : startRow;
	endRow = playerY / m_MapTileHeight + 3;
	endRow = endRow > m_Row ? m_Row : endRow;

	startCol = playerX / m_MapTileWidth - 3;
	startCol = startCol < 0 ? 0 : startCol;
	endCol = playerX / m_MapTileWidth + 3;
	endCol = endCol > m_Col ? m_Col : endCol;


	for (int i = startRow; i<endRow; i++) {
		for (int j = startCol; j<endCol; j++) {
			int unit = i * m_Col + j;

			if (m_MapTiles.find(unit) == m_MapTiles.end())
			{
				std::string unitpath(std::to_string(m_MapID));
				unitpath.append("/");
				unitpath.append(std::to_string(unit));
				if (iothread->IsFileLoadOk(unitpath.c_str()))
				{
					if (m_MapTiles.find(unit) == m_MapTiles.end())
					{
						int width = 0, height = 0;
						uint8_t* imgBuffer = stbi_load_from_memory(m_XyqMap->GetUnitBitmap(unit), (int)m_XyqMap->GetUnitBitmapSize(unit),
							&width, &height, 0, 3);
						if (imgBuffer == NULL) {
							printf("Failed to load %s\n", stbi_failure_reason());
							return ;
						}
						m_MapTiles[unit] = new Texture(m_MapTileWidth, m_MapTileHeight, false, imgBuffer);
						stbi_image_free(imgBuffer);
					}

				}
				else
				{
					iothread->PostTask(unitpath.c_str(), [this, unit](const char* path)->bool {
						m_XyqMap->ReadUnit(unit);
						return true;
					});
				}
			}
			else
			{
				renderer->DrawSprite(m_MapTiles[i*m_Col + j],
					glm::vec2(j * m_MapTileWidth + mapOffsetX, i * m_MapTileHeight + mapOffsetY),
					glm::vec2(m_MapTileWidth, m_MapTileHeight),
					0.0f,
					glm::vec3(1.0f, 1.0f, 1.0f)
				);
			}

		}
	}
}


void GameMap::DrawCell(int cur_x, int cur_y)
{
	SpriteRenderer* renderer = SPRITE_RENDERER_INSTANCE;
	int minx, maxx, miny, maxy;
	int posx, posy;
	int screenWidth = WINDOW_INSTANCE->GetWidth();
	int screenHeight = WINDOW_INSTANCE->GetHeight();
	int halfScreenWidth = screenWidth / 2;
	int halfScreenHeight = screenHeight / 2;

	posx = -cur_x + halfScreenWidth;
	posy = -cur_y + halfScreenHeight;
	minx = posx - (halfScreenWidth + 10);
	minx = minx<0 ? 0 : minx;
	maxx = posx + (halfScreenWidth + 10);
	maxx = maxx>m_MapWidth ? m_MapWidth : maxx;

	miny = posy - (halfScreenHeight + 10);
	miny = miny<0 ? 0 : miny;
	maxy = posy + (halfScreenHeight + 10);
	maxy = maxy>m_MapHeight ? m_MapHeight : maxy;

	//for (int y = 0; y < m_CellHeight; y++)
	for (int y = miny / 20; y < maxy / 20; y++)
	{
		//for (int x = 0; x < m_CellWidth; x++)
		for (int x = minx / 20; x < maxx / 20; x++)
		{
			if (m_Cell[x][y] >= 1)
			{
				renderer->DrawSprite(m_CellPic,
					glm::vec2(x * 20 + cur_x,
						y * 20 + cur_y),
					glm::vec2(20, 20),
					0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
			}
		}
	}
}

void GameMap::DrawCell()
{
	DrawCell(m_MapOffsetX, m_MapOffsetY);
}

void GameMap::DrawMask(int playerX, int playerY, int drawY)
{
	auto* iothread = file_loading_thread();
	std::string fileName = FileSystem::GetMapPath(std::to_string(m_MapID));
	if (!iothread->IsFileLoadOk(fileName.c_str()))return;

	int screenWidth = WINDOW_INSTANCE->GetWidth();
	int screenHeight = WINDOW_INSTANCE->GetHeight();
	int halfScreenWidth = screenWidth / 2;
	int halfScreenHeight = screenHeight / 2;

	int mapOffsetX = halfScreenWidth - playerX;
	int mapOffsetY = halfScreenHeight - playerY;

	mapOffsetX = GMath::Clamp(mapOffsetX, -m_Width + screenWidth, 0);
	mapOffsetY = GMath::Clamp(mapOffsetY, -m_Height + screenHeight, 0);

	int startRow, endRow, startCol, endCol;
	startRow = GMath::Clamp(playerY / m_MapTileHeight - m_MapTileCoef, 0, m_Row);
	endRow = GMath::Clamp(playerY / m_MapTileHeight + m_MapTileCoef, 0, m_Row);
	startCol = GMath::Clamp(playerX / m_MapTileWidth - m_MapTileCoef, 0, m_Col);
	endCol = GMath::Clamp(playerX / m_MapTileWidth + m_MapTileCoef, 0, m_Col);


	std::set<int> drawMasks;
	for (int i = startRow; i < endRow; i++)
	{
		for (int j = startCol; j < endCol; j++)
		{
			int unit = i * m_Col + j;
			auto& info = m_XyqMap->GetUnit(unit);
			drawMasks.insert(info.OwnMasks.begin(), info.OwnMasks.end());
		}
	}

	for (auto m : drawMasks)
	{
		std::string maskpath(std::to_string(m_MapID));
		maskpath.append("/mask/");
		maskpath.append(std::to_string(m));

		if (m_MaskTiles.find(m) == m_MaskTiles.end())
		{
			if (iothread->IsFileLoadOk(maskpath.c_str()))
			{
				if (m_MaskTiles.find(m) == m_MaskTiles.end())
				{
					m_MaskTiles[m] = new Texture(m_XyqMap->GetMaskWidth(m),
						m_XyqMap->GetMaskHeight(m), true, (uint8*)m_XyqMap->GetMaskBitmap(m));
				}
			}
			else
			{
				iothread->PostTask(maskpath.c_str(), [this, m](const char* path)->bool {
					m_XyqMap->ReadMask(m);
					return true;
				});
			}
		}
		else
		{
			auto& info = m_XyqMap->GetMask(m);
			int x = info.StartX;
			int y = info.StartY;
			int w = info.Width;
			int h = info.Height;
			if (y + h > playerY + 20)
				SPRITE_RENDERER_INSTANCE->DrawMask(
					m_MaskTiles[m],
					glm::vec2(x + mapOffsetX, y + mapOffsetY),
					glm::vec2(w, h),
					0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

		}
	}

	for (int i = startRow; i < endRow; i++)
	{
		for (int j = startCol; j < endCol; j++)
		{
			int unit = i * m_Col + j;
			if (m_MapTiles.find(unit) == m_MapTiles.end())
			{
				std::string unitpath(std::to_string(m_MapID));
				unitpath.append("/");
				unitpath.append(std::to_string(unit));
				if (iothread->IsFileLoadOk(unitpath.c_str()))
				{
					if (m_MapTiles.find(unit) == m_MapTiles.end())
					{
						int width = 0, height = 0;
						uint8_t* imgBuffer = stbi_load_from_memory(m_XyqMap->GetUnitBitmap(unit), (int)m_XyqMap->GetUnitBitmapSize(unit),
							&width, &height, 0, 3);
						if (imgBuffer == NULL) {
							printf("Failed to load %s\n", stbi_failure_reason());
							return;
						}
						m_MapTiles[unit] = new Texture(m_MapTileWidth, m_MapTileHeight, false, imgBuffer);
						stbi_image_free(imgBuffer);
					}
				}
				else
				{
					iothread->PostTask(unitpath.c_str(), [this, unit](const char* path)->bool {
						m_XyqMap->ReadUnit(unit);
						return true;
					});
				}
			}
			else
			{
				SPRITE_RENDERER_INSTANCE->DrawMapSprite(m_MapTiles[i*m_Col + j],
					glm::vec2(j * m_MapTileWidth + mapOffsetX, i * m_MapTileHeight + mapOffsetY),
					glm::vec2(m_MapTileWidth, m_MapTileHeight),
					0.0f,
					0.5f
				);
			}
		}
	}
}
#endif