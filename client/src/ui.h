#pragma once
#include "pos.h"

class Texture;
class InputManager;

class View
{
public:
	friend InputManager;

	virtual bool OnClick(int button, int x, int y) { return false; };
	virtual Bound GetViewBounds() { return Bound({ X,X + Width,Y,Y + Height }); }
	virtual int GetViewLayer()const { return Layer; };

	virtual bool CheckDrag(int dx, int dy) {
		return EnableDrag && pow(dx, 2) + pow(dy, 2) >= 16;
	};
	virtual void OnDragStart() { };

	virtual void OnDragMove(int dx, int dy) {
		X += dx;
		Y += dy;
	};
	virtual void OnDragEnd() {};
	virtual void OnHover(float x,float y) {};
	virtual void OnGlobleClick(int x, int y) {};
	virtual void OnCharacterInput(uint32_t charcode) {};
	virtual void OnFocusChanged(bool focus) {};

	virtual void OnKeyDownEvent(int keyCode) {};
	virtual void OnKeyUpEvent(int keyCode) {};
	virtual void OnKeyRepeatEvent(int keyCode) {};

	View() :dragStart(false), pressed(false), dragX(-1), dragY(-1), button(-1) { }
	virtual ~View() {};
	
	bool Visible = true;
	float X = 0;
	float Y = 0;
	float Width = 0;
	float Height = 0;
	int Layer = 1;
	bool EnableDrag = false;
private:
	void SetDragStart(bool isstart) { dragStart = isstart; }
	bool IsDragStart() { return dragStart; }
	void SetPressed(bool ispress, int button) { pressed = ispress; this->button = button; }
	bool IsPressed() { return pressed; }
	int GetPressedButton() { return button; }

	bool dragStart;
	bool pressed;
	int button;
	int dragX;
	int dragY;
	
};

class ImageView : public View
{
public:
	uint64_t BackgroundResID;
	Texture* Background;
	ImageView(uint32_t pkg, uint32_t wasID);
	virtual ~ImageView();

	void OnUpdate();

	void OnDraw();
	
	bool OnClick(int button, int x, int y) override;



	void OnHover(float x, float y) override;
private:
	float m_HoverX;
	float m_HoverY;
};


void luaopen_ui(lua_State* L);