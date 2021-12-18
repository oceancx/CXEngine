#pragma once
#include "singleton.h"
#include <lua.hpp>
#include <NESupport.h>
#include "resource_manager.h"
#include "utils.h"
#include "nanovg.h"
#include "pos.h"
#include "ui.h"

class BaseSprite;

class UIObject : public View
{
public:
	virtual void Draw() {};
	bool MarkRemove = false;
};

class NEImageView : public UIObject
{
public:
	NEImageView(uint64_t resoureID = 0, std::vector<PalSchemePart> * patMatrix = nullptr);
	NEImageView(uint32_t pkg, uint32_t wasID, std::vector<PalSchemePart>* patMatrix = nullptr) : NEImageView(RESOURCE_MANAGER_INSTANCE->EncodeWAS(pkg, wasID), patMatrix) {};
	~NEImageView();

	virtual void Draw() override;
	BaseSprite* GetBaseSprite() { return m_pBS; }
private:
	map<int, int> m_ImageIDs;
	BaseSprite* m_pBS;
};

class UITextView : public UIObject 
{
public:
	UITextView() ;
	~UITextView() {};
	virtual void Draw() override;
	void SetText(const char* txt, float x, float y, int align, float wrapWidth);
	CXString Text;
	CXString Font;
	float Size;
	NVGcolor Color;
	NVGcolor BGColor;
	int Align;
	float WrapWidth = 0;
	float LineHeight = 0;
};
class NPCDialog;

class UIRenderer : public Singleton<UIRenderer>
{
public:
	UIRenderer();
	~UIRenderer();

	void Update();
	void Draw();
	
	void AddToDraw(UIObject* obj);
	void RemoveToDraw(UIObject* obj);
	void Clear();
	NVGcontext* GetContext();
	void Begin();
	void End();
	NPCDialog* GetDialog() { return m_Dialog; }
private:
	NPCDialog* m_Dialog;
	vector<UIObject*> m_Objects;
};

class NPCDialog :public UIObject
{
public:
	enum EShowMode{
		SHOW_PLOT_TEXT,
		SHOW_OPTIONS
	};

	struct Option
	{
		shared_ptr<UITextView>  tv;
		int funcRef;
	};

	NPCDialog();
	~NPCDialog();
	void Draw() override;
	void SetText(const char* txt);
	virtual Bound GetViewBounds() override;
	virtual bool OnClick(int button, int x, int y);
	void AddOption(string txt, int func);
	void UpdateOptionsLayout();
	void ClearOptions();
private:
	EShowMode m_ShowMode;
	NEImageView* m_FaceImg;
	NEImageView* m_TvBG;
	UITextView* m_Tv;

	vector<Option> m_Options;
};

void luaopen_ui_renderer(lua_State* L);