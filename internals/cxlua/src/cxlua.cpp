#include "cxlua.h"
#include "tsv.h"

std::string DEFAULT_CWD = "";
bool check_lua_error(lua_State* L, int res, const char* func)
{
	if (res != LUA_OK) {
		luaL_traceback(L, L, lua_tostring(L, -1), 0);
		const char* errmsg = lua_tostring(L, -1);
		printf("%s\npcall error:\t[func]%s\n", errmsg, func);
		return false;
	}
	return true;
}


std::map<std::string, std::string> g_CmdArgs;
void handle_command_args(int argc,char *argv[])
{
	g_CmdArgs["argv0"] = argv[0];
	for (int i = 1; i < argc; i++)
	{
		std::string argi = argv[i];
		printf("%s\n", argi.c_str());

		if (argi.substr(0, 2) == "--") {
			argi = argi.substr(2, argi.size());
			auto kv = utils::split(argi, '=');
			g_CmdArgs[kv[0]] = kv[1];
		}
		else if (argi.substr(0, 1) == "-") {
			argi = argi.substr(1, argi.size());
			g_CmdArgs[argi] = "";
		}
	}
}

bool command_arg_check(const char* key){
	return g_CmdArgs.find(key) != g_CmdArgs.end();
}

const char* command_arg_get(const char* key)
{
	if (g_CmdArgs.find(key) == g_CmdArgs.end()) {
		return nullptr;
	}else{
		return g_CmdArgs[key].c_str();
	}
}

const char* command_arg_opt_str(const char* key, const char* def){
	auto arg = command_arg_get(key);
	if (arg) {
		return arg;
	}
	else {
		return def;
	}
}

int command_arg_opt_int(const char* key, int def)
{
	auto arg = command_arg_get(key);
	if (arg) {
		int intarg = std::stoi(arg);
		return intarg;
	}
	else {
		return def;
	}
}


void init_default_cwd(const char* arg0)
{
	std::string PATH_SEP = "";
	std::string param(arg0);
	if (param.find_last_of("\\") != std::string::npos) {
		PATH_SEP = "\\";
	}
	else if (param.find_last_of("/") != std::string::npos)
	{
		PATH_SEP = "/";
	}
	DEFAULT_CWD = param.substr(0, param.find_last_of(PATH_SEP))  + PATH_SEP;
}
std::string get_default_cwd()
{
	return  DEFAULT_CWD;
}
int lua_get_default_cwd(lua_State*L )
{
	lua_pushstring(L,get_default_cwd().c_str());
	return 1;
}

static int s_EnumCounter = 0;
int enum_reset(){
	s_EnumCounter = 0;
	return s_EnumCounter;
}

int enum_next() {
	return ++s_EnumCounter;
}


#define luaL_requirelib(L,name,fn) (luaL_requiref(L, name, fn, 1),lua_pop(L, 1))
extern "C" int luaopen_cjson(lua_State * L);

void luaopen_cxlua(lua_State* L)
{
	luaL_requirelib(L, "cjson", luaopen_cjson);
	luaopen_tsv(L);
	luaopen_net_thread_queue(L);
	luaopen_netlib(L);
	
	script_system_register_function(L, enum_reset);
	script_system_register_function(L, enum_next);
	
	script_system_register_function(L, command_arg_check);
	script_system_register_function(L, command_arg_get);
	script_system_register_function(L, command_arg_opt_str);
	script_system_register_function(L, command_arg_opt_int);

	script_system_register_luac_function_with_name(L, "get_default_cwd", lua_get_default_cwd);
}
