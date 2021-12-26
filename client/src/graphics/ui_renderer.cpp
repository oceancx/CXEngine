#include "ui_renderer.h"
#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"
#include "window.h"
#include "file_system.h"
#include "input_manager.h"
#include <script_system.h>
#include "animation/sprite.h"
#include "logger.h"
#include "cxmath.h"


static unordered_map<uint64_t, int> s_ImageCache;

uint64_t encode_image_cache_id(uint64_t resid, int index) 
{
	uint64_t index64 = index;
	uint64_t id = resid | index64 << 48;
	return id;
}

void decode_image_cache_id(uint64_t id, uint64_t& resid, int& index)
{
	resid = id & 0x00ffffff;
	index = id >> 48;
}

void decode_image_cache_id(uint64_t id, uint32_t& pack, uint32_t& wasid, int& index)
{
	uint64_t resid;
	decode_image_cache_id(id, resid, index);
	res_decode_was(resid, pack, wasid);
}

class UINEImageView  : public UIObject
{
public:
	UINEImageView(BaseSprite* sp) :m_BaseSprite(sp) {};
	~UINEImageView() {};
	void Draw() override;
private:
	BaseSprite* m_BaseSprite;
};

void UINEImageView::Draw()
{
	m_BaseSprite->Draw();
}


NVGcontext* vg = NULL;
UIRenderer::UIRenderer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, WINDOW_INSTANCE->GetFrameBuffer());

	vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	if (vg == NULL) {
		cxlog_info("Could not init nanovg.\n");
		return;
	}

	int res = 0;
	/*res = nvgCreateFont(vg, "MSYH", FileSystem::GetFontPath("msyh.ttf").c_str());
	assert(res >= 0);*/
	/*res = nvgCreateFont(vg, "MSHT", FileSystem::GetFontPath("msht.ttf").c_str());
	assert(res >= 0);*/
	res = nvgCreateFont(vg, "SIMSUN", FileSystem::GetFontPath("simsun.ttc").c_str());
	assert(res >= 0);
	m_Dialog = new NPCDialog();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

UIRenderer::~UIRenderer()
{
	SafeDelete(m_Dialog);
	nvgDeleteGL3(vg);
}

void UIRenderer::Update()
{

}

void UIRenderer::Draw()
{
	
	//script_system_call_function(script_system_get_luastate(), "on_ui_renderer_draw");

	for (UIObject*& obj:m_Objects){
		if (obj&&!obj->MarkRemove) {
			obj->Draw();
		}
		else {
			delete obj;
			obj = nullptr;
		}
	}
	if (m_Objects.size() > 0) {
		vector<UIObject*> tmp;
		for (UIObject*& obj : m_Objects) {
			if (obj&&!obj->MarkRemove) {
				tmp.push_back(obj);
			}
		}
		m_Objects.swap(tmp);
	}
	m_Dialog->Draw();
	
}

void UIRenderer::AddToDraw(UIObject* obj)
{
	m_Objects.push_back(obj);
}

void UIRenderer::RemoveToDraw(UIObject* obj)
{
	obj->MarkRemove = true;
}

void UIRenderer::Clear()
{
	for(auto*& obj : m_Objects){
		delete obj;
		obj = nullptr;
	}
	m_Objects.clear();
}

NVGcontext* UIRenderer::GetContext()
{
	return vg;
}
void UIRenderer::Begin()
{
	float width = (float)WINDOW_INSTANCE->GetWidth();
	float height = (float)WINDOW_INSTANCE->GetHeight();
	nvgBeginFrame(vg, width, height, width* 1.0f / height);
}
void UIRenderer::End()
{
	nvgEndFrame(vg);
}
NEImageView::NEImageView(uint64_t resoureID /*= 0*/, std::vector<PalSchemePart>* patMatrix /*= nullptr*/)
{
	m_pBS = new BaseSprite(resoureID, patMatrix);
}

NEImageView::~NEImageView()
{
	delete m_pBS;
}

void NEImageView::Draw()
{
	int index = m_pBS->Dir * m_pBS->GroupFrameCount + m_pBS->CurrentFrame;
	auto& frame = m_pBS->m_pSprite->Frames[index];

	uint64_t id = encode_image_cache_id(m_pBS->ResID, index);
	auto it = s_ImageCache.find(id);
	int imageid = 0;
	if (it == s_ImageCache.end()) {
		imageid = nvgCreateImageRGBA(vg, frame.Width, frame.Height, 0, (unsigned char*)frame.Src.data());
		s_ImageCache.insert({ id,imageid });
	}
	else {
		imageid = it->second;
	}
	nvgSave(vg);
	float left = m_pBS->Pos.x - frame.KeyX;
	float top = m_pBS->Pos.y - frame.KeyY;
	float w = (float)m_pBS->Width;
	float h = (float)m_pBS->Height;
	float rot = 0.0f / 180.0f * NVG_PI;
	float pad = 10;

	NVGpaint imgPaint = nvgImagePattern(vg, left, top, w, h, rot, imageid, 1);
	nvgBeginPath(vg);
	/*left += pad;
	top += pad;
	w -= pad * 2;
	h -= pad * 2;*/
	nvgRect(vg, left, top, w, h);
	nvgFillPaint(vg, imgPaint);
	nvgFill(vg);
	nvgRestore(vg);
}


UITextView::UITextView()
	:Text(""),
	Font("SIMSUN"),
	Size(14.f),
	Color(nvgRGBA(255, 255, 255, 255)),
	BGColor(nvgRGBA(0, 0, 0, 0)),
	Align(NVG_ALIGN_LEFT | NVG_ALIGN_TOP)
{
	nvgTextMetrics(vg, 0, 0, &Height);
}

void UITextView::Draw()
{
	if (Text.size() == 0)return;
	if (!Visible)return;
	nvgSave(vg);
	
	nvgFontSize(vg, Size);
	nvgFontFace(vg, Font.c_str()); 
	nvgTextAlign(vg, Align);

	const char* start = Text.data();
	const char* end = Text.data() + Text.size();
	if (WrapWidth!= 0) {
		nvgBeginPath(vg);

		nvgFillColor(vg, BGColor);
		nvgRoundedRect(vg, X - 8, Y - 8, Width + 16, Height + 16, 8.f);
		nvgFill(vg);

		NVGtextRow rows[3];
		int nrows, lnum = 0;
		float x, y;
		x = X;
		y = Y;
		while ((nrows = nvgTextBreakLines(vg, start, end, WrapWidth, rows, 3))) {
			for (int i = 0; i < nrows; i++) {
				NVGtextRow* row = &rows[i];

				nvgFillColor(vg, Color);
				nvgText(vg, x, y, row->start, row->end);
				nvgFillColor(vg, Color);
				nvgText(vg, x, y, row->start, row->end);

				lnum++;
				y += LineHeight;
			}
			start = rows[nrows - 1].next;
		}
	}
	else {
		nvgFillColor(vg, Color);
		nvgText(vg, X, Y, Text.c_str(), NULL);

		nvgFillColor(vg, Color);
		nvgText(vg, X, Y, Text.c_str(), NULL);
	}
	nvgRestore(vg);
}

void UITextView::SetText(const char* txt, float x, float y,int align,float wrapWidth)
{
	Text = txt;
	X = x;
	Y = y;
	Align = align;
	WrapWidth = wrapWidth;
	nvgTextMetrics(vg, NULL, NULL, &LineHeight);
	
	float bounds[4];
	nvgTextBoxBounds(vg, X, Y, WrapWidth, Text.c_str(), 0, bounds);
	
	Width = bounds[2] - bounds[0];
	Height = bounds[3] - bounds[1];

	if (Align & NVG_ALIGN_BOTTOM) {
		Y = Y - Height;
	}

	if (Align & NVG_ALIGN_CENTER) {
		X = X - Width / 2;
	}
}

NPCDialog::NPCDialog()
{
	m_ShowMode = SHOW_PLOT_TEXT;
	Visible = false;
	
	m_TvBG = new NEImageView(WZIFEWDF, 0x73D983B7);
	m_FaceImg = new NEImageView(WZIFEWDF, 0x7F84C945);
	m_Tv = new UITextView();
	
	m_Tv->Color = nvgRGB(255, 255, 255);
	m_Tv->Size = 20.f;
	 
	float w = (float)WINDOW_INSTANCE->GetWidth();
	auto* tvBGsp = m_TvBG->GetBaseSprite();
	X = (w - tvBGsp->Width) / 2;
	Y = 290;
	Width = (float)tvBGsp->Width;
	Height = (float)tvBGsp->Height;

	INPUT_MANAGER_INSTANCE->RegisterView(this);
}

NPCDialog::~NPCDialog()
{
	INPUT_MANAGER_INSTANCE->UnRegisterView(this);
	SafeDelete(m_TvBG);
	SafeDelete(m_Tv);
	SafeDelete(m_FaceImg);
	m_Options.clear();
}

void NPCDialog::Draw()
{
	if (Visible) {
		float w = (float)WINDOW_INSTANCE->GetWidth();
		float h = (float)WINDOW_INSTANCE->GetHeight();

		auto* tvBGsp = m_TvBG->GetBaseSprite();
		tvBGsp->Pos.x = X;
		tvBGsp->Pos.y = Y;

		auto* facesp = m_FaceImg->GetBaseSprite();
		facesp->Pos.x = X;
		facesp->Pos.y = Y - facesp->Height;

		m_FaceImg->Draw();
		m_TvBG->Draw();
		m_Tv->Draw();
		for (int i = 0; i < m_Options.size(); i++) {
			m_Options[i].tv->Draw();
		}
	}
}

void NPCDialog::SetText(const char* txt)
{
	auto* tvBGsp = m_TvBG->GetBaseSprite();
	float x = X + 16;
	float y = Y + 24;
	float wrapWidth = Width - 32.f;
	m_Tv->SetText(txt, x, y, NVG_ALIGN_TOP | NVG_ALIGN_LEFT, wrapWidth);
}

bool NPCDialog::OnClick(int button, int x, int y)
{
	if (m_Options.size() > 0) {
		for (int i = 0; i < m_Options.size(); i++) {
			Pos ps((float)x, (float)y);
			auto& bd = m_Options[i].tv->GetViewBounds();
			if (utils::BoundHitTest(bd, ps)) {
				lua_State*L = script_system_get_luastate();
				lua_rawgeti(L, LUA_REGISTRYINDEX, m_Options[i].funcRef);
				int res = lua_pcall(L, 0, 0, 0);
				check_lua_error(L, res);
				Visible = false;
				break;
			}
		}
		return true;
	}else{
		if (Visible) {
			Visible = false;
		}
		return true;
	}
}

void NPCDialog::AddOption(string txt, int func)
{
	Option op;
	op.funcRef = func;
	op.tv = make_shared<UITextView>();
	op.tv->Text = txt;
	op.tv->Size = 20.f;
	op.tv->Color = nvgRGB(198, 22, 24);
	m_Options.push_back(op);
}

void NPCDialog::UpdateOptionsLayout()
{
	float sx = X + 28;
	float sy = m_Tv->Y + m_Tv->Height + 9;
	for (int i = 0; i < (int)m_Options.size(); i++) {
		auto& op = m_Options[i];
		float bd[4];
		nvgTextBounds(vg, sx, sy, op.tv->Text.c_str(), 0, bd);
		op.tv->X = sx;
		op.tv->Y = sy;
		op.tv->Width = bd[2] - bd[0];
		op.tv->Height = bd[3] - bd[1];
		sy = sy + op.tv->Height + 4;
	}
}

void NPCDialog::ClearOptions()
{
	lua_State*L = script_system_get_luastate();
	for (auto& op : m_Options) {
		luaL_unref(L, LUA_REGISTRYINDEX, op.funcRef);
	}
	m_Options.clear();
}

Bound NPCDialog::GetViewBounds()
{
	auto* tvBGsp = m_TvBG->GetBaseSprite();
	if (tvBGsp == nullptr)return Bound({ 0,0,0,0 });

	Bound bound;
	bound.left = tvBGsp->Pos.x;
	bound.top = tvBGsp->Pos.y;
	bound.right = tvBGsp->Pos.x + tvBGsp->GetFrameWidth();
	bound.bottom = tvBGsp->Pos.y + tvBGsp->GetFrameHeight();
	return bound;
}

int ui_renderer_add_to_draw(lua_State*L){
	NEImageView* ptr = lua_check_pointer<NEImageView>(L, 1);
	UIRenderer::GetInstance()->AddToDraw(ptr);
	return 0;
}

int ne_imageview_get_base_sprite(lua_State*L){
	NEImageView* ptr = lua_check_pointer<NEImageView>(L, 1);
	BaseSprite* bs = ptr->GetBaseSprite();
	lua_push_base_sprite(L, bs);
	return 1;
}

luaL_Reg MT_NE_IMAGEVIEW[] =
{
	{ "GetBaseSprite",ne_imageview_get_base_sprite},
	{NULL,NULL}
};

int ne_imageview_create(lua_State* L)
{
	uint32_t pack = (uint32_t)lua_tointeger(L, 1);
	uint32_t wasid = (uint32_t)lua_tointeger(L, 2);

	NEImageView** ptr = (NEImageView * *)lua_newuserdata(L, sizeof(NEImageView));
	*ptr = new NEImageView(pack, wasid);
	if (luaL_newmetatable(L, "MT_NE_IMAGEVIEW")) {
		luaL_setfuncs(L, MT_NE_IMAGEVIEW, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2); 
	return 1;
}

int ui_textview_set_text(lua_State* L)
{
	UITextView* tv = lua_check_pointer<UITextView>(L, 1);
	const char* txt = lua_tostring(L, 2);
	tv->Text = txt;
	return 0;
}

int ui_textview_set_pos(lua_State* L)
{
	UITextView* tv = lua_check_pointer<UITextView>(L, 1);
	float x = (float) lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	tv->X = x;
	tv->Y = y;
	return 0;
}


int ui_textview_set_color(lua_State* L)
{
#define break_color_rgba(c)  nvgRGBA((c>>24)& 0xff,(c& 0xff0000) >> 16,  (c& 0xff00) >> 8, (c& 0xff))
	UITextView* tv = lua_check_pointer<UITextView>(L, 1);
	uint32_t color = (uint32_t)lua_tointeger(L, 2);
	tv->Color = break_color_rgba(color);
#undef break_color_rgba
	return 0;
}

int ui_textview_set_bg_color(lua_State* L)
{
#define break_color_rgba(c)  nvgRGBA((c>>24)& 0xff,(c& 0xff0000) >> 16,  (c& 0xff00) >> 8, (c& 0xff))
	UITextView* tv = lua_check_pointer<UITextView>(L, 1);
	uint32_t color = (uint32_t)lua_tointeger(L, 2);
	tv->BGColor = break_color_rgba(color);
#undef break_color_rgba
	return 0;
}

int ui_textview_set_text_size(lua_State* L)
{
	UITextView* tv = lua_check_pointer<UITextView>(L, 1);
	float size = (float)lua_tonumber(L, 2);
	tv->Size = size;
	return 0;
}

int ui_textview_set_width(lua_State*L){
	UITextView* tv = lua_check_pointer<UITextView>(L, 1);
	float width = (float)lua_tonumber(L, 2);
	tv->WrapWidth = width;
	return 0;
}


int ui_textview_draw(lua_State* L) {
	UITextView* tv = lua_check_pointer<UITextView>(L, 1);
	tv->Draw();
	return 0;
}


luaL_Reg MT_UI_TEXTVIEW[] = {
	{ "SetWidth", ui_textview_set_width},
	{ "SetText",ui_textview_set_text},
	{ "SetPos",ui_textview_set_pos},
	{ "SetColor",ui_textview_set_color},
	{ "SetBGColor",ui_textview_set_bg_color},
	{ "SetTextSize",ui_textview_set_text_size},
	{ "Draw",ui_textview_draw},
	{NULL,NULL}
};

int ui_textview_create(lua_State* L)
{
	lua_push_pointer(L, new UITextView());
	if (luaL_newmetatable(L, "MT_UI_TEXTVIEW")) {
		luaL_setfuncs(L, MT_UI_TEXTVIEW, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
	return 1;
}

int ui_textview_destroy(lua_State* L)
{
	auto* ptr = lua_check_pointer<UITextView>(L, 1);
	delete ptr;
	return 0;
}
void ui_renderer_clear(){
	UIRenderer::GetInstance()->Clear();
}

int npc_dialog_show(lua_State*L) {
	int argi = 1;
	auto* dlg = UIRenderer::GetInstance()->GetDialog();
	bool show = (bool)lua_toboolean(L, argi++);
	string txt = luaL_optstring(L, argi, "");
	if (txt != "") {
		argi++;
	}
	dlg->SetText(txt.c_str());
	dlg->ClearOptions();
	int n = (int)luaL_len(L, argi);
	for (int i = 1; i <= n; i++) {
		lua_geti(L, argi, i);
			lua_getfield(L, -1, "txt");
			const char* txt = lua_tostring(L, -1);
			lua_pop(L, 1);
			lua_getfield(L, -1, "func");
			int funcRef = luaL_ref(L, LUA_REGISTRYINDEX);
		lua_pop(L, 1);
		dlg->AddOption(txt, funcRef);
	}
	dlg->UpdateOptionsLayout();
	dlg->Visible = show;
	return 0;
}

void npc_dialog_set_xy(int x, int y) {
	auto* dlg = UIRenderer::GetInstance()->GetDialog();
	dlg->X = (float)x;
	dlg->Y = (float)y;
}

bool npc_dialog_is_show(){
	auto* dlg = UIRenderer::GetInstance()->GetDialog();
	return dlg->Visible;
}

void ui_renderer_update() {
	UIRenderer::GetInstance()->Update();
}

void ui_renderer_draw() {
	UIRenderer::GetInstance()->Draw();

}

void luaopen_ui_renderer(lua_State* L)
{
	script_system_register_luac_function(L, ne_imageview_create);
	script_system_register_luac_function(L, ui_textview_create);
	script_system_register_luac_function(L, ui_textview_destroy);
	script_system_register_luac_function(L, ui_renderer_add_to_draw);
	script_system_register_function(L, ui_renderer_clear);
	script_system_register_luac_function(L, npc_dialog_show);
	script_system_register_function(L, npc_dialog_is_show);
	script_system_register_function(L, npc_dialog_set_xy);

	script_system_register_function(L, ui_renderer_update);
	script_system_register_function(L, ui_renderer_draw);

};
