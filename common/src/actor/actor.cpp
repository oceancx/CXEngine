#include "actor.h"
#include "cxlua.h"
#include "utils.h"
#include "cxmath.h"
#include "actor_manager.h"
#include "action.h"
#ifndef SIMPLE_SERVER
#include "actor/move.h"
#include "scene/game_map.h"
#include "window.h"
#include "scene/scene_manager.h"
#include "input_manager.h"
#include "animation/sprite.h"
#include "text_renderer.h"
#include "resource_manager.h"
#include "graphics/ui_renderer.h"
#include "nanovg.h"
#endif
#include <script_system.h>
#include "actor_enum.h"

#define ACTOR_METATABLE_NAME "mt_actor"
Actor::Actor(uint64_t pid)
	:BaseGameEntity(pid),
	m_Target(nullptr)
{
	lua_State* L = script_system_get_luastate();
	lua_getglobal(L, "actor_on_reg_props");
	lua_push_actor(L, this);
	int res = lua_pcall(L, 1, 0, 0);
	check_lua_error(L, res);
	
	SetProperty(PROP_ID, pid);

	m_PatMatrix.clear();

#ifndef SIMPLE_SERVER
	m_MoveHandle = new MoveHandle(this);
	m_SayWidget = new TextView();
	m_SayWidget->PaddingHorizontal = 4;
	m_SayWidget->PaddingVertical = 2;
	m_SayWidget->ShowEmotion = true;
	 m_SayWidget->BackgroundResID = RESOURCE_MANAGER_INSTANCE->EncodeWAS(WZIFEWDF, 0xEF073E43 /* 0xA4120EA9*/);

	m_SayDuration = 0;
	m_ASM = new ActionStateMachine(this);  
	INPUT_MANAGER_INSTANCE->RegisterView(this);
	/*PathMoveAction* action = new PathMoveAction(this);
	m_ASM->ChangeAction(action);*/
	//m_ASM->PushAction(ACTION_IDLE);

	m_NameTV = new UITextView();
	m_NameTV->Font = "SIMSUN";
 	m_NameTV->Size = 16.f;
	m_NameTV->Align = NVG_ALIGN_CENTER;
	m_NameTV->Color = nvgRGBA(118, 253, 140, 255);
	UIRenderer::GetInstance()->AddToDraw(m_NameTV);

	m_SayTV = new UITextView();
	m_SayTV->Font = "SIMSUN";
	m_SayTV->Size = 14.f;
	m_SayTV->Align = NVG_ALIGN_BOTTOM | NVG_ALIGN_CENTER;
	m_SayTV->Color = nvgRGBA(255, 255, 255, 255); // text color
	m_SayTV->BGColor = nvgRGBA(30, 30, 30, 128);
	m_SayTV->WrapWidth = 100;
	UIRenderer::GetInstance()->AddToDraw(m_SayTV);

	GetASM()->Reset();
#endif
}

Actor::~Actor()
{
	m_PatMatrix.clear();
	
#ifndef SIMPLE_SERVER
	SafeDelete(m_MoveHandle);
	INPUT_MANAGER_INSTANCE->UnRegisterView(this);
	SafeDelete(m_ASM);
	SafeDelete(m_SayWidget);
	UIRenderer::GetInstance()->RemoveToDraw(m_NameTV);
	UIRenderer::GetInstance()->RemoveToDraw(m_SayTV);
#endif
}

void Actor::OnUpdate()
{
#ifndef SIMPLE_SERVER
	m_MoveHandle->Update();
	m_ASM->Update();
	if (m_SayDuration > 0)
	{
		if (m_SayWidget)
		{
			m_SayWidget->OnUpdate();
		}
	}
#endif
}

void Actor::OnDraw()
{
#ifndef SIMPLE_SERVER
	if (IsLocal() && !SCENE_MANAGER_INSTANCE->IsDrawStrider())return;

	m_ASM->Draw();
	ActorProp& name = GetProperty(PROP_NAME);
	if (!name.toString().empty())
	{
		m_NameTV->Text = name.toString();
		auto* avatar = m_ASM->GetAvatar();
		if (avatar) {
			m_NameTV->X = avatar->Pos.x;
			m_NameTV->Y = avatar->Pos.y + 36;
			m_NameTV->Draw();
		}
	}

	if (m_SayDuration > 0)
	{
		int past = (int)WINDOW_INSTANCE->GetDeltaTimeMilliseconds();
		m_SayDuration -= past;
		auto* avatar = m_ASM->GetAvatar();
		if (avatar) {
			m_SayTV->X = avatar->Pos.x - avatar->KeyX + avatar->Width / 2;
			m_SayTV->Y = avatar->Pos.y - avatar->KeyY;
			m_SayTV->Text = m_SayText;
			m_SayTV->Draw();
		}
	}
#endif
}

void Actor::SetDir(int dir)
{
	if(IsCombat()){
		m_Props[PROP_COMBAT_DIR] = dir;
	}else{
		m_Props[PROP_DIR] = dir;
	}
	
#ifndef SIMPLE_SERVER
	auto* avatar = m_ASM->GetAvatar();
	if(avatar){
		if (avatar->GroupCount == 4) {
			if (IsCombat()) {
				m_Props[PROP_COMBAT_DIR] = GMath::Dir8toDir4(dir);
			}
			else {
				m_Props[PROP_DIR] = GMath::Dir8toDir4(dir);
			}
		}
	}
#endif
}

int Actor::GetDir()
{
	int dir = 0;
	if(IsCombat()){
		dir = m_Props[PROP_COMBAT_DIR].toInt();
	}else{
		dir = m_Props[PROP_DIR].toInt();
	}
	
#ifndef SIMPLE_SERVER
	auto* avatar = m_ASM->GetAvatar();
	if (avatar) {
		if (avatar->GroupCount == 4) {
			dir = GMath::Dir8toDir4(dir);
		}
	}
#endif
	return dir;
}

void Actor::ReverseDir()
{
	int dir = 0;
	if (IsCombat()) {
		dir = m_Props[PROP_COMBAT_DIR].toInt();
	}
	else {
		dir = m_Props[PROP_DIR].toInt();
	}
	SetDir(GMath::GetReverseDir(dir));
}

void Actor::SetActionID(int action)
{
	if (m_Props[PROP_ACTOR_TYPE].toInt() == ACTOR_TYPE_PLAYER) {
		m_Props[PROP_ACTION_ID] = action;
	}
	else {
		if (action_is_emotion_action(action))return;
		m_Props[PROP_ACTION_ID] = action;
	}
}

void Actor::SetPos(float x, float y)
{
	if (IsCombat()) {
		m_Props[PROP_COMBAT_POS].v2[0] = x;
		m_Props[PROP_COMBAT_POS].v2[1] = y;
	}
	else {
		m_Props[PROP_POS].v2[0] = x;
		m_Props[PROP_POS].v2[1] = y;
	}
}

void Actor::SetPos(Pos p)
{
	SetPos(p.x, p.y);
}

Pos Actor::GetPos()
{
	ActorProp& prop = m_Props[IsCombat() ? PROP_COMBAT_POS : PROP_POS];
	return  Pos(prop.v2[0], prop.v2[1]);
}

void Actor::SetMoveToPos(Pos dest)
{
	m_Props[PROP_MOVE_TO_POS].v2[0] = dest.x;
	m_Props[PROP_MOVE_TO_POS].v2[1] = dest.y;
}

Pos Actor::GetMoveToPos()
{
	return { m_Props[PROP_MOVE_TO_POS].v2[0] ,m_Props[PROP_MOVE_TO_POS].v2[1] };
}

float Actor::GetWidth()
{
#ifndef SIMPLE_SERVER
	auto* avatar = m_ASM->GetAvatar();
	if (!avatar)return 0;
	return (float)avatar->Width;
#else
	return 0;
#endif
}

float Actor::GetHeight()
{
#ifndef SIMPLE_SERVER
	auto* avatar = m_ASM->GetAvatar();
	if (!avatar)return 0;
	return (float)avatar->Height;
#else
	return 0;
#endif
}

int Actor::GetDirByDegree(float degree)
{
#ifndef SIMPLE_SERVER
	int dircnt = m_ASM->GetDirCount();
	if (dircnt == 8) {
		return GMath::Astar_GetDir(degree);
	}
	else {
		return GMath::Astar_GetDir4(degree);
	}
#else
	return GMath::Astar_GetDir(degree);
#endif
}

Pos Actor::GetAttackVec()
{
	Actor* target = this->GetTarget();
	if (target == nullptr)return { 0,0 };
	Pos tmp = target->GetPos() - this->GetPos();
	glm::vec2  v(tmp.x, tmp.y);
	v = glm::normalize(v);
	Pos vec(v.x, v.y);
	return vec;
}

void Actor::SetLocal(bool local)
{
	actor_manager_set_local_player(this);
}

bool Actor::IsLocal()
{
	return actor_manager_is_local_player(this);
}


float Actor::GetMoveDestDistSquare(Pos dest)
{
	Pos& pos = GetPos();
	return ::GMath::Astar_GetDistanceSquare(pos.x,pos.y, dest.x, dest.y);
}

float Actor::GetMoveDestAngle(Pos dest)
{
	Pos& pos = GetPos();
	return ::GMath::Astar_GetAngle(pos.x,pos.y, dest.x, dest.y);
}

const char* Actor::GetLogName()
{
	static CXString logName;
	logName = utils::Utf8ToGB2312(GetProperty(PROP_NAME).toString().c_str());
	return logName.c_str();
}

CXString Actor::GetWeaponAvatarID()
{
	if (m_Props[PROP_ACTOR_TYPE].toInt() == ACTOR_TYPE_NPC)
		return "";
	return m_Props[PROP_WEAPON_AVATAR_ID].toString();
}
#ifndef SIMPLE_SERVER
BaseScene* Actor::GetScene()
{
	return scene_manager_fetch_scene(m_Props[PROP_SCENE_ID].toInt());
}

Bound Actor::GetViewBounds()
{
	auto* avatar = m_ASM->GetAvatar();
	if (avatar == nullptr) return { 0,0,0,0 };
	Pos pos = avatar->Pos;
	pos.x -= avatar->GetFrameKeyX();
	pos.y -= avatar->GetFrameKeyY();
	return Bound{ pos.x, (pos.x + avatar->GetFrameWidth()),
		pos.y,(pos.y + avatar->GetFrameHeight()) };
}

bool Actor::CheckDrag(int dx, int dy)
{
	return pow(dx, 2) + pow(dy, 2) >= 16;
}

void Actor::OnDragMove(int dx, int dy)
{
	TranslateX((float)dx);
	TranslateY((float)dy);
}

void Actor::OnHover(float x, float y)
{
	lua_State* L = script_system_get_luastate();
	lua_getglobal(L, "actor_fire_event");
	lua_push_actor(L, this);
	lua_pushinteger(L, ACTOR_EV_ON_HOVER);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	int res = lua_pcall(L, 4, 1, 0);
	check_lua_error(L, res);
}

bool Actor::OnClick(int button, int x, int y)
{
	lua_State* L = script_system_get_luastate();
	lua_getglobal(L, "actor_ev_on_click");
	lua_push_actor(L, this);
	lua_pushinteger(L, button);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	int res = lua_pcall(L, 4, 1, 0);
	check_lua_error(L, res);
	bool clicked = (bool)lua_toboolean(L, 1);
	return clicked;
}

void Actor::Say(std::string Text)
{
	m_SayText = Text;
	std::wstring wText = utils::StringToWstring(Text);
	m_SayDuration = 1000 * 10;// *24;
	m_SayWidget->Text = wText;
	m_SayWidget->TextCache = std::vector<uint32_t>(wText.begin(), wText.end());
}
#endif

Actor* lua_check_actor(lua_State*L, int index)
{
	Actor** ptr = (Actor**)lua_touserdata(L, index);
	return *ptr;
}

int actor_destroy(lua_State * L)
{
	Actor** ptr = (Actor**)lua_touserdata(L, 1);
	Actor* actor = *ptr;
	if (actor) {
		SafeDelete(actor);
	}
	return 0;
}
int actor_update(lua_State * L) {
	Actor* actor = lua_check_actor(L, 1);
	actor->OnUpdate();
	return 0;
}

int actor_draw(lua_State * L) {
	Actor* actor = lua_check_actor(L, 1);
	actor->OnDraw();
	return 0;
}

int actor_set_dir(lua_State* L)
{
	Actor* actor = lua_check_actor(L, 1);
	lua_Number dir = lua_tonumber(L, 2);
	actor->SetDir((int)dir);
	return 0;
}
int actor_get_dir(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushnumber(L, actor->GetDir());
	return 1;
}

int actor_reverse_dir(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	actor->ReverseDir();
	return 0;
}

int actor_set_local(lua_State* L)
{
	Actor* actor = lua_check_actor(L, 1);
	bool  local = (bool)lua_toboolean(L, 2);
	actor->SetLocal(local);
	return 0;
}

int actor_is_local(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushboolean(L, actor_manager_is_local_player(actor));
	return 1;
}


int actor_get_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushinteger(L, actor->GetID());
	return 1;
}

int actor_set_action_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	int actionID = (int)lua_tointeger(L, 2);
	actor->SetActionID(actionID);
#ifndef SIMPLE_SERVER
	Action* action = new Action(actor);
	actor->GetASM()->ChangeAction(action);
#endif
	return 0;
}

int actor_get_action_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushinteger(L, actor->GetProperty(PROP_ACTION_ID).i);
	return 1;
}

int actor_translate_x(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	float x = (float)luaL_optnumber(L, 2, 0.f);
	actor->TranslateX(x);
	return 0;
}

int actor_translate_y(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	float y = (float)luaL_optnumber(L, 2, 0.f);
	actor->TranslateY(y);
	return 0;
}


int actor_move_to(lua_State* L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	actor->GetMoveHandle()->MoveTo(x, y);
#endif
	return 0;

}

int actor_is_moving(lua_State* L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	lua_pushboolean(L, actor->GetMoveHandle()->IsMove());
	return 1;
#endif
	return 0;
}


int actor_say(lua_State* L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	const char* msg = lua_tostring(L, 2);
	actor->Say(msg);
#endif
	return 0;
}

int actor_show_dialog(lua_State*L){
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	bool show = (bool)lua_toboolean(L, 2);
	const char* msg = luaL_optstring(L, 3,"");
	auto* dlg = UIRenderer::GetInstance()->GetDialog();
	dlg->SetText(msg);
	dlg->Visible = show;
#endif
	return 0;
}


int actor_get_width(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushnumber(L, actor->GetWidth());
	return 1;
}

int actor_get_height(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushnumber(L, actor->GetHeight());
	return 1;
}

int actor_clear_frames(lua_State* L) {
	return 0;
}

int actor_play_cast(lua_State*L) {
#ifndef SIMPLE_SERVER 
	Actor* actor = lua_check_actor(L, 1);
	CastAction* action = new CastAction(actor);
	actor->GetASM()->ChangeAction(action);
#endif
	return 0;
}

int actor_set_time_interval(lua_State*L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	float ti = (float)lua_tonumber(L, 2);
	actor->GetASM()->SetTimeInterval(ti);
#endif
	return 0;
}

int actor_get_avatar(lua_State*L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	int action = (int)luaL_optinteger(L, 2, -1);
	auto* avatar = actor->GetASM()->GetAvatar(action);
	if (avatar == nullptr)return 0;
	lua_push_animation(L, avatar);
	return 1;
#else
	return 0;
#endif
}

int actor_get_weapon(lua_State*L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	auto* weapon = actor->GetASM()->GetWeapon();
	if (weapon == nullptr)return 0;
	lua_push_animation(L, weapon);
	return 1;
#else
	return 0;
#endif
}

int actor_change_pal_matrix(lua_State*L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	int len = (int)luaL_len(L, 2);
	std::vector<NE::PalSchemePart> patMatrix;
	for (int i = 1; i<=len; i++) {
		NE::PalSchemePart seg_matrix;
		lua_geti(L, -1, i);
		
		lua_getfield(L, -1, "from");
		seg_matrix.from = (uint16_t)lua_tointeger(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "to");
		seg_matrix.to = (uint16_t)lua_tointeger(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "mat");
		assert(luaL_len(L, -1) == 9);
		seg_matrix.mat.resize(9);
		seg_matrix.mat.clear();
		for (int mat_i = 1; mat_i <= 9; mat_i++) {
			lua_geti(L, -1, mat_i);
			uint16_t val = (uint16_t)lua_tointeger(L, -1);
			seg_matrix.mat.push_back(val);
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
		
		lua_pop(L, 1);
		patMatrix.push_back(seg_matrix);
	}
	int action = actor->GetASM()->GetActionID();
	actor->GetASM()->Reset();
	actor->SetActionID(action);
	actor->SetPalette(patMatrix);
	actor->GetASM()->Reset();
	return 0;
#else
	return 0;
#endif
}

int actor_get_pal_matrix(lua_State*L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	return 0;
#else
	return 0;
#endif
}

int actor_reset_asm(lua_State*L){
	Actor* actor = lua_check_actor(L, 1);
#ifndef SIMPLE_SERVER
	actor->GetASM()->Reset();
#endif
	return 0;
}

int actor_clear_pal_matrix(lua_State*L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	return 0;
#else
	return 0;
#endif
}

int actor_set_target(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	Actor* target = lua_check_actor(L, 2);
	actor->SetTarget(target);
	return 0;
}

int actor_get_target(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	if (actor->GetTarget() != nullptr) {
		lua_push_actor(L, actor->GetTarget());
		return 1;
	}else{
		return 0;
	}
	
}

int actor_restore_action(lua_State*L)
{
	Actor* actor = lua_check_actor(L, 1);
#ifndef SIMPLE_SERVER
	actor->GetASM()->RestoreAction();
#endif
	return 0;
}
int actor_asm_set_action(lua_State*L){
	Actor* actor = lua_check_actor(L, 1);
	int action_id = (int)lua_tointeger(L, 2);
#ifndef SIMPLE_SERVER
	actor->GetASM()->SetAction(action_id);
#endif
	return 0;
}

int actor_play_be_hit(lua_State*L) {
	Actor* actor = lua_check_actor(L, 1);
#ifndef SIMPLE_SERVER
	BeHitAction* behit = new BeHitAction(actor->GetTarget(), actor);
	behit->MoveVec = actor->GetAttackVec();
	actor->GetTarget()->GetASM()->ChangeAction(behit);
	auto * avatar = actor->GetASM()->GetAvatar();
	if(actor->GetTarget()->GetProperty(PROP_IS_DEAD).toBool()){
		avatar->Pause(500);
	}else{
		avatar->Pause(200);
	}
#endif
	return 0;
}

int actor_move_on_screen_with_duration(lua_State*L) {
	Actor* actor = lua_check_actor(L, 1);
#ifndef SIMPLE_SERVER
	float offx = (float)lua_tonumber(L, 2);
	float offy = (float)lua_tonumber(L, 3);
	float dur  = (float)lua_tonumber(L, 4);
	bool keep_dir = lua_toboolean(L, 5);
	actor->GetMoveHandle()->MoveOnScreenWithDuration({ offx,offy }, dur, keep_dir);
#endif
	return 0;
}

int actor_get_move_dest_angle(lua_State*L) {
	Actor* actor = lua_check_actor(L, 1);
#ifndef SIMPLE_SERVER
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	float angle= actor->GetMoveDestAngle({ x,y });
	lua_pushnumber(L, angle);
#endif
	return 1;
}

int actor_get_dir_by_degree(lua_State*L) {
	Actor* actor = lua_check_actor(L, 1);
#ifndef SIMPLE_SERVER
	float degree = (float)lua_tonumber(L, 2);
	lua_pushnumber(L, actor->GetDirByDegree(degree));
#endif
	return 1;
}

int actor_push_action(lua_State*L) {
	Actor* actor = lua_check_actor(L, 1);
	int action = (int)lua_tointeger(L, 2);
#ifndef SIMPLE_SERVER
	int n = lua_gettop(L);
	int argn = 3;
	ActionInfo info;
	info.actionID = action;
	while (argn <= n) {
		CXString type = lua_tostring(L, argn++);
		if (type == "move") {
			info.move_dur = (float)lua_tonumber(L, argn++);
			info.dx = (float)lua_tonumber(L, argn++);
			info.dy = (float)lua_tonumber(L, argn++);
		}
		else if (type == "loop") {
			info.loop = (bool)lua_toboolean(L, argn++);
			info.loopCount = (int)lua_tointeger(L, argn++);
		}
		else if (type == "update") {
			if (lua_isfunction(L, argn)) {
				lua_pushvalue(L, argn);
				int ref = luaL_ref(L, LUA_REGISTRYINDEX);
				info.updateCB = ref;
			}
			argn++;
		}
		else if (type == "interval") {
			info.interval = (float)lua_tonumber(L, argn++);
		}
	}
	actor->GetASM()->PushAction(info);
#endif
	return 0;
}

int actor_clear_action(lua_State*L) {
	Actor* actor = lua_check_actor(L, 1);
#ifndef SIMPLE_SERVER
	actor->GetASM()->ClearAction();
#endif
	return 0;
}

int actor_move_action_to_back(lua_State*L) {
	Actor* actor = lua_check_actor(L, 1);
#ifndef SIMPLE_SERVER
	actor->GetASM()->MoveActionToBack();
#endif
	return 0;
}

int actor_add_front_anim(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
#ifndef SIMPLE_SERVER
	Animation* anim = lua_check_animation(L, 2);
	actor->GetASM()->AddFrontAnim(anim);
#endif
	return 0;
}
int actor_remove_front_anim(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
#ifndef SIMPLE_SERVER
	Animation* anim = lua_check_animation(L, 2);
	actor->GetASM()->RemoveFrontAnim(anim);
#endif
	return 0;
}

int actor_add_back_anim(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
#ifndef SIMPLE_SERVER
	Animation* anim = lua_check_animation(L, 2);
	actor->GetASM()->AddBackAnim(anim);
#endif
	return 0;
}
int actor_remove_back_anim(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
#ifndef SIMPLE_SERVER
	Animation* anim = lua_check_animation(L, 2);
	actor->GetASM()->RemoveBackAnim(anim);
#endif
	return 0;
}

int actor_clear_front_anim(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
#ifndef SIMPLE_SERVER
	actor->GetASM()->ClearFrontAnim();
#endif
	return 0;
}

int actor_clear_back_anim(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
#ifndef SIMPLE_SERVER
	actor->GetASM()->ClearBackAnim();
#endif
	return 0;
}

int actor_show_beatnumber(lua_State*L){
	Actor* actor = lua_check_actor(L, 1);
	float num = (float)lua_tonumber(L, 2);
#ifndef SIMPLE_SERVER
	auto* beatnumber = actor->GetASM()->GetBeatNumber();
	beatnumber->SetNumber(num);
	auto* map = SCENE_MANAGER_INSTANCE->GetCurrentScene()->GetGameMap();
	int offx = map != nullptr ? map->GetMapOffsetX() : 0;
	int offy = map != nullptr ? map->GetMapOffsetY() : 0;
	if (actor->IsCombat()) {
		offx = offy = 0;
	} 
	auto* avatar = actor->GetASM()->GetAvatar();
	float bny = (float)(actor->GetY() + offy - avatar->GetFrameKeyY() + avatar->GetFrameHeight() / 2);
	beatnumber->SetPos((float)actor->GetX() + offx, bny);
	beatnumber->Beat();
#endif
	return 0;
}

int actor_get_attack_vec(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	Pos vec= actor->GetAttackVec();
	lua_pushnumber(L, vec.x);
	lua_pushnumber(L, vec.y);
	return 2;
}
int actor_is_dirty(lua_State*L){
	Actor* actor = lua_check_actor(L, 1);
	lua_pushboolean(L, actor->IsDirty());
	return 1;
}
int actor_clear_dirty(lua_State*L){
	Actor* actor = lua_check_actor(L, 1);
	actor->ClearDirty();
	return 0;
}

int actor_get_dirty_props(lua_State*L){
	Actor* actor = lua_check_actor(L, 1);
	auto dirty_props = actor->GetDirtyProps();
	lua_newtable(L);
	int i = 1;
	for (auto prop : dirty_props) {
		lua_pushinteger(L, prop);
		lua_seti(L, -2, i++);
	}
	return 1;
}


//{ "__gc",actor_destroy },
luaL_Reg mt_actor[] = {
	{ "Destroy",actor_destroy },
{ "Update",actor_update },
{ "Draw",actor_draw },
{ "GetWidth", actor_get_width },
{ "GetHeight", actor_get_height },
{ "GetID", actor_get_id },
{ "SetDir", actor_set_dir },
{ "GetDir", actor_get_dir },
{ "ReverseDir", actor_reverse_dir },
{ "SetLocal", actor_set_local },
{ "IsLocal", actor_is_local },
{ "SetActionID", actor_set_action_id },
{ "GetActionID", actor_get_action_id },
{ "TranslateX", actor_translate_x },
{ "TranslateY", actor_translate_y },
{ "MoveTo", actor_move_to },
{ "IsMoving", actor_is_moving},
{ "Say", actor_say },
{ "ShowDialog", actor_show_dialog},
{ "ClearFrames", actor_clear_frames },
{ "SetTarget", actor_set_target},
{ "GetTarget", actor_get_target},
{ "PlayCast", actor_play_cast },
{ "SetTimeInterval", actor_set_time_interval },
{ "GetAvatar", actor_get_avatar },
{ "GetWeapon", actor_get_weapon },
{ "ChangePalMatrix", actor_change_pal_matrix},
{ "GetPalMatrix", actor_get_pal_matrix},
{ "ClearPalMatrix", actor_clear_pal_matrix},
{ "RegProperty", actor_reg_prop},
{ "GetProperty", actor_get_prop},
{ "SetProperty", actor_set_prop},
{ "ResetASM", actor_reset_asm},
{"RestoreAction", actor_restore_action},
{"ASMSetAction", actor_asm_set_action},
{"PlayBeHit", actor_play_be_hit},
{"MoveOnScreenWithDuration",actor_move_on_screen_with_duration},
{"GetMoveDestAngle",actor_get_move_dest_angle},
{"GetDirByDegree",actor_get_dir_by_degree},
{"PushAction",actor_push_action},
{"ClearAction",actor_clear_action},
{"MoveActionToBack",actor_move_action_to_back},
{"AddFrontAnim",actor_add_front_anim},
{"RemoveFrontAnim",actor_remove_front_anim},
{"AddBackAnim",actor_add_back_anim},
{"RemoveBackAnim",actor_remove_back_anim},
{ "ClearBackAnim", actor_clear_back_anim},
{ "ClearFrontAnim", actor_clear_front_anim},
{"ShowBeatNumber",actor_show_beatnumber},
{"GetAttackVec",actor_get_attack_vec},
{ "IsDirty",actor_is_dirty},
{ "ClearDirty",actor_clear_dirty},
{ "GetDirtyProps",actor_get_dirty_props},
{ NULL, NULL }
};

void lua_push_actor(lua_State*L, Actor* actor)
{
	Actor** ptr = (Actor**)lua_newuserdata(L, sizeof(Actor*));
	*ptr = actor;
	if (luaL_newmetatable(L, ACTOR_METATABLE_NAME)) {
		luaL_setfuncs(L, mt_actor, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}


int actor_get_metatable(lua_State* L){
	if (luaL_newmetatable(L, ACTOR_METATABLE_NAME)) {
		luaL_setfuncs(L, mt_actor, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	return 1;
}

int action_system_get_action_size() {
	return (int)action_get_size();
}

std::string action_system_get_action_name(int action) {
	return action_get_name(action);
}

int math_get_distance(lua_State* L)
{
	float sx = (float)lua_tonumber(L, 1);
	float sy = (float)lua_tonumber(L, 2); 
	float ex = (float)lua_tonumber(L, 3); 
	float ey = (float)lua_tonumber(L, 4);
	float ret = GMath::Astar_GetDistance(sx,sy,ex,ey);
	lua_pushnumber(L, ret);
	return 1;
}


int math_get_distance_square(lua_State *L){
	float sx = (float)lua_tonumber(L, 1);
	float sy = (float)lua_tonumber(L, 2);
	float ex = (float)lua_tonumber(L, 3);
	float ey = (float)lua_tonumber(L, 4);
	float ret = GMath::Astar_GetDistanceSquare(sx, sy, ex, ey);
	lua_pushnumber(L, ret);
	return 1;
}

int astar_get_angle(lua_State* L) {
	float sx = (float)lua_tonumber(L, 1);
	float sy = (float)lua_tonumber(L, 2);
	float ex = (float)lua_tonumber(L, 3);
	float ey = (float)lua_tonumber(L, 4);
	float ret = GMath::Astar_GetAngle(sx, sy, ex, ey);
	lua_pushnumber(L, ret);
	return 1;
}

int astar_get_angle_use_box_xy(lua_State* L) {
	int sx = (int)lua_tointeger(L, 1);
	int sy = (int)lua_tointeger(L, 2);
	int ex = (int)lua_tointeger(L, 3);
	int ey = (int)lua_tointeger(L, 4);
	int ret = GMath::Astar_GetAngleUseBoxXY(sx, sy, ex, ey);
	lua_pushinteger(L, ret);
	return 1;
}

int astar_get_dir_use_int(int degree) {
	return GMath::Astar_GetDirUseInt(degree);
}

int astar_get_dir(float degree) {
	return GMath::Astar_GetDir(degree);
}

int astar_get_dir4(float degree) {
	return GMath::Astar_GetDir4(degree);
}

int math_get_reverse_dir(int dir) {
	return GMath::GetReverseDir(dir);
}

int math_clamp(int value,int min,int max) {
	return GMath::Clamp(value, min, max);
}

int math_dir_8_to_4(int dir) {
	return GMath::Dir8toDir4(dir);
}

int math_next_dir4(int dir) {
	return GMath::NextDir4(dir);
}

void luaopen_actor(lua_State* L)
{
	script_system_register_luac_function(L, math_get_distance);
	script_system_register_luac_function(L, math_get_distance_square);
	script_system_register_luac_function(L, astar_get_angle);
	script_system_register_luac_function(L, astar_get_angle_use_box_xy);
	script_system_register_function(L, astar_get_dir_use_int);
	script_system_register_function(L, astar_get_dir);
	script_system_register_function(L, astar_get_dir4);
	script_system_register_function(L, math_get_reverse_dir);
	script_system_register_function(L, math_clamp);
	script_system_register_function(L, math_dir_8_to_4);
	script_system_register_function(L, math_next_dir4);

	script_system_register_luac_function(L, actor_get_metatable);

	script_system_register_function(L, action_system_get_action_size);
	script_system_register_function(L, action_system_get_action_name);


#define REG_ENUM(e) (lua_pushinteger(L, e), lua_setglobal(L, #e))
	REG_ENUM(ACTOR_TYPE_DEFAULT);
	REG_ENUM(ACTOR_TYPE_PLAYER);
	REG_ENUM(ACTOR_TYPE_SUMMON);
	REG_ENUM(ACTOR_TYPE_NPC);

	REG_ENUM(ACTION_IDLE);
	REG_ENUM(ACTION_WALK);
	REG_ENUM(ACTION_SIT);
	REG_ENUM(ACTION_ANGRY);
	REG_ENUM(ACTION_SAYHI);
	REG_ENUM(ACTION_DANCE);
	REG_ENUM(ACTION_SALUTE);
	REG_ENUM(ACTION_CLPS);
	REG_ENUM(ACTION_CRY);
	REG_ENUM(ACTION_BATIDLE);
	REG_ENUM(ACTION_ATTACK);
	REG_ENUM(ACTION_CAST);
	REG_ENUM(ACTION_BEHIT);
	REG_ENUM(ACTION_RUNTO);
	REG_ENUM(ACTION_RUNBACK);
	REG_ENUM(ACTION_DEFEND);


	REG_ENUM(ASM_ACTION);
	REG_ENUM(ASM_ATTACK_ACTION);
	REG_ENUM(ASM_CAST_ACTION);
	REG_ENUM(ASM_BEHIT_ACTION);
	REG_ENUM(ASM_BECAST_ACTION);
	REG_ENUM(ASM_DEAD_FLY_ACTION);
	REG_ENUM(ASM_PATH_MOVE_ACTION);
		
	REG_ENUM(WEAPON_SPEAR);
	REG_ENUM(WEAPON_AXE);
	REG_ENUM(WEAPON_SWORD);
	REG_ENUM(WEAPON_DBSWORDS);
	REG_ENUM(WEAPON_RIBBON);
	REG_ENUM(WEAPON_CLAW);
	REG_ENUM(WEAPON_FAN);
	REG_ENUM(WEAPON_WAND);
	REG_ENUM(WEAPON_HAMMER);
	REG_ENUM(WEAPON_WHIP);
	REG_ENUM(WEAPON_RING);
	REG_ENUM(WEAPON_KNIFE);
	REG_ENUM(WEAPON_STAFF);
	REG_ENUM(WEAPON_JEWEL);
	REG_ENUM(WEAPON_BOW);


	REG_ENUM(ROLE_FYN);
	REG_ENUM(ROLE_YNX);
	REG_ENUM(ROLE_XYS);
	REG_ENUM(ROLE_JXK);
	REG_ENUM(ROLE_HMR);
	REG_ENUM(ROLE_GJL);
	REG_ENUM(ROLE_JMW);
	REG_ENUM(ROLE_HTG);
	REG_ENUM(ROLE_WTJ);
	REG_ENUM(ROLE_XCE);
	REG_ENUM(ROLE_STB);
	REG_ENUM(ROLE_LTZ);

	REG_ENUM(DIR_N);
	REG_ENUM(DIR_S);
	REG_ENUM(DIR_W);
	REG_ENUM(DIR_E);
	REG_ENUM(DIR_NE);
	REG_ENUM(DIR_NW);
	REG_ENUM(DIR_SE);
	REG_ENUM(DIR_SW);


	REG_ENUM(ACTOR_EV_ON_CLICK);
	REG_ENUM(ACTOR_EV_ON_HOVER);
#undef REG_ENUM
	luaopen_actor_enum(L);
}
