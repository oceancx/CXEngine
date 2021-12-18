#include "logger.h"
#include <script_system.h>
#include "utils.h"

void log_print(const char *str) 
{
	printf("%s\n", utils::Utf8ToGB2312(str).c_str());
}
int lua_cxlog(lua_State*L){
	int n = lua_gettop(L);  /* number of arguments */
	int i;
	lua_getglobal(L, "tostring");
	for (i = 1; i <= n; i++) {
		size_t l;
		lua_pushvalue(L, -1);  /* function to be called */
		lua_pushvalue(L, i);   /* value to print */
		lua_call(L, 1, 1);
		const char* s = lua_tolstring(L, -1, &l);  /* get result */
		if (s == NULL)
			return luaL_error(L, "'tostring' must return a string to 'print'");
		if (i > 1) lua_writestring("\t", 1);
		std::string str = utils::Utf8ToGB2312(s);
		lua_writestring(str.data(), str.size());
		lua_pop(L, 1);  /* pop result */
	}
	lua_writeline();
	return 0;
}

int lua_cxlog_info(lua_State* L)
{
	return lua_cxlog(L);
}

int lua_cxlog_err(lua_State* L)
{
	return lua_cxlog(L);
}

int lua_cxlog_warn(lua_State* L)
{
	return lua_cxlog(L);
}

void Logger::Print(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
}

void luaopen_logger(lua_State* L) {
	script_system_register_function(L, log_print);
	script_system_register_luac_function_with_name(L, "cxlog_info", lua_cxlog_info);
	script_system_register_luac_function_with_name(L, "cxlog_err", lua_cxlog_info);
	script_system_register_luac_function_with_name(L, "cxlog_warn", lua_cxlog_info);
}
