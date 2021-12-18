#include "actor_manager.h"
#include "actor.h"
#include "utils.h"
#include "cxmath.h"
#ifndef SIMPLE_SERVER
#include "scene/base_scene.h"
#include "scene/game_map.h"
#include "window.h"
#include "animation/sprite.h"
#include "scene/scene_manager.h"
#endif

std::map<uint64_t, Actor*> g_Players;
uint64_t g_LocalPid = 0;

int actor_manager_set_local_player(lua_State*L) {
	uint64_t pid = (uint64_t)lua_tointeger(L, 1);
	g_LocalPid = pid;
	return 0;
}

void actor_manager_set_local_player(Actor* actor)
{
	g_LocalPid = actor->GetID();
}

bool actor_manager_is_local_player(Actor* actor) {
	return actor->GetID() == g_LocalPid;
}

void actor_manager_set_scene(int id)
{
	for(auto& it : g_Players){
		it.second->SetProperty(PROP_SCENE_ID, id);
	}
}
int lua_actor_manager_fetch_local_player(lua_State*L)
{
	auto it = g_Players.find(g_LocalPid);
	if (it != g_Players.end()) {
		lua_push_actor(L, it->second);
		return 1;
	}
	return 0;
}

Actor* actor_manager_fetch_local_player()
{
	auto it = g_Players.find(g_LocalPid);
	if (it != g_Players.end()) {
		return it->second;
	}
	return nullptr;
}

Actor* actor_manager_find_player_by_name(const char* name){
	for(auto& it : g_Players){
		if(it.second->GetName() ==std::string(name)){
			return it.second;
		}
	}
	return nullptr;
}

void actor_manager_update()
{
	auto* player = actor_manager_fetch_local_player();
	if (!player)return;
	for (auto& it : g_Players)
	{
		if(player->GetProperty(PROP_SCENE_ID).toInt()==it.second->GetProperty(PROP_SCENE_ID).toInt()){
			it.second->OnUpdate();
		}
	}
#ifndef SIMPLE_SERVER
	AnimationManager::GetInstance()->Update();
#endif
}

void actor_manager_draw()
{
#ifndef SIMPLE_SERVER
	auto* player = actor_manager_fetch_local_player();
	if (!player)return;
	vector<Actor*> drawPlayers;
	for (auto& it : g_Players)
	{
		if (player->GetProperty(PROP_SCENE_ID).toInt() == it.second->GetProperty(PROP_SCENE_ID).toInt()) {
			drawPlayers.push_back(it.second);
		}
	}
	
	std::sort(drawPlayers.begin(), drawPlayers.end(), [](Actor* lhs, Actor* rhs) {
		return lhs->GetY() < rhs->GetY();
		});

	for (auto* player: drawPlayers)
	{
		player->OnDraw();
	}
	AnimationManager::GetInstance()->Draw();
#endif // !SIMPLE_SERVER
}

int lua_actor_manager_create_actor(lua_State*L)
{
	uint64_t pid = (uint64_t)lua_tointeger(L, 1);
	Actor* actor = new Actor(pid);
	g_Players.insert({ pid, actor });
	lua_push_actor(L, actor);
	return 1;
}


int lua_actor_manager_destroy_actor(lua_State* L)
{
	uint64_t pid = (uint64_t)lua_tointeger(L, 1);
	auto it = g_Players.find(pid);
	if (it != g_Players.end()) {
		delete it->second; 
		g_Players.erase(it);
	}
	return 0;
}



int lua_create_actor(lua_State* L)
{
	uint64_t pid = (uint64_t)lua_tointeger(L, 1);
	Actor* actor = new Actor(pid);
	lua_push_actor(L, actor);
	return 1;
}


int lua_destroy_actor(lua_State* L)
{
	Actor* actor = lua_check_actor(L, 1);
	delete actor;
	return 0;
}

 

int lua_actor_manager_fetch_player_by_id(lua_State*L) {
	uint64_t pid = (uint64_t)lua_tointeger(L, 1);
	auto it = g_Players.find(pid);
	if (it != g_Players.end()) {
		lua_push_actor(L, it->second);
		return 1;
	}
	return 0;
}


int lua_actor_manager_fetch_all_players(lua_State*L) {
	lua_newtable(L);
	int i = 1;
	for(auto& it : g_Players){
		if(it.second->GetProperty(PROP_ACTOR_TYPE).toInt() == ACTOR_TYPE_PLAYER){
			lua_push_actor(L, it.second);
			lua_seti(L, -2, i++);
		}
	}
	return 1;
}


int lua_actor_manager_fetch_all_actors(lua_State* L) {
	lua_newtable(L);
	int i = 1;
	for (auto& it : g_Players) {
		lua_push_actor(L, it.second);
		lua_seti(L, -2, i++);
	}
	return 1;
}



int lua_actor_manager_clear_all(lua_State*L){
	for (auto& it : g_Players) {
		SafeDelete(it.second);
	}
	g_Players.clear();
	return 0;
}

void luaopen_actor_manager(lua_State* L) {
	
	script_system_register_luac_function_with_name(L, "actor_manager_clear_all", lua_actor_manager_clear_all);
	script_system_register_luac_function_with_name(L, "actor_manager_destroy_actor", lua_actor_manager_destroy_actor);
	script_system_register_luac_function_with_name(L, "actor_manager_create_actor", lua_actor_manager_create_actor);
	script_system_register_luac_function_with_name(L, "actor_manager_fetch_local_player", lua_actor_manager_fetch_local_player);
	script_system_register_luac_function_with_name(L, "actor_manager_fetch_player_by_id", lua_actor_manager_fetch_player_by_id);
	script_system_register_luac_function_with_name(L, "scene_manager_fetch_local_player", lua_actor_manager_fetch_local_player);
	script_system_register_luac_function_with_name(L, "actor_manager_fetch_all_players", lua_actor_manager_fetch_all_players);
	script_system_register_luac_function_with_name(L, "actor_manager_fetch_all_actors", lua_actor_manager_fetch_all_actors);

	script_system_register_function(L, actor_manager_update);
	script_system_register_function(L, actor_manager_draw);
	script_system_register_function(L, actor_manager_set_scene);
	script_system_register_luac_function(L, actor_manager_set_local_player);

	script_system_register_luac_function(L, lua_create_actor);
	script_system_register_luac_function(L, lua_destroy_actor);
}

