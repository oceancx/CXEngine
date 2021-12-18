#pragma once
#include <string>
#include "singleton.h"
#include "pos.h"
/*
负责初始化Engine Update Draw
*/
class Game : public Singleton<Game>
{
public:
	Game();
	~Game();
	Pos MapPosToScreenPos(Pos mapPos);
	Pos ScreenPosToMapPos(Pos screenPos);
private:
	bool IsRunning();
};

#define GAME_INSTANCE Game::GetInstance()

void util_save_image_file(const char* filename, int width, int height, int pixelDepth, char* data);
void luaopen_game(lua_State* L);