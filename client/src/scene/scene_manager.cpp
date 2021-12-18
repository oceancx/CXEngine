#include "scene_manager.h"
#include "cxrandom.h"
#include <script_system.h>
#include "profile.h"
#include <scene/base_scene.h>
#include <scene/game_map.h>
#include "input_manager.h"
#include "game.h"
#include "cxmath.h"
#include "file_system.h"
#include "window.h"
#include "net.h"
#include "logger.h"
#include "actor/actor_manager.h"
#include "animation/sprite.h"
#include "cxlua.h"
#include "graphics/ui_renderer.h"
#include "sprite_renderer.h"
#include "texture_manager.h"


static bool s_DrawMask, s_DrawStrider = true, s_DrawCell, s_DrawMap= true, s_DrawAnnouncement, s_AutoRun;
static int sLastDrawCall = 0;
/*
lua 尽快完成tsv解析  然后把scene创建放在lua
*/
SceneManager::SceneManager()
	:m_pCurrentScene(nullptr),
	m_pNextScene(nullptr),
	m_PlayerEnterX(0),
	m_PlayerEnterY(0),
	m_SwitchingScene(false)
{

}

SceneManager::~SceneManager()
{
	for (auto& scene : m_Scenes)
	{
		delete scene.second;
		scene.second = nullptr;
	}
	m_Scenes.clear();
	UIRenderer::GetInstance()->DeleteSingleton();
}

void SceneManager::Init()
{
	script_system_call_function(script_system_get_luastate(), "on_scene_manager_init");
};

void SceneManager::SwitchScene(String name)
{
	if (m_pCurrentScene != nullptr)
	{
		if (m_pCurrentScene->GetName() == name)return;
	}
	auto it = m_Scenes.find(name);
	if (it != m_Scenes.end())
	{
		m_pNextScene = it->second;
		m_SwitchingScene = true;
	}
}

void SceneManager::SwitchScene(int id)
{
	for (auto& scene : m_Scenes)
	{
		if (scene.second->GetSceneID() == id)
		{
			m_pNextScene = scene.second;
			m_SwitchingScene = true;
			break;
		}
	}
}



void SceneManager::AddScene(BaseScene* scene)
{
	if (!scene) return;
	if (m_Scenes.count(scene->GetName()) > 0) return;
	m_Scenes[scene->GetName()] = scene;
};

void SceneManager::RemoveScene(String name)
{
	auto it = m_Scenes.find(name);
	if (it != m_Scenes.end())
	{
		delete it->second;
		it->second = nullptr;
		m_Scenes.erase(it);
	}
}


bool SceneManager::IsDrawMask()
{
	return s_DrawMask;
}

bool SceneManager::IsDrawStrider()
{
	return s_DrawStrider;
}

bool SceneManager::IsDrawCell()
{
	return s_DrawCell;
}

bool SceneManager::IsDrawMap()
{
	return s_DrawMap;
}

bool SceneManager::IsDrawAnnounce()
{
	return s_DrawAnnouncement;
}

bool SceneManager::IsAutoRun()
{
	return s_AutoRun;
}



void SceneManager::Update()
{
	if (m_SwitchingScene)
	{
		if (m_pNextScene != nullptr)
		{
			if (m_pCurrentScene)
			{
				script_system_call_function(script_system_get_luastate(), "on_scene_manager_uninit_scene", m_pCurrentScene->GetName());
				m_pCurrentScene->UnLoad();
				m_pCurrentScene = nullptr;
			}

			m_pCurrentScene = m_pNextScene;
			m_pCurrentScene->Load();

			script_system_call_function(script_system_get_luastate(), "on_scene_manager_init_scene", m_pCurrentScene->GetName());
		}
		m_SwitchingScene = false;
	}
	else
	{
		if (m_pCurrentScene)
		{
			m_pCurrentScene->Update();
			script_system_call_function(script_system_get_luastate(), "on_scene_manager_update", m_pCurrentScene->GetName());
		}
	}
};


void SceneManager::Draw()
{
	if (m_SwitchingScene)return;
	glBindFramebuffer(GL_FRAMEBUFFER, WINDOW_INSTANCE->GetFrameBuffer());

	SpriteRenderer::GetInstance()->ResetDrawCall();
	int gameWidth = WINDOW_INSTANCE->GetWidth();
	int gameHeight = WINDOW_INSTANCE->GetHeight();
	glViewport(0, 0, gameWidth, gameHeight);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	if (m_pCurrentScene) {
		m_pCurrentScene->Draw();
		script_system_call_function(script_system_get_luastate(), "on_scene_manager_draw", m_pCurrentScene->GetName());
	}

	UIRenderer::GetInstance()->Begin();
	UIRenderer::GetInstance()->Draw();
	UIRenderer::GetInstance()->End();

	sLastDrawCall = SpriteRenderer::GetInstance()->GetDrawCall();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
};


BaseScene* SceneManager::GetCurrentScene()
{
	return m_pCurrentScene;
};


Actor* scene_find_player(const char* player_name)
{
	return actor_manager_find_player_by_name(player_name);
}

void scene_manager_init()
{
	SCENE_MANAGER_INSTANCE->Init();
}

void scene_manager_update()
{
	SCENE_MANAGER_INSTANCE->Update();
}

 

void scene_manager_draw()
{
	SCENE_MANAGER_INSTANCE->Draw();
}
void scene_manager_deinit()
{
	SCENE_MANAGER_INSTANCE->DeleteSingleton();
}


void scene_manager_switch_scene_by_name(const char* name)
{
	SCENE_MANAGER_INSTANCE->SwitchScene(name);
}

void scene_manager_switch_scene_by_id(int id)
{
	auto& scenes = SCENE_MANAGER_INSTANCE->GetAllScene();
	for (auto& it : scenes) {
		if (it.second->GetSceneID() == id) {
			SCENE_MANAGER_INSTANCE->SwitchScene(it.second->GetName());
			return;
		}
	}
}

void scene_manager_add_scene(int id, const char* name)
{
	SCENE_MANAGER_INSTANCE->AddScene(new BaseScene(id, name));
}

void scene_manager_add_custom_scene(int scene_id, const char* name, int map_id)
{
	BaseScene* scene = new BaseScene(scene_id, name);;
	scene->SetMapID(map_id);
	SCENE_MANAGER_INSTANCE->AddScene(scene);
}
 



void scene_manager_sync_draw_cbx(bool draw_map, bool draw_cell, bool draw_strider, bool draw_mask, bool draw_announcement, bool auto_run) {
	s_DrawMap = draw_map;
	s_DrawCell = draw_cell;
	s_DrawStrider = draw_strider;
	s_DrawMask = draw_mask;
	s_DrawAnnouncement = draw_announcement;
	s_AutoRun = auto_run;
}

BaseScene* scene_manager_fetch_scene(int sceneID)
{
	auto& scenes = SCENE_MANAGER_INSTANCE->GetAllScene();
	for (auto& it : scenes) {
		if (it.second->GetSceneID() == sceneID) {
			return it.second;
		}
	}
	return nullptr;
}

int scene_manager_get_imgui_cursor_pos(lua_State* L) {
	Pos pos = SCENE_MANAGER_INSTANCE->GetImGuiCursorPos();
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	return 2;
};

int scene_manager_get_current_scene_id() {
	auto* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if (scene) {
		return scene->GetSceneID();
	}
	return 0;
}

string scene_manager_get_current_scene_name() {
	auto* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if (scene) {
		return scene->GetName();
	}
	return "";
}


bool scene_is_combat()
{
	auto* actor = actor_manager_fetch_local_player();
	if (actor) {
		return actor->IsCombat();
	}
	return false;
}




void scene_set_map(int mapid)
{
	auto* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if (scene) {
		scene->SetMapID(mapid);
	}
}

void game_map_reset_map_offset() {
	auto* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if (scene) {
		if (scene->GetGameMap()) {
			scene->GetGameMap()->ResetMapOffset();
		}
	}
}

int scene_get_map_offset(lua_State* L) {
	int x = 0;
	int y = 0;
	auto* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if (scene) {
		auto* map = scene->GetGameMap();
		if (map) {
			x = map->GetMapOffsetX();
			y = map->GetMapOffsetY();
		}
	}
	lua_pushinteger(L, x);
	lua_pushinteger(L, y);
	return 2;
}

int debug_get_drawcall() {
	return sLastDrawCall;
}

int debug_get_texture_count() {
	return TEXTURE_MANAGER_INSTANCE->GetTextureCount();
}

int debug_get_sprites_count() {
	return RESOURCE_MANAGER_INSTANCE->GetSpriteCount();
}


void luaopen_scene_manager(lua_State* L)
{

	script_system_register_function(L, debug_get_drawcall);
	script_system_register_function(L, debug_get_texture_count);
	script_system_register_function(L, debug_get_sprites_count);



	script_system_register_function(L, scene_manager_init);
	script_system_register_function(L, scene_manager_update);
	script_system_register_function(L, scene_manager_draw);
	 

	script_system_register_function(L, scene_manager_deinit);
	script_system_register_function(L, scene_manager_add_scene);
	script_system_register_function(L, scene_manager_add_custom_scene);

	script_system_register_function(L, scene_is_combat);
	script_system_register_function(L, scene_set_map);
	script_system_register_function(L, game_map_reset_map_offset);

	script_system_register_luac_function(L, scene_get_map_offset);

	script_system_register_luac_function(L, scene_manager_get_imgui_cursor_pos);


	script_system_register_function(L, scene_manager_switch_scene_by_id);
	script_system_register_function(L, scene_manager_switch_scene_by_name);


	script_system_register_function(L, scene_manager_get_current_scene_id);
	script_system_register_function(L, scene_manager_get_current_scene_name);

	script_system_register_function(L, scene_manager_sync_draw_cbx);

}