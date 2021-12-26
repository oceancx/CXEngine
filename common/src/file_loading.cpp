#include "file_loading.h"
#include <NESupport.h>


FileLoadingThread::FileLoadingThread()
{
	m_Running = true;
	m_This = std::thread(std::bind(&FileLoadingThread::Run, this));
}
FileLoadingThread::~FileLoadingThread()
{
	{
		std::lock_guard<std::mutex> locker(m_Lock);
		m_Running = false;
		m_Cond.notify_one();
	}
	m_This.join();
}

void FileLoadingThread::PostTask(const char* cpath, std::function<bool(const char*)> fileparser)
{
	std::lock_guard<std::mutex> locker(m_Lock);
	std::string path(cpath);
	auto it = m_TaskStates.find(path);
	if (it == m_TaskStates.end())
	{
		m_TaskQueue.emplace_back(path, fileparser);
		m_TaskStates.insert({ path, TaskState({ true,false }) });
		m_Cond.notify_one();
	}
	else
	{
		if (it->second.Loading) return;
		if (it->second.Ok) return;
		m_TaskStates.erase(it);
		m_TaskQueue.emplace_back(path, fileparser);
		m_TaskStates.insert({ path, TaskState({ true,false }) });
		m_Cond.notify_one();
	}
}

void FileLoadingThread::Run()
{
	while (true)
	{
		std::unique_lock<std::mutex> ulocker(m_Lock);
		m_Cond.wait(ulocker, [this]() {return !m_Running || !m_TaskQueue.empty(); });
		if (!m_Running)
		{
			ulocker.unlock();
			return;
		}
		auto task = m_TaskQueue.front();
		m_TaskQueue.pop_front();
		ulocker.unlock();
		bool ok = task.Parser(task.Path.c_str());

		auto it = m_TaskStates.find(task.Path);
		if (it != m_TaskStates.end())
		{
			it->second.Loading = false;
			it->second.Ok = ok;
		}
	}
}
//only can call from main thread
bool FileLoadingThread::IsFileLoading(const char* cpath)
{
	std::lock_guard<std::mutex> locker(m_Lock);
	std::string path(cpath);
	auto it = m_TaskStates.find(path);
	if (it == m_TaskStates.end())return false;
	return it->second.Loading;
}

bool FileLoadingThread::IsFileLoadOk(const char* cpath)
{
	std::lock_guard<std::mutex> locker(m_Lock);
	std::string path(cpath);
	auto it = m_TaskStates.find(path);
	if (it == m_TaskStates.end())return false;
	return it->second.Ok;
}


void FileLoadingThread::ResetTaskState(const char* cpath)
{
	std::lock_guard<std::mutex> locker(m_Lock);
	std::string path(cpath);
	auto it = m_TaskStates.find(path);
	if (it != m_TaskStates.end())
	{
		it->second.Loading = false;
		it->second.Ok = false;
	}

}

void FileLoadingThread::RemoveTaskState(const char* cpath)
{
	std::lock_guard<std::mutex> locker(m_Lock);
	std::string path(cpath);
	auto state_it = m_TaskStates.find(path);
	if (state_it != m_TaskStates.end())
	{
		if (state_it->second.Loading == true && state_it->second.Ok == false)
		{
			for (std::deque<Task>::iterator it = m_TaskQueue.begin(); it != m_TaskQueue.end(); ++it)
			{
				if (it->Path == std::string(cpath))
				{
					m_TaskQueue.erase(it);
					break;
				}
			}
		}
		m_TaskStates.erase(state_it);
	}
}

static FileLoadingThread* s_FileLoadingThread;
FileLoadingThread* file_loading_thread()
{
	if (s_FileLoadingThread == nullptr)
	{
		s_FileLoadingThread = new FileLoadingThread();
	}
	return s_FileLoadingThread;
}

void luaopen_ne_support(lua_State*L)
{

}