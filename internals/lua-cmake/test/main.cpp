#include <iostream>
extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
int main()
{
  lua_State*  L = luaL_newstate();
  return 0;
}