#include "ui.h"
#include "input_manager.h"
#include "resource_manager.h"
#include "utils.h"
#include "window.h"
#include "sprite_renderer.h"
#include "scene/scene_manager.h"
#include "file_system.h"


ImageView::ImageView(uint32_t pkg, uint32_t wasID)
{
	INPUT_MANAGER_INSTANCE->RegisterView(this);
	Background = nullptr;
	BackgroundResID = RESOURCE_MANAGER_INSTANCE->EncodeWAS(pkg, wasID);
	X = Y = Width = Height = 0;
	m_HoverX = m_HoverY = 0;
}

ImageView::~ImageView()
{
	INPUT_MANAGER_INSTANCE->UnRegisterView(this);
	SafeDelete(Background);
}

void ImageView::OnUpdate()
{
	auto backgroud = RESOURCE_MANAGER_INSTANCE->LoadWASSpriteByID(BackgroundResID);
	if (backgroud != nullptr && Background == nullptr)
	{
		auto& frame = backgroud->Frames[0];
		Background = new Texture(frame.Width, frame.Height, true, (uint8*)frame.Src.data());
		Width = (float)frame.Width;
		Height = (float)frame.Height;
		X = (float)(WINDOW_INSTANCE->GetCenterX() - frame.Width / 2);
		Y = (float)(WINDOW_INSTANCE->GetCenterY() - frame.Height / 2);
	}
}

void ImageView::OnDraw()
{
	// drawBackgound
	if (Background != nullptr)
		SPRITE_RENDERER_INSTANCE->DrawFrameSprite(
			Background,
			{ X ,Y },
			{ Width,Height });
}

bool ImageView::OnClick(int button, int x, int y)
{
	/*dynamic_cast<Scene*>(SCENE_MANAGER_INSTANCE->GetCurrentScene())->
		OnSmapClick(m_HoverX, m_HoverY, (float)Width - 18.f * 2,( float)Height - 15.f * 2);*/
	return true;
}


void ImageView::OnHover(float x, float y)
{
	std::cout << "OnHover " << x - X - 18 << " : " << y - Y - 15 << std::endl;
	m_HoverX = x - X - 18;
	m_HoverY = y - Y - 15;
}


void luaopen_ui(lua_State* L)
{
}