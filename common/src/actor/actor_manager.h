#pragma once

#include "actor.h"

class BaseScene;
bool actor_manager_is_local_player(Actor* actor);
void actor_manager_set_local_player(Actor* actor);
void actor_manager_set_scene(int id);

Actor* actor_manager_fetch_local_player();

Actor* actor_manager_find_player_by_name(const char* name);


void actor_manager_update();
void actor_manager_draw();

void luaopen_actor_manager(lua_State* L);
