#pragma once
#include <map>
#include "timer.h"
#include <functional>
#include "singleton.h"

class Timer;

class TimerManager final: public Singleton<TimerManager>
{
public:
	TimerManager();
	~TimerManager();

	void Update(float64 dt);

	bool CreateTimer(const std::string& name, float32 targetTime, std::function<void()> func, bool loop = false, bool countingDown = true, bool paused = false);
	bool RemoveTimer(const std::string & name);
	void PauseTimer(const std::string & name, bool paused);
	void SetCountingDownTimer(const std::string & name, bool countingDown);
	void SetLoopTimer(const std::string & name, bool looping);
	void ResetTimer(const std::string & name, bool paused = false);
	void SetTargetTimeTimer(const std::string & name, float32 targetTime, bool reset = true, bool paused = false);
	void SetFunctionTimer(const std::string & name, const std::function<void()> & func);
	float64 ForceEndTimer(const std::string & name);
	void ForwardTimer(const std::string & name, float64 time);
	void ForwardAllTimers(float64 time);

	int32 GetTimerMinutes(const std::string & name) const;
	int32 GetTimerSeconds(const std::string & name) const;
	int32 GetTimerTotalSeconds(const std::string & name) const;
	float64 GetTimerTargetTime(const std::string & name) const;
	float64 GetTimerAccurateTime(const std::string & name) const;

private:
	std::map<const std::string, ::Timer> m_TimerContainer;
	std::map<const std::string, ::Timer> m_GarbageContainer;

};

#define TIMER_MANAGER_INTANCE TimerManager::GetInstance()

void luaopen_timer_manager(lua_State* L);