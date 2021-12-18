#pragma once
#include <script_system.h>
//thread load files, use filepath and user defined parse function to do files loading
// report loading progress and when done notify main thread.
// cache files 
class FileLoadingThread
{
public:
	struct Task
	{
		std::string Path;
		std::function<bool(const char*)> Parser;
		Task(std::string path, std::function<bool(const char*)> parser)
		{
			Path = path;
			Parser = parser;
		}
	};

	struct TaskState
	{
		bool Loading;
		bool Ok;
		TaskState() = default;
	};

	FileLoadingThread();
	~FileLoadingThread();

	void PostTask(const char* cpath, std::function<bool(const char*)> fileparser);

	void Run();
	//only can call from main thread
	bool IsFileLoading(const char* cpath);

	bool IsFileLoadOk(const char* cpath);
	void ResetTaskState(const char* cpath);
	void RemoveTaskState(const char* cpath);
	
private:
	std::mutex m_Lock;
	std::mutex m_StateLock;
	std::condition_variable m_Cond;
	std::thread m_This;

	std::deque<Task> m_TaskQueue;
	std::set<std::string> m_FilterSet;
	std::map<std::string, TaskState> m_TaskStates;
	bool m_Running;

};

FileLoadingThread* file_loading_thread();


void luaopen_ne_support(lua_State*L);