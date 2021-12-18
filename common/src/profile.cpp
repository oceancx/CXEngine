#include "profile.h"
#include "Time/Time.h"
#include "logger.h"


Profiler::Profiler()
{
}

Profiler::~Profiler()
{



}
void Profiler::Start(std::string msg)
{
	auto start_time = Time::GetCurrentTime();
	start_time_q.push_back(start_time);
	msg_q.push_back(msg);
}
void Profiler::End()
{
	auto start_time = start_time_q.back();
	start_time_q.pop_back();
	auto msg = msg_q.back();
	msg_q.pop_back();
	

	auto dur = Time::GetCurrentTime() - start_time;
	Logger::Print("[Profiler]:%s. dur=%ldms\n", msg.c_str(), dur);
}