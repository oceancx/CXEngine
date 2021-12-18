#pragma once

#include <string>
#include <pos.h>
#include <list>
#include <define_types.h>

std::string action_get_name(int i);
size_t action_get_size();
std::string action_system_get_action(int i);
bool action_is_show_weapon(int action);
bool action_is_emotion_action(int action);
bool action_is_battle_action(int action);


#ifndef SIMPLE_SERVER
class Actor;
class ActionStateMachine;
class Animation;
class BeatNumber;
class Action
{
public:
	Action(Actor* actor);
	virtual ~Action() {};
	virtual void Enter();
	virtual void Exit() {};
	virtual  void Update();
	virtual  void Draw() {};

	void OnEnter();
	void OnExit();
	EASMActionType GetType() { return m_Type; }
protected:
	EASMActionType m_Type;
	ActionStateMachine *  m_pASM;
	Actor* m_Actor;
};

class CastAction :public Action
{
public:
	CastAction(Actor* actor) :Action(actor){ m_Type = ASM_CAST_ACTION; };
	virtual ~CastAction() {};
	virtual  void Update();
	virtual void Exit();
	virtual void Enter();
private:
	Pos m_AttackVec;
	Actor* m_Target;
};


class BeHitAction : public Action
{
public:
	BeHitAction(Actor* actor, Actor* attacker) :Action(actor), m_Attacker(attacker) { m_Type = ASM_BEHIT_ACTION; };
	virtual ~BeHitAction() {};
	virtual  void Update();
	virtual void Exit();
	virtual void Enter();
	void Draw();
	Pos MoveVec;
private:
	Actor* m_Attacker;
	int m_State;
	Pos m_SavedPos;
	bool m_bMoveEnd;
};

class BeCastAction : public Action
{
public:
	BeCastAction(Actor* actor, Actor* attacker) :Action(actor), m_Attacker(attacker) { m_Type = ASM_BECAST_ACTION; };
	virtual ~BeCastAction() {};
	virtual  void Update();
	virtual void Exit();
	virtual void Enter();
	Pos MoveVec;
private:
	Actor* m_Attacker;
	int m_State;
};

class DeadFlyAction : public Action
{
public:
	DeadFlyAction(Actor* actor, Actor* attacker, Pos dir) :Action(actor), m_Attacker(attacker), m_Dir(dir) { m_Type = ASM_DEAD_FLY_ACTION; };
	virtual ~DeadFlyAction() {};
	virtual  void Update();
	virtual void Enter();
	virtual void Exit();
private:
	int m_SavedDir;
	Pos m_Dir;
	Pos m_FlyPos;
	Pos m_SavedPos;
	Actor* m_Attacker;
};


class PathMoveAction : public Action
{
public:
	PathMoveAction(Actor* actor) :Action(actor) { m_Type = ASM_PATH_MOVE_ACTION; };
	virtual ~PathMoveAction() {};
	virtual  void Update();
	virtual void Exit();
	virtual void Enter();
private:
	int m_State;
};

struct ActionInfo
{
	float move_dur = 0;
	float dx = 0;
	float dy = 0;
	bool loop = false;
	int loopCount = 0;
	int actionID = ACTION_IDLE;
	int updateCB = -1;
	float interval = -1;
};

class ActionStateMachine
{
public:
	ActionStateMachine(Actor* actor);
	~ActionStateMachine();
	void Update();
	void Draw();

	void SetWeapon(CXString id);
	void SetAvatar(CXString id);
	void SetAction(int id);

	void RestoreAction();
	void ChangeAction(Action* action);
	void EnsureLoadAction(int action);
	
	Animation* GetAvatar(int action = -1);
	Animation* GetWeapon(int action = -1);
	bool HasWeapon() { return m_HasWeapon; }
	Action* GetAction() { return m_pCurrentAction; }
	void SetTimeInterval(float ti) { m_TimeInterval = ti; }
	void Reset();
	int GetDirCount(int action = -1);

	int GetActionID() { return m_ActionID; };

	void AddFrontAnim(Animation* anim);
	void AddBackAnim(Animation* anim);
	void RemoveFrontAnim(Animation* anim);
	void RemoveBackAnim(Animation* anim);
	void ClearFrontAnim();
	void ClearBackAnim();

	void MoveActionToBack() { m_bMoveActionToBack = true; }
	void PushAction(ActionInfo& info) { 
		m_ActionQueue.push_back(info); 
	}
	void ClearAction() { m_ActionQueue.clear(); }
	int GetCurrentAction() { 
		if (m_ActionQueue.empty())return -1;
		auto info = m_ActionQueue.front();
		return info.actionID;
	}
	BeatNumber* GetBeatNumber();
private:
	Actor * m_Actor;
	float m_TimeInterval;
	int m_ActionID;
	CXString m_WeaponID;
	CXString m_AvatarID;
	bool m_HasWeapon;
	Action* m_pPreviousAction;
	Action* m_pCurrentAction;
	std::map<int, Animation*> m_WeaponActions;
	std::map<int, Animation*> m_AvatarActions;
	Animation* m_PlayerShadow;
	deque<Animation*> m_FrontAnimQueue;
	deque<Animation*> m_BackAnimQueue;
	bool		m_bMoveActionToBack;
	deque<ActionInfo> m_ActionQueue;

	BeatNumber* m_BeatNumber;
};
#endif // !SIMPLE_SERVER


void luaopen_action(lua_State* L);