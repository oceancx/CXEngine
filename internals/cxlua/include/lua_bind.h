#pragma once
#include "any.h"
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include <functional>
#include <vector>
#include <cmath>
#include <string>

template <typename T>
T* lua_check_pointer(lua_State* L, int index) {
	T** ptr = (T**)lua_touserdata(L, index);
	return *ptr;
};

template <typename T>
void lua_push_pointer(lua_State* L, T* ptr) {
	T** ud = (T**)lua_newuserdata(L, sizeof(T*));
	*ud = ptr;
}



bool check_lua_error(lua_State* L, int res, const char* func = "");

template <typename Ret, typename... Args>
Ret callfunc(std::function<Ret(Args...)> func, std::vector<any> anyargs);

template <typename Ret>
Ret callfunc(std::function<Ret()> func, std::vector<any> anyargs)
{
	if (anyargs.size() > 0)
		throw std::runtime_error("oops, argument list too long");
	return func();
}

template <typename Ret, typename Arg0, typename... Args>
Ret callfunc(std::function<Ret(Arg0, Args...)> func, std::vector<any> anyargs)
{
	if (anyargs.size() == 0)
		throw std::runtime_error("oops, argument list too short");
	Arg0 arg0 = any_cast<Arg0>(anyargs[0]);
	anyargs.erase(anyargs.begin());
	std::function<Ret(Args... args)> lambda =
		([=](Args... args) -> Ret {
		return func(arg0, args...);
			});
	return callfunc(lambda, anyargs);
}

template <typename Ret, typename... Args>
std::function<any(std::vector<any>)> adaptfunc(Ret(*func)(Args...)) {
	std::function<Ret(Args...)> stdfunc = func;
	std::function<any(std::vector<any>)> result =
		([=](std::vector<any> anyargs) -> any {
		return any(callfunc(stdfunc, anyargs));
			});
	return result;
}

template < typename... Args>
std::function<any(std::vector<any>)> adaptfunc(void(*func)(Args...)) {
	std::function<void(Args...)> stdfunc = func;
	std::function<any(std::vector<any>)> result =
		([=](std::vector<any> anyargs) -> any {
		callfunc(stdfunc, anyargs);
		return any();
			});
	return result;
}

static inline any lua_getanyvalue(lua_State* L, int i)
{
	switch (lua_type(L, i))
	{
	case LUA_TNIL:
		return 0;
	case LUA_TBOOLEAN:
		return static_cast<bool>(lua_toboolean(L, i));
	case LUA_TLIGHTUSERDATA:
		return 0;
	case LUA_TNUMBER:
	{
		lua_Number num = lua_tonumber(L, i);
		lua_Number inum = std::floor(num);
		if (num - inum != 0)
		{
			return (lua_Number)num;
		}
		else {
			lua_Integer n = (lua_Integer)num;
			if (n > INT32_MAX) {
				return (lua_Integer)n;
			}
			else {
				return (int32_t)n;
			}
		}

	}
	case LUA_TSTRING:
		return lua_tostring(L, i);
	case LUA_TTABLE:
	case LUA_TFUNCTION:
	case LUA_TUSERDATA:
	case LUA_TTHREAD:
	default:
		return 0;
	}
}

static inline void lua_pushanyvalue(lua_State* L, any a)
{
	if (a.type() == typeid(bool))
	{
		lua_pushboolean(L, any_cast<bool>(a));
	}
	else if (a.type() == typeid(int))
	{
		lua_pushinteger(L, any_cast<int>(a));
	}
	else if (a.type() == typeid(double))
	{
		lua_pushnumber(L, any_cast<double>(a));
	}
	else if (a.type() == typeid(float))
	{
		lua_pushnumber(L, (double)(any_cast<float>(a)));
	}
	else if (a.type() == typeid(const char*))
	{
		lua_pushstring(L, any_cast<const char*>(a));
	}
	else if (a.type() == typeid(std::string))
	{
		lua_pushstring(L, any_cast<std::string>(a).c_str());
	}
	else if (a.type() == typeid(uint64_t)) {
		lua_pushinteger(L, any_cast<uint64_t>(a));
	}
	else if (a.type() == typeid(int64_t))
	{
		lua_pushinteger(L, any_cast<int64_t>(a));
	}
	else
	{
		lua_pushboolean(L, true);
	}
}

template<typename...Ts>
void lua_pushargs(lua_State* L, Ts... args)
{
	auto t = std::make_tuple(args...);
	lua_pushhelper(L, t, std::make_index_sequence<sizeof...(Ts)>());
}

template<typename ...Ts, std::size_t...Is>
void lua_pushhelper(lua_State* L, std::tuple<Ts...> const& t, std::index_sequence<Is...>)
{
	using expander = int[];
	(void)expander {
		0, (lua_pushanyvalue(L, std::get<Is>(t)), 0)...
	};
}



template <typename FuncType>
int cfunction_t(lua_State* L)
{
	FuncType** af = (FuncType**)lua_touserdata(L, lua_upvalueindex(1));
	int argn = lua_gettop(L);
	std::vector<any> params;
	for (int i = 1; i <= argn; i++)
	{
		params.push_back(lua_getanyvalue(L, i));
	}

	any ret = adaptfunc(*af)(params);
	if (ret.type() == typeid(void))
		return 0;

	lua_pushanyvalue(L, ret);
	return 1;
}

template<typename FuncType>
void lua_register_c_function(lua_State* L, FuncType* func, const char* name)
{
	FuncType** ud = (FuncType**)lua_newuserdata(L, sizeof(FuncType*));
	(*ud) = (FuncType*)func;
	lua_pushcclosure(L, cfunction_t<FuncType>, 1);
	lua_setglobal(L, name);
}

#define lua_register_function(L,fn) lua_register_c_function<decltype(fn)>(L,fn,#fn)

template <typename...Ts>
std::vector<any> call_lua_function(lua_State* L, char const* func, Ts... args)
{
	int top = lua_gettop(L);
	lua_getglobal(L, func);
	if (lua_isnil(L, -1)) {
		std::vector<any> rets;
		rets.push_back(true);
		return rets;
	}
	if (sizeof...(Ts) != 0)
		lua_pushargs(L, args...);
	int res = lua_pcall(L, sizeof...(Ts), LUA_MULTRET, 0);
	check_lua_error(L, res, func);

	int nret = lua_gettop(L) - top;
	std::vector<any> rets;
	for (int i = 1; i <= nret; i++)
	{
		rets.push_back(lua_getanyvalue(L, top + i));
	}
	return rets;
}


#define script_system_register_function(L, fn) lua_register_function(L, fn)
#define script_system_register_luac_function(L, fn) (lua_pushcfunction(L, (fn)), lua_setglobal(L, #fn))
#define script_system_register_luac_function_with_name(L, name, fn) (lua_pushcfunction(L, (fn)), lua_setglobal(L, name))
#define script_system_call_function(L, ...) call_lua_function(L, __VA_ARGS__)
