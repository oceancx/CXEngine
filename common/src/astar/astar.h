#pragma once
#include <string>
#include <list>
#include "heap.h"
#include <map>
#include "pos.h"
/*
纯算法实现 不包含输入输出
*/
class Astar
{
public:
	struct Node
	{
		int x, y;
		int  G, H;
		union {
			int F;
			int key;
		};
		int dir;
		Node* parant;
	};


	Astar(int width, int height, std::vector<std::vector<int>> cell) : mWidth(width), mHeight(height) {
		mCell = cell;
	};
	~Astar();
	bool PathFinding(int sx, int sy, int ex, int ey);
	Node* NewNode(int x, int y);

	template <typename T>
	void insertVis(int x, int y, Node* pNode, T& t)
	{
		t.insert(std::make_pair(std::make_pair(x, y), pNode));
	}

	template <typename T>
	Node* findVis(int x, int y, T& t)
	{
		std::map<std::pair<int, int>, Node*>::iterator
			iter = t.find(std::make_pair(x, y));
		if (iter == t.end())
		{
			return nullptr;
		}
		else
		{
			return (*iter).second;
		}
	}

	template <typename T>
	void eraseVis(int x, int y, T& t)
	{
		t.erase(std::make_pair(x, y));
	}
	std::list<Pos> GetMoveList()
	{
		return mPathList;
	}

private:
	void PrintMap(std::string filename, int w, int h);

	std::vector<std::vector<int>> mCell;
	int mWidth;
	int mHeight;

	Heap<Node*> mOpenList;
	std::list<Node*> mCloseList;
	std::list<Pos> mPathList;
	std::map<std::pair<int, int>, Node*> mVisOpen;
	std::map<std::pair<int, int>, Node*> mVisClose;


};

