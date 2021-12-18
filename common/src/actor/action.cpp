#include "action.h"
#ifndef SIMPLE_SERVER
#include "utils.h"
#include "game.h"
#include "actor/move.h"
#include "actor/actor.h"
#include "resource_manager.h"
#include "animation/sprite.h"
#include "texture_manager.h"
#include "scene/scene_manager.h"
#include "window.h"
#include "scene/base_scene.h"
#include "scene/game_map.h"
#endif
#include "cxmath.h"
#include "time/timer_manager.h"
#include "actor_manager.h"
#include <script_system.h>
#include "logger.h"

#define  PERFRAME_TIME (0.016f*2.5f)

static std::vector<std::string> s_ActionSet = { u8"idle",u8"walk",u8"sit",u8"angry",u8"sayhi",u8"dance",u8"salute",u8"clps",u8"cry",u8"batidle",u8"attack",u8"cast",u8"behit",u8"runto",u8"runback",u8"defend",u8"unknown" };;
std::string action_get_name(int i)
{
	return s_ActionSet[i];
}

size_t action_get_size()
{
	return s_ActionSet.size();
}

std::string action_system_get_action(int i)
{
	return s_ActionSet[i];
}

bool action_is_show_weapon(int action)
{
	if (action == ACTION_SIT
		|| action == ACTION_ANGRY
		|| action == ACTION_SAYHI
		|| action == ACTION_DANCE
		|| action == ACTION_SALUTE
		|| action == ACTION_CRY)
	{
		return false;
	}
	return true;
}

bool action_is_emotion_action(int action)
{
	if (action == ACTION_SIT
		|| action == ACTION_ANGRY
		|| action == ACTION_SAYHI
		|| action == ACTION_DANCE
		|| action == ACTION_SALUTE
		|| action == ACTION_CRY)
	{
		return true;
	}
	return false;
}

bool action_is_battle_action(int action)
{
	if (action == ACTION_CLPS
		|| action == ACTION_BATIDLE
		|| action == ACTION_ATTACK
		|| action == ACTION_CAST
		|| action == ACTION_BEHIT
		|| action == ACTION_RUNTO
		|| action == ACTION_RUNBACK
		|| action == ACTION_DEFEND)
	{
		return true;
	}
	return false;
}

void call_combat_system_on_acting_end(Actor* actor)
{
	lua_State* L = script_system_get_luastate();
	lua_getglobal(L, "combat_system_on_acting_end");
	lua_push_actor(L, actor);
	int res = lua_pcall(L, 1, 0, 0);
	check_lua_error(L, res);
}



#ifndef SIMPLE_SERVER

Animation* CreateBuffAnimation(uint64_t resid) {
	if (resid == 0)return nullptr;
	Animation* anim = new Animation(resid);
	anim->SetLoop(0);
	return anim;
}


Action::Action(Actor* _actor) :
	m_Actor(_actor),
	m_Type(ASM_ACTION)
{
	m_pASM = m_Actor->GetASM();
}

void Action::Update()
{
	int action = m_Actor->GetProperty(PROP_ACTION_ID).toInt();
	if (action != m_pASM->GetActionID()) {
		m_pASM->SetAction(action);
	}

	auto* avatar = m_pASM->GetAvatar();
	if (!avatar)return;
	avatar->Update();
} 


void Action::OnEnter()
{
	Enter();
}

void Action::OnExit()
{
	Exit();
}

void Action::Enter()
{
	int action = m_Actor->GetProperty(PROP_ACTION_ID).toInt();
	if (action != m_pASM->GetActionID()) {
		m_pASM->SetAction(action);
	}

	auto* avatar = m_pASM->GetAvatar();
	if (!avatar)return;
	avatar->Reset();
}




void CastAction::Update()
{
	auto* avatar = m_pASM->GetAvatar();
	if (!avatar)return;
	avatar->Update();

	int action = m_pASM->GetActionID();
	if (action == ACTION_BATIDLE) {
		if (avatar->IsGroupEndUpdate()) {
			m_pASM->SetAction(ACTION_CAST);
		}
	}
	else if (action == ACTION_CAST) {
		if (avatar->IsGroupEndUpdate()) {
			m_Actor->SetActionID(ACTION_BATIDLE);
			auto* new_action = new Action(m_Actor);
			m_pASM->ChangeAction(new_action);
			/*m_pASM->AddDelayCallback(1000, [this]() {
				m_Actor->SetProperty(PROP_COMBAT_ACTING, false);
				call_combat_system_on_acting_end(m_Actor);
			});*/
		}
		else if (avatar->IsFrameUpdate()) {
			if (m_Actor->GetProperty(PROP_ASM_PLAY_BEHIT).toBool()) {
				if (avatar->CurrentFrame == avatar->GroupFrameCount / 2) {
					BeCastAction* action = new BeCastAction(m_Target, m_Actor);
					action->MoveVec = m_Actor->GetAttackVec();
					m_Target->GetASM()->ChangeAction(action);
				}
			}
			else {
				if (avatar->CurrentFrame == avatar->GroupFrameCount /2) {
					uint64_t id = m_Actor->GetProperty(PROP_CAST_ID).toUInt64();
					Animation* anim = new Animation(id);
					anim->SetLoop(1);
					auto* targetAvatar = m_Target->GetASM()->GetAvatar();
					anim->Pos.x = targetAvatar->Pos.x;
					anim->Pos.y = targetAvatar->Pos.y - targetAvatar->GetFrameKeyY() + targetAvatar->GetFrameHeight() / 2;
					anim->AddFrameCallback(anim->GroupFrameCount/6*5, [this, anim]() {
						uint64_t resid = m_Actor->GetProperty(PROP_ASM_BUFF_ANIM).toUInt64();
						auto* anim = CreateBuffAnimation(resid);
						m_Target->GetASM()->AddFrontAnim(anim);
						});
					AnimationManager::GetInstance()->AddAnimation(anim);
				}
			}
		}
	}
}
void CastAction::Exit()
{

}

void CastAction::Enter()
{
	m_Target = m_Actor->GetTarget();
	if(m_Target){
		float degree = m_Actor->GetMoveDestAngle(m_Target->GetPos());
		int dir = m_Actor->GetDirByDegree(degree);
		dir = GMath::Dir8toDir4(dir);
		m_Actor->SetDir(dir);
		m_Target->SetDir(dir);
		m_Target->ReverseDir();
		m_Target->GetASM()->SetAction(ACTION_BATIDLE);
	}
	
	m_pASM->SetAction(ACTION_BATIDLE);
	m_Actor->SetProperty(PROP_COMBAT_ACTING, true);
}

void BeHitAction::Update()
{
	auto* avatar = m_pASM->GetAvatar();
	if (!avatar)return;
	avatar->Update();
	
	int action = m_pASM->GetActionID();
	if (action == ACTION_BEHIT) {
		if (avatar->IsGroupEndUpdate()) {
			if (m_Actor->GetProperty(PROP_IS_DEAD).toBool()) {
				DeadFlyAction* action = new DeadFlyAction(m_Actor, m_Attacker, MoveVec);
				m_pASM->ChangeAction(action);
			}
			else {
				m_Actor->SetActionID(ACTION_BATIDLE);
				m_pASM->RestoreAction();
			}
		}
		else if (avatar->IsFrameUpdate()) {
			if (avatar->CurrentFrame == 1) {
				Animation* anim = new Animation(m_Actor->GetProperty(PROP_ASM_BEHIT_ANIM).toUInt64());
				anim->SetLoop(1);
				anim->Offset.y = -avatar->GetFrameKeyY() + avatar->GetFrameHeight() / 2.0f;
				anim->AddFrameCallback(1, [this, avatar]() {
					Pos pos = m_Actor->GetPos();
					pos.x = pos.x + MoveVec.x * 3;
					pos.y = pos.y + MoveVec.y * 3;
					m_Actor->SetPos(pos);
				});
				anim->AddFrameCallback(anim->GroupFrameCount / 2, [this, avatar]() {
					m_Actor->GetMoveHandle()->MoveOnScreenWithDuration({ MoveVec.x * 7 ,MoveVec.y * 7 }, PERFRAME_TIME * 4, true);
				});
				m_Actor->GetASM()->AddFrontAnim(anim);
				int tm = (int)(anim->GroupFrameCount*anim->FrameInterval * 1000);
				avatar->Pause(tm);
			}
		}
	}
}

void BeHitAction::Exit()
{
	m_Actor->SetPos(m_SavedPos);
	m_Actor->GetMoveHandle()->MoveOnScreenWithDuration({ -MoveVec.x * 10,-MoveVec.y * 10 }, PERFRAME_TIME / 2, true);
}

void BeHitAction::Enter()
{
	m_pASM->SetAction(ACTION_BEHIT);
	m_SavedPos = m_Actor->GetPos();
	auto*targetAvatar = m_pASM->GetAvatar();
	BeatNumber* beatNumber = new BeatNumber();
	auto* map = SCENE_MANAGER_INSTANCE->GetCurrentScene()->GetGameMap();
	int offx = map != nullptr ? map->GetMapOffsetX() : 0 ;
	int offy = map != nullptr ? map->GetMapOffsetY() : 0;
	if(m_Actor->IsCombat()){
		offx = offy = 0;
	}
	float bny = (float)(m_Actor->GetY() + offy - targetAvatar->GetFrameKeyY() + targetAvatar->GetFrameHeight() / 2);
	beatNumber->SetPos((float)m_Actor->GetX() + offx, bny);

	beatNumber->SetNumber(m_Actor->GetProperty(PROP_ASM_DAMAGE).toFloat());
	beatNumber->Beat();
	AnimationManager::GetInstance()->AddBeatNumber(beatNumber);
}

void BeHitAction::Draw()
{
	
}

void BeCastAction::Update()
{
	auto* avatar = m_pASM->GetAvatar();
	if (!avatar)return;
	avatar->Update();
	int action = m_pASM->GetActionID();
	if (action == ACTION_BEHIT) {
		if (avatar->IsGroupEndUpdate()) {
			if(m_Actor->GetProperty(PROP_IS_DEAD).toBool()){
				DeadFlyAction* action = new DeadFlyAction(m_Actor, m_Attacker, MoveVec);
				m_pASM->ChangeAction(action);
				m_pASM->GetAvatar()->Pause(100);
			}else{
				Pos pos = m_Actor->GetPos();
				pos.x = pos.x - MoveVec.x * 5;
				pos.y = pos.y - MoveVec.y * 5;
				m_Actor->SetPos(pos);
				m_pASM->SetAction(ACTION_BATIDLE);	
			}
		}
		else if (avatar->IsFrameUpdate()) {
			if (avatar->CurrentFrame == 1) {
				uint64_t id = m_Attacker->GetProperty(PROP_CAST_ID).toUInt64();
				Animation* anim = new Animation(id);
				anim->SetLoop(1);
				anim->Pos.x = avatar->Pos.x;
				anim->Pos.y = avatar->Pos.y - avatar->GetFrameKeyY() + avatar->GetFrameHeight() / 2;
				anim->AddFrameCallback(anim->GroupFrameCount * 5 / 6, [this, anim]() {
					Pos pos = m_Actor->GetPos();
					pos.x = pos.x + MoveVec.x * 5;
					pos.y = pos.y + MoveVec.y * 5;
					m_Actor->SetPos(pos);
				});

				AnimationManager::GetInstance()->AddAnimation(anim);
				int tm = (int)(anim->GroupFrameCount*anim->FrameInterval * 1000);
				avatar->Pause(tm);
			}
		}
	}
}

void BeCastAction::Exit()
{

}

void BeCastAction::Enter()
{
	m_pASM->SetAction(ACTION_BEHIT);

	auto*targetAvatar = m_pASM->GetAvatar();
	BeatNumber* beatNumber = new BeatNumber();
	auto* map = SCENE_MANAGER_INSTANCE->GetCurrentScene()->GetGameMap();
	int offx = map != nullptr ? map->GetMapOffsetX() : 0;
	int offy = map != nullptr ? map->GetMapOffsetY() : 0;
	if (m_Actor->IsCombat()) {
		offx = offy = 0;
	}
	float bny = (float)(m_Actor->GetY() + offy - targetAvatar->GetFrameKeyY() + targetAvatar->GetFrameHeight() / 2);
	beatNumber->SetPos((float)m_Actor->GetX() + offx, bny);
	
	beatNumber->SetNumber(m_Actor->GetProperty(PROP_ASM_DAMAGE).toFloat());
	beatNumber->Beat();
	AnimationManager::GetInstance()->AddBeatNumber(beatNumber);
}

void PathMoveAction::Update()
{
	auto* avatar = m_pASM->GetAvatar();
	if (!avatar)return;
	avatar->Update();
	int action = m_pASM->GetActionID();
	if (action == ACTION_IDLE) {
		if (avatar->IsFrameUpdate()) {
			if (m_Actor->GetMoveHandle()->IsMove()) {
				m_pASM->SetAction(ACTION_WALK);
			}
		}
	}
	else if (action == ACTION_WALK) {
		if (avatar->IsFrameUpdate()) {
			if (!m_Actor->GetMoveHandle()->IsMove()) {
				m_pASM->SetAction(ACTION_IDLE);
			}
		}
	}
}

void PathMoveAction::Exit()
{

}

void PathMoveAction::Enter()
{
	if (m_Actor->GetProperty(PROP_ACTION_ID).toInt() != ACTION_IDLE)
		m_Actor->SetActionID(ACTION_IDLE);
	m_pASM->SetAction(ACTION_IDLE);
}

void DeadFlyAction::Update()
{
	auto* avatar = m_pASM->GetAvatar();
	if (!avatar)return;
	avatar->Update();

	int dir = m_Actor->GetDir();
	Pos pos = m_Actor->GetPos();

	if (pos.y - avatar->GetFrameKeyY() <= 0) {
		m_Dir.y = -m_Dir.y;
	}

	if (pos.y - avatar->GetFrameKeyY() + avatar->GetFrameHeight() >= 600) {
		m_Dir.y = -m_Dir.y;
	}

	if (avatar->IsFrameUpdate()) {
		pos.x += m_Dir.x * 49;
		pos.y += m_Dir.y * 49;
		m_Actor->SetPos(pos);
	}
	
	if (m_Dir.x < 0) {
		if (pos.x - avatar->GetFrameKeyX() < 0 ) {
			Action* action = new Action(m_Actor);
			m_Actor->GetASM()->ChangeAction(action);
			m_Actor->SetPos(m_SavedPos);
			m_Actor->SetDir(m_SavedDir);
			m_Actor->SetActionID(ACTION_BATIDLE);
			return;
		}
	}
	else if (m_Dir.x >=0) {
		if (pos.x - avatar->GetFrameKeyX() + avatar->GetFrameWidth() >= 800) {
			Action* action = new Action(m_Actor);
			m_Actor->GetASM()->ChangeAction(action);
			m_Actor->SetPos(m_SavedPos);
			m_Actor->SetDir(m_SavedDir);
			m_Actor->SetActionID(ACTION_BATIDLE);
			return;
		}
	}

	if(avatar->IsGroupEndUpdate()){
		dir = GMath::NextDir4(dir);
		m_Actor->SetDir(dir);
	}
}

void DeadFlyAction::Enter()
{
	m_pASM->SetAction(ACTION_BEHIT);
	m_SavedPos = m_Actor->GetPos();
	auto* avatar = m_pASM->GetAvatar();
	m_SavedDir = avatar->GetDir();
	avatar->SetFrameInterval(0.016f * 4);
	avatar->SetLoop(0);
}


void DeadFlyAction::Exit()
{
	m_Actor->SetPos(m_SavedPos);
}

ActionStateMachine::ActionStateMachine(Actor* _actor)
	:m_Actor(_actor)
{
	m_TimeInterval = 0.016f * 5;
	m_pCurrentAction = nullptr;
	m_pPreviousAction = nullptr;
	for (int action = ACTION_IDLE; action < ACTION_COUNT; action++) {
		m_AvatarActions.insert({ action,nullptr });
		m_WeaponActions.insert({ action,nullptr });
	}
	m_PlayerShadow = new Animation(SHAPEWDF, 0xDCE4B562);
	m_WeaponID = m_Actor->GetProperty(PROP_WEAPON_AVATAR_ID).toString();
	m_AvatarID = m_Actor->GetProperty(PROP_AVATAR_ID).toString();
	m_ActionID = ACTION_IDLE;
	m_bMoveActionToBack = false;
	ActionInfo info;
	info.actionID = ACTION_IDLE;
	m_ActionQueue.push_back(info);
	m_BeatNumber = new BeatNumber();
}

ActionStateMachine::~ActionStateMachine()
{
	for (auto& it : m_AvatarActions) {
		SafeDelete(it.second);
	}
	m_AvatarActions.clear();

	for (auto& it : m_WeaponActions) {
		SafeDelete(it.second);
	}
	m_WeaponActions.clear();

	SafeDelete(m_PlayerShadow);
	for (auto& it : m_FrontAnimQueue){
		SafeDelete(it);
	}
	m_FrontAnimQueue.clear();

	for (auto& it : m_BackAnimQueue) {
		SafeDelete(it);
	}
	m_BackAnimQueue.clear();

	m_ActionQueue.clear();
	SafeDelete(m_BeatNumber);
}

void ActionStateMachine::Update()
{
	if (m_AvatarID != m_Actor->GetProperty(PROP_AVATAR_ID).toString())
	{
		SetAvatar(m_Actor->GetProperty(PROP_AVATAR_ID).toString());
	}
	if (m_HasWeapon != (m_Actor->GetWeaponAvatarID() != "")) {
		m_HasWeapon = (m_Actor->GetWeaponAvatarID() != "");
	}
	if (m_HasWeapon) {
		if (m_WeaponID != m_Actor->GetWeaponAvatarID()) {
			SetWeapon(m_Actor->GetWeaponAvatarID());
		}
	}

	if (m_bMoveActionToBack) {
		auto info = m_ActionQueue.front();
		m_ActionQueue.pop_front();
		m_ActionQueue.push_back(info);
		m_bMoveActionToBack = false;
		if (!m_ActionQueue.empty()) {
			auto info = m_ActionQueue.front();
			m_ActionID = info.actionID;
			auto* avatar = GetAvatar(m_ActionID);
			avatar->Replay();
			if (info.interval > 0) {
				avatar->FrameInterval = info.interval;
			}
			if (info.dx != 0 || info.dy != 0) {
				m_Actor->GetMoveHandle()->MoveOnScreenWithDuration(Pos(info.dx, info.dy), info.move_dur, true);
			}
		}
	}

	m_ActionID = -1;
	if (!m_ActionQueue.empty()) {
		auto info = m_ActionQueue.front();
		m_ActionID = info.actionID;
	}
	auto* avatar = GetAvatar(m_ActionID);
	if (!avatar)return;
	avatar->Update();
	if (avatar->GetState() == ANIMATION_STOP) {
		cxlog_info("%s pop action %s\n", 
			m_Actor->GetLogName(),
			action_get_name(m_ActionID).c_str());
		auto info = m_ActionQueue.front();
		int frontID = info.actionID;
		if (m_ActionID == frontID) {
			m_ActionQueue.pop_front();
		}
		if (!m_ActionQueue.empty()) {
			auto info = m_ActionQueue.front();
			m_ActionID = info.actionID;
			avatar = GetAvatar(m_ActionID);
			avatar->Replay();
		}
	}

	if (HasWeapon() && action_is_show_weapon(m_ActionID)) {
		auto* weapon = GetWeapon(m_ActionID);
		weapon->CurrentFrame = avatar->CurrentFrame;
	}

	for (auto& anim : m_FrontAnimQueue) {
		anim->Update();
	}
	for (auto it = m_FrontAnimQueue.begin(); it != m_FrontAnimQueue.end();) {
		if ((*it)->GetState() == ANIMATION_STOP) {
			delete *it;
			it = m_FrontAnimQueue.erase(it);
		}
		else {
			it++;
		}
	}

	for (auto& anim : m_BackAnimQueue) {
		anim->Update();
	}
	for (auto it = m_BackAnimQueue.begin(); it != m_BackAnimQueue.end();) {
		if ((*it)->GetState() == ANIMATION_STOP) {
			delete* it;
			it = m_BackAnimQueue.erase(it);
		}
		else {
			it++;
		}
	}

	if(m_BeatNumber){
		m_BeatNumber->Update();
	}
}

void ActionStateMachine::Draw()
{
	auto* avatar = GetAvatar(m_ActionID);
	if (!avatar)return;

	int screenWidth = WINDOW_INSTANCE->GetWidth();
	int screenHeight = WINDOW_INSTANCE->GetHeight();
	int halfScreenWidth = screenWidth / 2;
	int halfScreenHeight = screenHeight / 2;

	Pos pos = m_Actor->GetPos();
	int dir = m_Actor->GetDir();
	if (m_Actor->IsCombat()) {
		avatar->Pos.x = pos.x;
		avatar->Pos.y = pos.y;
	}else{
		if (m_Actor->IsLocal() && m_Actor->GetScene() != nullptr&&m_Actor->GetScene()->GetGameMap() != nullptr) {
			int mapWidth = m_Actor->GetScene()->GetGameMap()->GetWidth();
			int mapHeight = m_Actor->GetScene()->GetGameMap()->GetHeight();
			int px = m_Actor->GetX();
			int py = m_Actor->GetY();
			int maxMapOffsetX = mapWidth - halfScreenWidth;
			int maxMapOffsetY = mapHeight - halfScreenHeight;

			px = px < halfScreenWidth ? px :
				(px > maxMapOffsetX ?
				(screenWidth - (mapWidth - px)) : halfScreenWidth);
			py = py < halfScreenHeight ? py :
				(py > maxMapOffsetY ?
				(screenHeight - (mapHeight - py)) : halfScreenHeight);

			avatar->Pos.x = (float)px;
			avatar->Pos.y = (float)py;
		}
		else {
			auto* player = actor_manager_fetch_local_player();
			if (player) {
				auto* scene = player->GetScene();
				auto* map = scene->GetGameMap();
				int offx = map->GetMapOffsetX();
				int offy = map->GetMapOffsetY();
				avatar->Pos.x = (float)(m_Actor->GetX() + offx);
				avatar->Pos.y = (float)(m_Actor->GetY() + offy);
			}else{
				avatar->Pos.x = pos.x;
				avatar->Pos.y = pos.y;
			}
		}
	}

	avatar->Dir = dir;

	m_PlayerShadow->Pos.x = avatar->Pos.x;
	m_PlayerShadow->Pos.y = avatar->Pos.y;


	for (auto& anim : m_BackAnimQueue) {
		anim->Pos.x = avatar->Pos.x + anim->Offset.x;
		anim->Pos.y = avatar->Pos.y + anim->Offset.y;
		anim->Draw();
	}

	m_PlayerShadow->Draw();
	avatar->Draw();

	if (HasWeapon() && action_is_show_weapon(m_ActionID)) {
		auto* weapon = GetWeapon(m_ActionID);
		weapon->Pos.x = avatar->Pos.x;
		weapon->Pos.y = avatar->Pos.y;
		weapon->Dir = dir;
		weapon->Draw();
	}

	if (m_pCurrentAction) {
		m_pCurrentAction->Draw();
	}
	
	for (auto& anim : m_FrontAnimQueue) {
		anim->Pos.x = avatar->Pos.x + anim->Offset.x;
		anim->Pos.y = avatar->Pos.y + anim->Offset.y;
		anim->Draw();
	}
	

	if(m_BeatNumber){
		m_BeatNumber->Draw();
	}
}

void ActionStateMachine::SetWeapon(CXString id)
{
	for (auto& it : m_WeaponActions) {
		SafeDelete(it.second);
	}
	m_WeaponActions.clear();
	m_WeaponID = id;
	m_HasWeapon = id == "";
}

void ActionStateMachine::SetAvatar(CXString id)
{
	for (auto& it : m_AvatarActions) {
		SafeDelete(it.second);
	}
	m_AvatarActions.clear();
	m_AvatarID = id;
}

void ActionStateMachine::SetAction(int id)
{
	m_ActionID = id;

	EnsureLoadAction(m_ActionID);

	auto* avatar = GetAvatar(m_ActionID);
	if (!avatar)return;
	avatar->Reset();
	if (!HasWeapon() || !action_is_show_weapon(m_ActionID))return;
	auto* weapon = GetWeapon(m_ActionID);
	weapon->Reset();
}

void ActionStateMachine::RestoreAction()
{
	if (m_pCurrentAction) {
		m_pCurrentAction->OnExit();
		delete m_pCurrentAction;
	}
	m_pCurrentAction = m_pPreviousAction;
	if(m_pCurrentAction){
		m_pCurrentAction->OnEnter();
	}
	m_pPreviousAction = nullptr;
}

void ActionStateMachine::ChangeAction(Action* action)
{
	if (m_pCurrentAction) {
		m_pCurrentAction->OnExit();
	}
	if (m_pPreviousAction) {
		delete m_pPreviousAction;
		m_pPreviousAction = nullptr;
	}
	m_pPreviousAction = m_pCurrentAction;
	m_pCurrentAction = action;
	if (m_pCurrentAction) {
		m_pCurrentAction->OnEnter();
	}
}

void ActionStateMachine::EnsureLoadAction(int action)
{
	if (action < ACTION_IDLE || action >= ACTION_COUNT)return;

	if (m_AvatarActions.find(action) == m_AvatarActions.end()) {
		int avatar_type = 0;
		int actor_type = m_Actor->GetProperty(PROP_ACTOR_TYPE).toInt();
		if (actor_type == ACTOR_TYPE_PLAYER) {
			avatar_type = AVATAR_TYPE_ROLE;
		}
		else if (actor_type == ACTOR_TYPE_SUMMON) {
			avatar_type = AVATAR_TYPE_SUMMON;
		}
		else if (actor_type == ACTOR_TYPE_NPC) {
			avatar_type = AVATAR_TYPE_NPC;
		}
		auto resid = RESOURCE_MANAGER_INSTANCE->GetActorActionResID(avatar_type, m_AvatarID, action);
		if (resid == 0)return;
		if (m_Actor->GetPalette().size() != 0) {
			m_AvatarActions[action] = new Animation(resid, &m_Actor->GetPalette());
		}
		else {
			m_AvatarActions[action] = new Animation(resid);
		}
		
		m_AvatarActions[action]->FrameInterval = m_TimeInterval;
		m_AvatarActions[action]->SetLoop(0);
		m_AvatarActions[action]->Replay();
	}

	if (m_HasWeapon) {
		if (m_WeaponActions.find(action) == m_WeaponActions.end()) {
			auto resid = RESOURCE_MANAGER_INSTANCE->GetActionResID(AVATAR_TYPE_WEAPON, m_WeaponID, action);
			if (resid == 0) {
				m_HasWeapon = false; 
				return;
			}
			m_WeaponActions[action] = new Animation(resid);
			m_WeaponActions[action]->FrameInterval = m_TimeInterval;
			m_WeaponActions[action]->SetLoop(0);
			m_WeaponActions[action]->Replay();
		}
	}
}

Animation* ActionStateMachine::GetAvatar(int action)
{
	if (action == -1)action = m_ActionID;
	EnsureLoadAction(action);
	return m_AvatarActions[action];
}

Animation* ActionStateMachine::GetWeapon(int action)
{
	if (action == -1)action = m_ActionID;
	EnsureLoadAction(action);
	return m_WeaponActions[action];
}

void ActionStateMachine::Reset()
{
	SetAvatar(m_Actor->GetProperty(PROP_AVATAR_ID).toString());
	SetWeapon(m_Actor->GetWeaponAvatarID());

	PathMoveAction* action = new PathMoveAction(m_Actor);
	ChangeAction(action);
}

int ActionStateMachine::GetDirCount(int action)
{
	if (action == -1)action = m_ActionID;
	if (m_AvatarActions[action]) {
		return m_AvatarActions[action]->GroupCount;
	}
	return 4;
}

void ActionStateMachine::AddFrontAnim(Animation* anim)
{
	if (anim == nullptr)return;
	anim->Replay();
	m_FrontAnimQueue.push_back(anim);
}

void ActionStateMachine::AddBackAnim(Animation* anim)
{
	if (anim == nullptr)return;
	anim->Replay();
	m_BackAnimQueue.push_back(anim);
}

void ActionStateMachine::RemoveFrontAnim(Animation* anim)
{
	for (auto it = m_FrontAnimQueue.begin(); it != m_FrontAnimQueue.end();) {
		if ((*it) == anim) {
			delete (*it);
			m_FrontAnimQueue.erase(it);
			break;
		}
	}
}

void ActionStateMachine::RemoveBackAnim(Animation* anim)
{
	for (auto it = m_BackAnimQueue.begin(); it != m_BackAnimQueue.end();) {
		if ((*it) == anim) {
			delete (*it);
			m_BackAnimQueue.erase(it);
			break;
		}
	}
}

void ActionStateMachine::ClearFrontAnim()
{
	for (auto& it : m_FrontAnimQueue) {
		SafeDelete(it);
	}
	m_FrontAnimQueue.clear();
}

void ActionStateMachine::ClearBackAnim()
{
	for (auto& it : m_BackAnimQueue) {
		SafeDelete(it);
	}
	m_BackAnimQueue.clear();
}

BeatNumber* ActionStateMachine::GetBeatNumber()
{
	return m_BeatNumber;
}
#endif


int action_calc_run_to_pos(lua_State*L){
	Actor* actor = lua_check_actor(L, 1);
	Pos runto;
	//CalcRunToPos(actor, actor->GetTarget(), runto);
	lua_pushnumber(L, runto.x);
	lua_pushnumber(L, runto.y);
	return 2;
}

void luaopen_action(lua_State* L)
{
	script_system_register_luac_function(L, action_calc_run_to_pos);
	script_system_register_function(L, action_get_name);
	
}

