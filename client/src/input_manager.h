#pragma once

#include "singleton.h"
#include <functional>
#include "pos.h"
#include "ui.h"
#include "camera.h"

struct MousePos
{
	float x,y;
	MousePos(float x,float y) { this->x = x ; this->y = y ;}
};


struct ViewCompare
{
	bool operator()(View* lhs, View* rhs)  const
	{
		int layer1 = lhs->GetViewLayer();
		int layer2 = rhs->GetViewLayer();

		if (layer1 == layer2)
		{
			return lhs < rhs;
		}
		else
		{
			return layer1 < layer2;
		}
	}
};

class InputManager final : public Singleton<InputManager>
{
public:
	friend Singleton<InputManager>;  // cause private ctor dtor

	
	IntPos GetMouseIntPos();
	Pos GetMousePos();

	void Init();
	
	bool IsKeyDown(int keyCode) { return m_Keys[keyCode]; }

    bool IsKeyUp(int keyCode) { return m_Keys[keyCode]; }

	int GetKeyState(int keyCode) { return m_KeyStates[keyCode]; }

    float GetMouseX() { return m_MousePos.x; }
	float GetMouseY() { return m_MousePos.y; }

	void RegisterView(View*v);
	void UnRegisterView(View*v);
	void RegisterOnKeyClickEvent(int keyCode, std::function<void()> callback) { m_ClickEvents[keyCode].clear(); m_ClickEvents[keyCode].push_back(callback); }
	void RegisterOnKeyDownEvent(int keyCode, std::function<void()> callback) { m_KeyDownEvents[keyCode].clear(); m_KeyDownEvents[keyCode].push_back(callback); }
	void RegisterOnKeyUpEvent(int keyCode, std::function<void()> callback) { m_KeyUpEvents[keyCode].clear(); m_KeyUpEvents[keyCode].push_back(callback); }

	void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void KeyCallbackFunc(GLFWwindow* window, int key, int scancode, int action, int mode);
	void ScrollCallbackFunc(GLFWwindow* window, float xoffset, float yoffset);
	void MouseCallbackFunc(GLFWwindow* window, float xpos, float ypos);
	void CharacterInputCallback(GLFWwindow* window, unsigned int charcode);
	void RequestFocus(View* view);

	void SetCamera(Camera* camera);
	void SetWindowPos(float x, float y);
	void GetWindowPos(float& x, float& y);
private:
	
	InputManager();
	~InputManager();

	GLFWwindow* m_pWindow;

	std::vector<bool> m_Keys;

	bool m_bKeyBlock;

	std::vector<int>  m_KeyStates;

	bool m_FirstMouse;

	Pos m_WindowPos;
	Pos m_MousePos;
	std::map<int, std::vector<std::function<void()>>> m_ClickEvents;
	std::map<int, std::vector<std::function<void()>>> m_KeyDownEvents;
	std::map<int, std::vector<std::function<void()>>> m_KeyUpEvents;

	std::set<View*,ViewCompare> m_RegisterViews;
	View* m_FocusView;
	Camera* m_Camera;
};

#define INPUT_MANAGER_INSTANCE InputManager::GetInstance()

void luaopen_input_manager(lua_State* L);