#include <iostream>
#include <luadbg.h>
#include <string>

std::string PATH_SEP("");
std::string CWD = "";
int port = 4711;

std::string EXTENSION_DIR(const char* dir)
{
	return (CWD + dir);
}


void  argv_init(int argc, char* argv[])
{
	for (int i = 0; i < argc; i++)
	{
		std::string param = argv[i];

		if (param.find("--port=") != std::string::npos) {
			std::string str = param.substr(param.find_last_of("=") + 1);
			port = std::stoi(str);
		}
		else if (param.find("--cwd=") != std::string::npos) {
			CWD = param.substr(param.find_last_of("=") + 1);
		}
		else if (i == 0) {
			if (param.find_last_of("\\") != std::string::npos) {
				PATH_SEP = "\\";
			}
			else if (param.find_last_of("/") != std::string::npos)
			{
				PATH_SEP = "/";
			}
			std::string str = param.substr(0, param.find_last_of(PATH_SEP));
			CWD = str + PATH_SEP;
		}
		std::cerr << "arg " << i << ":" << argv[i] << std::endl;
	}
	std::cerr << "workdir = " << CWD << "   port = " << port << std::endl;
}


int main(int argc ,char** argv)
{
	argv_init(argc, argv);
	lua_State * L = luaL_newstate();
	luaL_openlibs(L); 
	luaopen_luadbg(L);
	luaL_dofile(L, EXTENSION_DIR("main.lua").c_str());
	
	printf("%s\\n", lua_tostring(L, -1));

	return 0;
}