#pragma once

#include "singleton.h"
#define  PROFILER_SCOPE(msg,code)  \
Profiler::GetInstance()->Start(msg);	\
	code	\
Profiler::GetInstance()->End()

class Profiler :public Singleton<Profiler>
{
public:
	Profiler();
	~Profiler();
	void Start(std::string msg);
	void End();
	
private:
	std::deque<uint64_t> start_time_q;
	std::deque<std::string> msg_q;
};
