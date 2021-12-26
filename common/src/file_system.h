#pragma once


#include "define_types.h"


class FileSystem 
{
public:
	FileSystem();
	~FileSystem();

	static std::string GetPath();
	static std::string MakePath(String rpath);
	static std::string FormatPath(String path);
	static String GetTSVPath(String name);
	static String GetTablePath(String name);
	static std::string GetAbsPath(std::string localPath);
	static std::string GetResourcePath(std::string localPath);
	static std::string GetAssetsPath(std::string path);
	static std::string GetShaderPath(std::string path);
	static std::string GetLuaPath(std::string path);
	static std::string GetWDFPath(std::string path);
	static std::string GetMapPath(std::string path);
	static std::string GetFontPath(std::string path);
	static std::string GetGameFontPath();
	static std::string GetIconPath(std::string path);
	static void InitWorkPath();

};

void luaopen_filesystem(lua_State*L);
