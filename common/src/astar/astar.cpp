#include "astar.h"
#include <fstream>
#include "cxmath.h"
#include "logger.h"

// N E S W N_E S_E S_W N_W
//int dir_x[8] = { 0, 1, 0, -1, 1, 1, -1, -1 };
//int dir_y[8] = { -1, 0, 1, 0, -1, 1, 1, -1 };

int dir_x[4] = { 0, 1, 0, -1};
int dir_y[4] = { -1, 0, 1, 0};


Astar::~Astar()
{

}

Astar::Node* Astar::NewNode(int x, int y)
{
	Node*tmpP2 = new Node();
	tmpP2->x = x;
	tmpP2->y = y;
	tmpP2->F = 0;
	tmpP2->H = 0;
	tmpP2->G = 0;
	tmpP2->parant = nullptr;
	return tmpP2;
}


void Astar::PrintMap(std::string filename, int w, int h)
{
	auto& map = mCell;
	std::ofstream fout(filename);
	for (int i = 0; i<h; i++) {
		for (int j = 0; j<w; j++) {
			if (map[j][i] == 0) {
				fout << '0';
				// printf("0");
			}
			else if (map[j][i] == 1) {
				fout << '1';
				// printf("1");
			}
			else if (map[j][i] == -2) {
				fout << '*';
				// printf("1");
				map[j][i] = 0;
			}
			else
			{
				fout << '1';
			}

		}
		fout << '\n';
	}
	fout.close();
}

bool Astar::PathFinding(int sx, int sy, int ex, int ey)
{
	//printf("sx %d sy %d ex %d ey %d\n", sx, sy, ex, ey);
	mOpenList.clear();
	mCloseList.clear();
	mVisOpen.clear();
	mVisClose.clear();
	mPathList.clear();

	Node* start_node = NewNode(sx, sy);
	mOpenList.add(start_node);
	insertVis(sx, sy, start_node, mVisOpen);

	while (!mOpenList.empty()) {
		eraseVis(sx, sy, mVisOpen);
		Node* cur_node = mOpenList.pop();
	//	mOpenList.print();
		//printf("cur x:%d y:%d key:%d\n",cur_node->x,cur_node->y,cur_node->key );
		
		insertVis(sx, sy, cur_node, mVisClose);
		mCloseList.push_back(cur_node);

		if (cur_node->x == ex && cur_node->y == ey) {
			Node* p = cur_node;
			do {
				Pos tmpPos;
			//	printf("x: %d y: %d\n", p->x, p->y);
				//mCell[p->x][p->y] = -2;
				// Node* curP = p;
				tmpPos.x = p->x*1.f;
				tmpPos.y = p->y*1.f;
				mPathList.push_front(tmpPos);

				if (p->y == sx && p->x == sy) {
					break;
				}
				p = p->parant;
				// delete curP;
			} while (p != nullptr);

		//	PrintMap("x.ttt", mWidth, mHeight, mCell);
			return true;
		}

		//for (int i = 0; i < 8; i++) {
		for (int i = 0; i < 4; i++) {
			int new_x = cur_node->x + dir_x[i];
			int new_y = cur_node->y + dir_y[i];
				
			//printf("i:%d new_x : %d new_y : %d\n",i,new_x,new_y );
			//int new_G_dist = i< 4 ?10  :14;
			int new_G_dist = 10 ;
			/*if (cur_node->parant != nullptr)
			{
				auto p_node = cur_node->parant;
				if (cur_node->x - p_node->x == new_x-cur_node->x && cur_node->y - p_node->y == new_y- cur_node->y)
				{
					new_G_dist = 9;
				}
			}*/

			if (new_x < 0 || new_y < 0 || new_x >= mCell.size() || new_y >= mCell[new_x].size()
				|| mCell[new_x][new_y] >= 1 || findVis(new_x, new_y, mVisClose) != nullptr) 
			{
				continue;
			}
			else
			{
				Node *pNodeVisOpen = findVis(new_x, new_y, mVisOpen);
				if (pNodeVisOpen == nullptr)
				{
					Node* newP = NewNode(new_x, new_y);
					newP->G = cur_node->G + new_G_dist;
					newP->H = abs(new_x - ex) * 10 + abs(new_y - ey) * 10;
					newP->F = newP->H + newP->G;
					newP->parant = cur_node;
					// printf("new F:%d=G(%d)+H(%d)\n",newP->F ,newP->G,newP->H );
					insertVis(new_x, new_y, newP, mVisOpen);
					mOpenList.add(newP);
				}
				else {
					if (cur_node->G + new_G_dist < pNodeVisOpen->G) {
						pNodeVisOpen->G = cur_node->G + new_G_dist;
						pNodeVisOpen->F = pNodeVisOpen->G + pNodeVisOpen->H;
						pNodeVisOpen->parant = cur_node;
					}
				}
			}
		}
	}

	// 	PrintMap("x.ttt", width, height, map);
	return false;
}