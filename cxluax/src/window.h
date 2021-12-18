#pragma once

#include "singleton.h"

class Window : public Singleton<Window>
{
public:
	Window();
	~Window();

	void Init(int w, int h);
	void Destroy();
	void Show();
	int GetRenderTexture();
	GLFWwindow* GetGLFWwindow();
	int GetWidth();
	int GetHeight();
	int GetWindowWidth();
	int GetWindowHeight();
	float GetCenterX();
	float GetCenterY();
	float GetDeltaTime();
	float GetDeltaTimeMilliseconds();
	void OnFrameBufferSizeCallback(int width, int height);
	float GetFPS();

	int GetFrameBuffer();

};


#define WINDOW_INSTANCE Window::GetInstance()

void luaopen_window(lua_State* L);

