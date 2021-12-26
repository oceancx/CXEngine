#pragma once
	
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdarg>
class Logger final : public Singleton<Logger>
{
public:
	static void Error(std::string msg) { std::cout<<"Error:"<<msg<<std::endl; }
	static void Warning(std::string msg) { std::cout<<"Warning:"<<msg<<std::endl; }
	static void Print(const char *format, ...);

	static void XXXPrint(int level,std::string log,std::string x) {
		std::cout << "LogPrint:" << log << std::endl;
	}

public:
	Logger() {}
	~Logger(){};
	
};

#define LOG_PRINT(format,...) Logger::GetInstance()->Print(format,__VA_ARGS__)

#define cxlog_info(format,...) Logger::GetInstance()->Print(format,__VA_ARGS__)

#define cxlog_err(format,...) Logger::GetInstance()->Print(format,__VA_ARGS__)

#define cxlog_warn(format,...) Logger::GetInstance()->Print(format,__VA_ARGS__)

enum struct LogLevel : int
{
	Info,
	Warning,
	Error,
	Debug
};

// #define LOG(...) \
// 	Logger::XXXPrint( \
// 		__VA_ARGS__) 

static const std::string STARENGINE_LOG_TAG = "STARENGINE";
void luaopen_logger(lua_State* L);
