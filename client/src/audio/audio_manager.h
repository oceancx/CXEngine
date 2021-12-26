#pragma once
#include <functional>

int audio_manager_play(const char* path, bool loop = false);
void audio_manager_stop(const char* path);

void luaopen_audio_manager(lua_State* L);