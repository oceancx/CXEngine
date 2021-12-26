#pragma once

#include "define_types.h"
#include "entity.h"
#include "pos.h"

class GameMap;
/*
场景对象的基类
可以拆分出SplashScene , UIScene , Scene（游戏场景）
*/
class BaseScene
{
public:
	BaseScene(int sceneID, String sceneName);

	virtual ~BaseScene();

	virtual void Update();

#ifndef SIMPLE_SERVER
	virtual void Draw();
#endif

	void SetSceneID(int id) { m_SceneID = id; };

	int GetSceneID() { return m_SceneID; };

	void SetMapID(int id) ;
	int GetMapID() { return m_MapID; };

	bool IsLoading() { return m_IsLoading; }

	void SetLoading(bool load) { m_IsLoading = load; }

	Pos GetBirthPos() { return  m_BirthPos; };

	void SetBirthPos(Pos pos) { m_BirthPos = pos; };

	void SetLoadingCenter(Pos pos) { m_LoadingCenter = pos; }

	int GetOffsetX() { return m_OffsetX; };

	int GetOffsetY() { return m_OffsetY; }

	void SetName(std::string str) { m_Name = str; }

	std::string GetName() { return m_Name; }

	
	GameMap* GetGameMap() { return m_Map; };

	void Load();

	void UnLoad();

protected:
	virtual void OnLoad() {} ;

	virtual void OnUnLoad() {};

	std::string m_Name;

	int m_SceneID;

	int m_MapID;
	
	int m_Width;

	int m_Height;

	int m_OffsetX;

	int m_OffsetY;
	
	bool m_IsLoading;

	bool m_Loaded;

	Pos m_BirthPos;
	
	Pos m_LoadingCenter;

	GameMap* m_Map;


};


