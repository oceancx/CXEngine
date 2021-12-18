#pragma once

#include "define_types.h"
#include "pos.h"
#include "entity.h"
#include <vector>

class MoveHandle;
#ifndef SIMPLE_SERVER
#include "ui.h"
#include <NESupport.h>
class ActionStateMachine;
class TextView;
class UITextView;
#endif
#include "actor_prop.h"
#include "actor_event.h"

class GameMap;

class BaseScene;


#ifndef SIMPLE_SERVER
class Actor : public BaseGameEntity, public View
#else
class Actor : public BaseGameEntity
#endif
{
public:
	Actor(uint64_t id);

	virtual ~Actor();
	virtual void OnUpdate();
	virtual void OnDraw(GameMap* gameMapPtr) {};
	virtual void OnDraw(int x, int y) {};
	virtual void OnDraw();
	virtual bool HandleMessage(const Telegram& msg) { return false; };

	virtual void SetDir(int dir);

	uint64_t GetID() { return GetProperty(PROP_ID).toUInt64(); }
	int GetDir();
	int GetDirByDegree(float degree);
	Pos  GetAttackVec();
	void ReverseDir();

	void SetActionID(int state);

	void SetPos(float x, float y);
	void SetPos(Pos p);
	Pos GetPos();

	void SetMoveToPos(Pos dest);
	Pos GetMoveToPos();

	int GetBoxX() { return static_cast<int>(GetPos().x / 20); }
	int GetBoxY() { return static_cast<int>(GetPos().y / 20); }

	int GetX() { return static_cast<int>(GetPos().x); }
	int GetY() { return static_cast<int>(GetPos().y); }

	void SetX(float x) { ActorProp& prop= m_Props[IsCombat() ? PROP_COMBAT_POS : PROP_POS]; prop.v2[0] = x; }
	void SetY(float y) { ActorProp& prop = m_Props[IsCombat() ? PROP_COMBAT_POS : PROP_POS]; prop.v2[1] = y; }

	void TranslateX(float x) { ActorProp& prop = m_Props[IsCombat() ? PROP_COMBAT_POS : PROP_POS]; prop.v2[0] += x; }
	void TranslateY(float y) { ActorProp& prop = m_Props[IsCombat() ? PROP_COMBAT_POS : PROP_POS]; prop.v2[1] += y; }

	virtual float GetWidth();
	virtual float GetHeight();

	bool IsCombat() { return m_Props[PROP_IS_COMBAT].toBool(); }

	void SetLocal(bool local);
	bool IsLocal();

	bool IsMove() {  return m_Props[PROP_IS_MOVE].toBool(); }

	float GetMoveDestDistSquare(Pos dest);
	float GetMoveDestAngle(Pos dest);

	void SetTurnReady(bool ready) { m_Props[PROP_TURN_READY] = ready;};
	bool IsTurnReady() { return m_Props[PROP_TURN_READY].toBool(); };

	void SetTarget(Actor* target) { m_Target = target; };
	Actor* GetTarget() { return m_Target; };

	const char* GetLogName();

	CXString GetWeaponAvatarID();
#ifndef SIMPLE_SERVER
	MoveHandle* GetMoveHandle() { return m_MoveHandle; }
	BaseScene* GetScene();
	std::vector<NE::PalSchemePart>& GetPalette() { return m_PatMatrix; }
	void SetPalette(std::vector<NE::PalSchemePart> patMatrix) { m_PatMatrix = patMatrix; }
	ActionStateMachine* GetASM() { return m_ASM; };
	Bound GetViewBounds() override;
	bool CheckDrag(int x, int y) override;
	void OnDragMove(int x, int y)override;
	void OnHover(float x, float y) override;
	bool OnClick(int button, int x, int y) override;
	void Say(std::string Text);
#endif
	

	template<typename T>
	void CopyProperty(int index,T& out) {
		ActorProp& prop = m_Props[index];
		switch (prop.type)
		{
		case PROP_TYPE_BOOL:
			out = prop.b;
			break;
		case PROP_TYPE_INT:
			out = prop.i;
			break;
		case PROP_TYPE_UINT64:
			out = prop.l;
			break;
		case PROP_TYPE_FLOAT:
			out = prop.f;
			break;
		case PROP_TYPE_STR:
			out = prop.s;
			break;
		case PROP_TYPE_VEC2:
			out[0] = prop.v2[0];
			out[1] = prop.v2[1];
			break;
		}
	}

	ActorProp& GetProperty(int index) { return m_Props[index]; }
	void SetProperty(int index, ActorProp prop) { 
		m_Props[index] = prop; 
		DirtyProperty(index);
	}

	void DirtyProperty(int index) {
		m_DirtyProps.insert(index);
	}

	void RegProperty(int index, ActorProp& prop) { assert(index == m_Props.size()); m_Props.push_back(prop); }

	bool IsDirty() { return m_DirtyProps.size() != 0; }

	std::set<int> GetDirtyProps() { return m_DirtyProps; }
	void ClearDirty() { m_DirtyProps.clear(); }
protected:
	bool m_bCalcMoveList;
	int m_SayDuration;
	std::list<Pos> m_MoveList;
	std::list<Pos> m_BackupMoveList;
	
	
	
	std::vector<NE::PalSchemePart> m_PatMatrix;
	std::vector<ActorProp> m_Props;
	std::set<int> m_DirtyProps;
	Actor* m_Target;
	CXString m_SayText;
#ifndef SIMPLE_SERVER
	MoveHandle* m_MoveHandle;
	TextView* m_SayWidget;
	UITextView* m_NameTV;
	UITextView* m_SayTV;
	ActionStateMachine* m_ASM;
#endif
};

Actor* lua_check_actor(lua_State*L, int index);
void lua_push_actor(lua_State*L, Actor* actor);
void luaopen_actor(lua_State* L);
