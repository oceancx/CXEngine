#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#ifdef __cplusplus
}
#endif

typedef struct _LuaProxy
{
	lua_State *(*lua_newstate)(lua_Alloc f, void *ud);
	void(*lua_close)(lua_State *L);
	lua_State *(*lua_newthread)(lua_State *L);
	lua_CFunction(*lua_atpanic)(lua_State *L, lua_CFunction panicf);
	const lua_Number *(*lua_version)(lua_State *L);
	int(*lua_absindex)(lua_State *L, int idx);
	int(*lua_gettop)(lua_State *L);
	void(*lua_settop)(lua_State *L, int idx);
	void(*lua_pushvalue)(lua_State *L, int idx);
	void(*lua_rotate)(lua_State *L, int idx, int n);
	void(*lua_copy)(lua_State *L, int fromidx, int toidx);
	int(*lua_checkstack)(lua_State *L, int n);
	void(*lua_xmove)(lua_State *from, lua_State *to, int n);
	int(*lua_isnumber)(lua_State *L, int idx);
	int(*lua_isstring)(lua_State *L, int idx);
	int(*lua_iscfunction)(lua_State *L, int idx);
	int(*lua_isinteger)(lua_State *L, int idx);
	int(*lua_isuserdata)(lua_State *L, int idx);
	int(*lua_type)(lua_State *L, int idx);
	const char     *(*lua_typename)(lua_State *L, int tp);
	lua_Number(*lua_tonumberx)(lua_State *L, int idx, int *isnum);
	lua_Integer(*lua_tointegerx)(lua_State *L, int idx, int *isnum);
	int(*lua_toboolean)(lua_State *L, int idx);
	const char     *(*lua_tolstring)(lua_State *L, int idx, size_t *len);
	size_t(*lua_rawlen)(lua_State *L, int idx);
	lua_CFunction(*lua_tocfunction)(lua_State *L, int idx);
	void	       *(*lua_touserdata)(lua_State *L, int idx);
	lua_State      *(*lua_tothread)(lua_State *L, int idx);
	const void     *(*lua_topointer)(lua_State *L, int idx);
	void(*lua_arith)(lua_State *L, int op);
	int(*lua_rawequal)(lua_State *L, int idx1, int idx2);
	int(*lua_compare)(lua_State *L, int idx1, int idx2, int op);
	void(*lua_pushnil)(lua_State *L);
	void(*lua_pushnumber)(lua_State *L, lua_Number n);
	void(*lua_pushinteger)(lua_State *L, lua_Integer n);
	const char *(*lua_pushlstring)(lua_State *L, const char *s, size_t len);
	const char *(*lua_pushstring)(lua_State *L, const char *s);
	const char *(*lua_pushvfstring)(lua_State *L, const char *fmt, va_list argp);
	const char *(*lua_pushfstring)(lua_State *L, const char *fmt, ...);
	void(*lua_pushcclosure)(lua_State *L, lua_CFunction fn, int n);
	void(*lua_pushboolean)(lua_State *L, int b);
	void(*lua_pushlightuserdata)(lua_State *L, void *p);
	int(*lua_pushthread)(lua_State *L);
	int(*lua_getglobal)(lua_State *L, const char *name);
	int(*lua_gettable)(lua_State *L, int idx);
	int(*lua_getfield)(lua_State *L, int idx, const char *k);
	int(*lua_geti)(lua_State *L, int idx, lua_Integer n);
	int(*lua_rawget)(lua_State *L, int idx);
	int(*lua_rawgeti)(lua_State *L, int idx, lua_Integer n);
	int(*lua_rawgetp)(lua_State *L, int idx, const void *p);
	void(*lua_createtable)(lua_State *L, int narr, int nrec);
	void *(*lua_newuserdata)(lua_State *L, size_t sz);
	int(*lua_getmetatable)(lua_State *L, int objindex);
	int(*lua_getuservalue)(lua_State *L, int idx);
	void(*lua_setglobal)(lua_State *L, const char *name);
	void(*lua_settable)(lua_State *L, int idx);
	void(*lua_setfield)(lua_State *L, int idx, const char *k);
	void(*lua_seti)(lua_State *L, int idx, lua_Integer n);
	void(*lua_rawset)(lua_State *L, int idx);
	void(*lua_rawseti)(lua_State *L, int idx, lua_Integer n);
	void(*lua_rawsetp)(lua_State *L, int idx, const void *p);
	int(*lua_setmetatable)(lua_State *L, int objindex);
	void(*lua_setuservalue)(lua_State *L, int idx);
	void(*lua_callk)(lua_State *L, int nargs, int nresults, lua_KContext ctx, lua_KFunction k);
	int(*lua_pcallk)(lua_State *L, int nargs, int nresults, int errfunc, lua_KContext ctx, lua_KFunction k);
	int(*lua_load)(lua_State *L, lua_Reader reader, void *data, const char *chunkname, const char *mode);
	int(*lua_dump)(lua_State *L, lua_Writer writer, void *data, int strip);
	int(*lua_yieldk)(lua_State *L, int nresults, lua_KContext ctx, lua_KFunction k);
	int(*lua_resume)(lua_State *L, lua_State *from, int narg);
	int(*lua_status)(lua_State *L);
	int(*lua_isyieldable)(lua_State *L);
	int(*lua_gc)(lua_State *L, int what, int data);
	int(*lua_error)(lua_State *L);
	int(*lua_next)(lua_State *L, int idx);
	void(*lua_concat)(lua_State *L, int n);
	void(*lua_len)(lua_State *L, int idx);
	size_t(*lua_stringtonumber)(lua_State *L, const char *s);
	lua_Alloc(*lua_getallocf)(lua_State *L, void **ud);
	void(*lua_setallocf)(lua_State *L, lua_Alloc f, void *ud);
	int(*lua_getstack)(lua_State *L, int level, lua_Debug *ar);
	int(*lua_getinfo)(lua_State *L, const char *what, lua_Debug *ar);
	const char *(*lua_getlocal)(lua_State *L, const lua_Debug *ar, int n);
	const char *(*lua_setlocal)(lua_State *L, const lua_Debug *ar, int n);
	const char *(*lua_getupvalue)(lua_State *L, int funcindex, int n);
	const char *(*lua_setupvalue)(lua_State *L, int funcindex, int n);
	void *(*lua_upvalueid)(lua_State *L, int fidx, int n);
	void(*lua_upvaluejoin)(lua_State *L, int fidx1, int n1, int fidx2, int n2);
	void(*lua_sethook)(lua_State *L, lua_Hook func, int mask, int count);
	lua_Hook(*lua_gethook)(lua_State *L);
	int(*lua_gethookmask)(lua_State *L);
	int(*lua_gethookcount)(lua_State *L);
	int(*luaopen_base)(lua_State *L);
	int(*luaopen_coroutine)(lua_State *L);
	int(*luaopen_table)(lua_State *L);
	int(*luaopen_io)(lua_State *L);
	int(*luaopen_os)(lua_State *L);
	int(*luaopen_string)(lua_State *L);
	int(*luaopen_utf8)(lua_State *L);
	int(*luaopen_bit32)(lua_State *L);
	int(*luaopen_math)(lua_State *L);
	int(*luaopen_debug)(lua_State *L);
	int(*luaopen_package)(lua_State *L);
	void(*luaL_checkversion_)(lua_State *L, lua_Number ver, size_t sz);
	int(*luaL_getmetafield)(lua_State *L, int obj, const char *e);
	int(*luaL_callmeta)(lua_State *L, int obj, const char *e);
	const char *(*luaL_tolstring)(lua_State *L, int idx, size_t *len);
	int(*luaL_argerror)(lua_State *L, int arg, const char *extramsg);
	const char *(*luaL_checklstring)(lua_State *L, int arg, size_t *l);
	const char *(*luaL_optlstring)(lua_State *L, int arg, const char *def, size_t *l);
	lua_Number(*luaL_checknumber)(lua_State *L, int arg);
	lua_Number(*luaL_optnumber)(lua_State *L, int arg, lua_Number def);
	lua_Integer(*luaL_checkinteger)(lua_State *L, int arg);
	lua_Integer(*luaL_optinteger)(lua_State *L, int arg, lua_Integer def);
	void(*luaL_checkstack)(lua_State *L, int sz, const char *msg);
	void(*luaL_checktype)(lua_State *L, int arg, int t);
	void(*luaL_checkany)(lua_State *L, int arg);
	int(*luaL_newmetatable)(lua_State *L, const char *tname);
	void(*luaL_setmetatable)(lua_State *L, const char *tname);
	void *(*luaL_testudata)(lua_State *L, int ud, const char *tname);
	void *(*luaL_checkudata)(lua_State *L, int ud, const char *tname);
	void(*luaL_where)(lua_State *L, int lvl);
	int(*luaL_error)(lua_State *L, const char *fmt, ...);
	int(*luaL_checkoption)(lua_State *L, int arg, const char *def, const char *const lst[]);
	int(*luaL_fileresult)(lua_State *L, int stat, const char *fname);
	int(*luaL_execresult)(lua_State *L, int stat);
	int(*luaL_ref)(lua_State *L, int t);
	void(*luaL_unref)(lua_State *L, int t, int ref);
	int(*luaL_loadfilex)(lua_State *L, const char *filename, const char *mode);
	int(*luaL_loadbufferx)(lua_State *L, const char *buff, size_t sz, const char *name, const char *mode);
	int(*luaL_loadstring)(lua_State *L, const char *s);
	lua_State *(*luaL_newstate)(void);
	lua_Integer(*luaL_len)(lua_State *L, int idx);
	const char *(*luaL_gsub)(lua_State *L, const char *s, const char *p, const char *r);
	void(*luaL_setfuncs)(lua_State *L, const luaL_Reg *l, int nup);
	int(*luaL_getsubtable)(lua_State *L, int idx, const char *fname);
	void(*luaL_traceback)(lua_State *L, lua_State *L1, const char *msg, int level);
	void(*luaL_requiref)(lua_State *L, const char *modname, lua_CFunction openf, int glb);
	void(*luaL_buffinit)(lua_State *L, luaL_Buffer *B);
	char *(*luaL_prepbuffsize)(luaL_Buffer *B, size_t sz);
	void(*luaL_addlstring)(luaL_Buffer *B, const char *s, size_t l);
	void(*luaL_addstring)(luaL_Buffer *B, const char *s);
	void(*luaL_addvalue)(luaL_Buffer *B);
	void(*luaL_pushresult)(luaL_Buffer *B);
	void(*luaL_pushresultsize)(luaL_Buffer *B, size_t sz);
	char *(*luaL_buffinitsize)(lua_State *L, luaL_Buffer *B, size_t sz);
	void(*luaL_pushmodule)(lua_State *L, const char *modname, int sizehint);
	void(*luaL_openlib)(lua_State *L, const char *libname, const luaL_Reg *l, int nup);
	void(*luaL_openlibs)(lua_State *L);
}LuaProxy;

#if defined(_WIN32) && defined(_LUADBG_BUILD_DLL)
#define LUADBGAPI __declspec(dllexport)
#elif defined(_WIN32) && defined(LUADBG_DLL)
#define LUADBGAPI __declspec(dllimport)
#elif defined(__GNUC__) && defined(_LUADBG_BUILD_DLL)
#define LUADBGAPI __attribute__((visibility("default")))
#else
#define LUADBGAPI
#endif


#ifdef __cplusplus
extern "C" { 
#endif	
LUADBGAPI int _luaopen_luadbg(LuaProxy* (*__proxy__)(), lua_State* L);
#ifdef __cplusplus
}
#endif

#ifdef _LUADBG_BUILD_DLL
#ifdef __cplusplus
extern "C" {
#endif
	LuaProxy* __lua_proxy__();
#ifdef __cplusplus
}
#endif
#define lua_newstate __lua_proxy__()->lua_newstate
#define lua_close __lua_proxy__()->lua_close
#define lua_newthread __lua_proxy__()->lua_newthread
#define lua_atpanic __lua_proxy__()->lua_atpanic
#define lua_version __lua_proxy__()->lua_version
#define lua_absindex __lua_proxy__()->lua_absindex
#define lua_gettop __lua_proxy__()->lua_gettop
#define lua_settop __lua_proxy__()->lua_settop
#define lua_pushvalue __lua_proxy__()->lua_pushvalue
#define lua_rotate __lua_proxy__()->lua_rotate
#define lua_copy __lua_proxy__()->lua_copy
#define lua_checkstack __lua_proxy__()->lua_checkstack
#define lua_xmove __lua_proxy__()->lua_xmove
#define lua_isnumber __lua_proxy__()->lua_isnumber
#define lua_isstring __lua_proxy__()->lua_isstring
#define lua_iscfunction __lua_proxy__()->lua_iscfunction
#define lua_isinteger __lua_proxy__()->lua_isinteger
#define lua_isuserdata __lua_proxy__()->lua_isuserdata
#define lua_type __lua_proxy__()->lua_type
#define lua_typename __lua_proxy__()->lua_typename
#define lua_tonumberx __lua_proxy__()->lua_tonumberx
#define lua_tointegerx __lua_proxy__()->lua_tointegerx
#define lua_toboolean __lua_proxy__()->lua_toboolean
#define lua_tolstring __lua_proxy__()->lua_tolstring
#define lua_rawlen __lua_proxy__()->lua_rawlen
#define lua_tocfunction __lua_proxy__()->lua_tocfunction
#define lua_touserdata __lua_proxy__()->lua_touserdata
#define lua_tothread __lua_proxy__()->lua_tothread
#define lua_topointer __lua_proxy__()->lua_topointer
#define lua_arith __lua_proxy__()->lua_arith
#define lua_rawequal __lua_proxy__()->lua_rawequal
#define lua_compare __lua_proxy__()->lua_compare
#define lua_pushnil __lua_proxy__()->lua_pushnil
#define lua_pushnumber __lua_proxy__()->lua_pushnumber
#define lua_pushinteger __lua_proxy__()->lua_pushinteger
#define lua_pushlstring __lua_proxy__()->lua_pushlstring
#define lua_pushstring __lua_proxy__()->lua_pushstring
#define lua_pushvfstring __lua_proxy__()->lua_pushvfstring
#define lua_pushfstring __lua_proxy__()->lua_pushfstring
#define lua_pushcclosure __lua_proxy__()->lua_pushcclosure
#define lua_pushboolean __lua_proxy__()->lua_pushboolean
#define lua_pushlightuserdata __lua_proxy__()->lua_pushlightuserdata
#define lua_pushthread __lua_proxy__()->lua_pushthread
#define lua_getglobal __lua_proxy__()->lua_getglobal
#define lua_gettable __lua_proxy__()->lua_gettable
#define lua_getfield __lua_proxy__()->lua_getfield
#define lua_geti __lua_proxy__()->lua_geti
#define lua_rawget __lua_proxy__()->lua_rawget
#define lua_rawgeti __lua_proxy__()->lua_rawgeti
#define lua_rawgetp __lua_proxy__()->lua_rawgetp
#define lua_createtable __lua_proxy__()->lua_createtable
#define lua_newuserdata __lua_proxy__()->lua_newuserdata
#define lua_getmetatable __lua_proxy__()->lua_getmetatable
#define lua_getuservalue __lua_proxy__()->lua_getuservalue
#define lua_setglobal __lua_proxy__()->lua_setglobal
#define lua_settable __lua_proxy__()->lua_settable
#define lua_setfield __lua_proxy__()->lua_setfield
#define lua_seti __lua_proxy__()->lua_seti
#define lua_rawset __lua_proxy__()->lua_rawset
#define lua_rawseti __lua_proxy__()->lua_rawseti
#define lua_rawsetp __lua_proxy__()->lua_rawsetp
#define lua_setmetatable __lua_proxy__()->lua_setmetatable
#define lua_setuservalue __lua_proxy__()->lua_setuservalue
#define lua_callk __lua_proxy__()->lua_callk
#define lua_pcallk __lua_proxy__()->lua_pcallk
#define lua_load __lua_proxy__()->lua_load
#define lua_dump __lua_proxy__()->lua_dump
#define lua_yieldk __lua_proxy__()->lua_yieldk
#define lua_resume __lua_proxy__()->lua_resume
#define lua_status __lua_proxy__()->lua_status
#define lua_isyieldable __lua_proxy__()->lua_isyieldable
#define lua_gc __lua_proxy__()->lua_gc
#define lua_error __lua_proxy__()->lua_error
#define lua_next __lua_proxy__()->lua_next
#define lua_concat __lua_proxy__()->lua_concat
#define lua_len __lua_proxy__()->lua_len
#define lua_stringtonumber __lua_proxy__()->lua_stringtonumber
#define lua_getallocf __lua_proxy__()->lua_getallocf
#define lua_setallocf __lua_proxy__()->lua_setallocf
#define lua_getstack __lua_proxy__()->lua_getstack
#define lua_getinfo __lua_proxy__()->lua_getinfo
#define lua_getlocal __lua_proxy__()->lua_getlocal
#define lua_setlocal __lua_proxy__()->lua_setlocal
#define lua_getupvalue __lua_proxy__()->lua_getupvalue
#define lua_setupvalue __lua_proxy__()->lua_setupvalue
#define lua_upvalueid __lua_proxy__()->lua_upvalueid
#define lua_upvaluejoin __lua_proxy__()->lua_upvaluejoin
#define lua_sethook __lua_proxy__()->lua_sethook
#define lua_gethook __lua_proxy__()->lua_gethook
#define lua_gethookmask __lua_proxy__()->lua_gethookmask
#define lua_gethookcount __lua_proxy__()->lua_gethookcount
#define luaopen_base __lua_proxy__()->luaopen_base
#define luaopen_coroutine __lua_proxy__()->luaopen_coroutine
#define luaopen_table __lua_proxy__()->luaopen_table
#define luaopen_io __lua_proxy__()->luaopen_io
#define luaopen_os __lua_proxy__()->luaopen_os
#define luaopen_string __lua_proxy__()->luaopen_string
#define luaopen_utf8 __lua_proxy__()->luaopen_utf8
#define luaopen_bit32 __lua_proxy__()->luaopen_bit32
#define luaopen_math __lua_proxy__()->luaopen_math
#define luaopen_debug __lua_proxy__()->luaopen_debug
#define luaopen_package __lua_proxy__()->luaopen_package
#define luaL_checkversion_ __lua_proxy__()->luaL_checkversion_
#define luaL_getmetafield __lua_proxy__()->luaL_getmetafield
#define luaL_callmeta __lua_proxy__()->luaL_callmeta
#define luaL_tolstring __lua_proxy__()->luaL_tolstring
#define luaL_argerror __lua_proxy__()->luaL_argerror
#define luaL_checklstring __lua_proxy__()->luaL_checklstring
#define luaL_optlstring __lua_proxy__()->luaL_optlstring
#define luaL_checknumber __lua_proxy__()->luaL_checknumber
#define luaL_optnumber __lua_proxy__()->luaL_optnumber
#define luaL_checkinteger __lua_proxy__()->luaL_checkinteger
#define luaL_optinteger __lua_proxy__()->luaL_optinteger
#define luaL_checkstack __lua_proxy__()->luaL_checkstack
#define luaL_checktype __lua_proxy__()->luaL_checktype
#define luaL_checkany __lua_proxy__()->luaL_checkany
#define luaL_newmetatable __lua_proxy__()->luaL_newmetatable
#define luaL_setmetatable __lua_proxy__()->luaL_setmetatable
#define luaL_testudata __lua_proxy__()->luaL_testudata
#define luaL_checkudata __lua_proxy__()->luaL_checkudata
#define luaL_where __lua_proxy__()->luaL_where
#define luaL_error __lua_proxy__()->luaL_error
#define luaL_checkoption __lua_proxy__()->luaL_checkoption
#define luaL_fileresult __lua_proxy__()->luaL_fileresult
#define luaL_execresult __lua_proxy__()->luaL_execresult
#define luaL_ref __lua_proxy__()->luaL_ref
#define luaL_unref __lua_proxy__()->luaL_unref
#define luaL_loadfilex __lua_proxy__()->luaL_loadfilex
#define luaL_loadbufferx __lua_proxy__()->luaL_loadbufferx
#define luaL_loadstring __lua_proxy__()->luaL_loadstring
#define luaL_newstate __lua_proxy__()->luaL_newstate
#define luaL_len __lua_proxy__()->luaL_len
#define luaL_gsub __lua_proxy__()->luaL_gsub
#define luaL_setfuncs __lua_proxy__()->luaL_setfuncs
#define luaL_getsubtable __lua_proxy__()->luaL_getsubtable
#define luaL_traceback __lua_proxy__()->luaL_traceback
#define luaL_requiref __lua_proxy__()->luaL_requiref
#define luaL_buffinit __lua_proxy__()->luaL_buffinit
#define luaL_prepbuffsize __lua_proxy__()->luaL_prepbuffsize
#define luaL_addlstring __lua_proxy__()->luaL_addlstring
#define luaL_addstring __lua_proxy__()->luaL_addstring
#define luaL_addvalue __lua_proxy__()->luaL_addvalue
#define luaL_pushresult __lua_proxy__()->luaL_pushresult
#define luaL_pushresultsize __lua_proxy__()->luaL_pushresultsize
#define luaL_buffinitsize __lua_proxy__()->luaL_buffinitsize
#define luaL_pushmodule __lua_proxy__()->luaL_pushmodule
#define luaL_openlib __lua_proxy__()->luaL_openlib
#define luaL_openlibs __lua_proxy__()->luaL_openlibs

#else

static LuaProxy* __lua_proxy_impl__()
{
	static LuaProxy __impl__{
		lua_newstate,
		lua_close,
		lua_newthread,
		lua_atpanic,
		lua_version,
		lua_absindex,
		lua_gettop,
		lua_settop,
		lua_pushvalue,
		lua_rotate,
		lua_copy,
		lua_checkstack,
		lua_xmove,
		lua_isnumber,
		lua_isstring,
		lua_iscfunction,
		lua_isinteger,
		lua_isuserdata,
		lua_type,
		lua_typename,
		lua_tonumberx,
		lua_tointegerx,
		lua_toboolean,
		lua_tolstring,
		lua_rawlen,
		lua_tocfunction,
		lua_touserdata,
		lua_tothread,
		lua_topointer,
		lua_arith,
		lua_rawequal,
		lua_compare,
		lua_pushnil,
		lua_pushnumber,
		lua_pushinteger,
		lua_pushlstring,
		lua_pushstring,
		lua_pushvfstring,
		lua_pushfstring,
		lua_pushcclosure,
		lua_pushboolean,
		lua_pushlightuserdata,
		lua_pushthread,
		lua_getglobal,
		lua_gettable,
		lua_getfield,
		lua_geti,
		lua_rawget,
		lua_rawgeti,
		lua_rawgetp,
		lua_createtable,
		lua_newuserdata,
		lua_getmetatable,
		lua_getuservalue,
		lua_setglobal,
		lua_settable,
		lua_setfield,
		lua_seti,
		lua_rawset,
		lua_rawseti,
		lua_rawsetp,
		lua_setmetatable,
		lua_setuservalue,
		lua_callk,
		lua_pcallk,
		lua_load,
		lua_dump,
		lua_yieldk,
		lua_resume,
		lua_status,
		lua_isyieldable,
		lua_gc,
		lua_error,
		lua_next,
		lua_concat,
		lua_len,
		lua_stringtonumber,
		lua_getallocf,
		lua_setallocf,
		lua_getstack,
		lua_getinfo,
		lua_getlocal,
		lua_setlocal,
		lua_getupvalue,
		lua_setupvalue,
		lua_upvalueid,
		lua_upvaluejoin,
		lua_sethook,
		lua_gethook,
		lua_gethookmask,
		lua_gethookcount,
		luaopen_base,
		luaopen_coroutine,
		luaopen_table,
		luaopen_io,
		luaopen_os,
		luaopen_string,
		luaopen_utf8,
		luaopen_bit32,
		luaopen_math,
		luaopen_debug,
		luaopen_package,
		luaL_checkversion_,
		luaL_getmetafield,
		luaL_callmeta,
		luaL_tolstring,
		luaL_argerror,
		luaL_checklstring,
		luaL_optlstring,
		luaL_checknumber,
		luaL_optnumber,
		luaL_checkinteger,
		luaL_optinteger,
		luaL_checkstack,
		luaL_checktype,
		luaL_checkany,
		luaL_newmetatable,
		luaL_setmetatable,
		luaL_testudata,
		luaL_checkudata,
		luaL_where,
		luaL_error,
		luaL_checkoption,
		luaL_fileresult,
		luaL_execresult,
		luaL_ref,
		luaL_unref,
		luaL_loadfilex,
		luaL_loadbufferx,
		luaL_loadstring,
		luaL_newstate,
		luaL_len,
		luaL_gsub,
		luaL_setfuncs,
		luaL_getsubtable,
		luaL_traceback,
		luaL_requiref,
		luaL_buffinit,
		luaL_prepbuffsize,
		luaL_addlstring,
		luaL_addstring,
		luaL_addvalue,
		luaL_pushresult,
		luaL_pushresultsize,
		luaL_buffinitsize,
		luaL_pushmodule,
		luaL_openlib,
		luaL_openlibs
	};
	return &__impl__;
}
#define luaopen_luadbg(L) _luaopen_luadbg(&__lua_proxy_impl__,L)

#endif // USE_LUA_AS_PROXY