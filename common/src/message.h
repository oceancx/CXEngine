#pragma once

#include "define_types.h"
#include "singleton.h"


class BaseGameEntity;

enum GameMessageType
{
	MSG_GAME_STATR,
	MSG_COMBAT_ACTOR_ATTACK,
	MSG_COMBAT_ACTOR_DEFEND,
	MSG_PlayerCombatIdleState,
	MSG_PlayerCombatMoveState,
	MSG_PlayerCombatBackState,
	MSG_PlayerCombatAttackState,
	MSG_PlayerCombatBeAttackedState,
	MSG_PlayerCombatBeCastAttackedState,
	MSG_PlayerCombatGlobalState
};






struct Telegram
{
	int64_t Sender;
	int64_t Receiver;
	int MsgType;			//GameMessageType
	int64_t DispatchTime;
	void* ExtraInfo;
    
	bool operator < (const Telegram& telegram) const
    {
        return this->DispatchTime < telegram.DispatchTime;
    }
 
};

class MessageDispatcher final : public Singleton<MessageDispatcher>
{
private: 
	friend class Singleton<MessageDispatcher>;
	std::set<Telegram> m_MessageQueue;
	void Discharge(BaseGameEntity* pReceiver, const Telegram& msg);
	MessageDispatcher(){
		m_MessageQueue.clear();
	}
public:
	
	void DispatchMessageX(int delay,
	uint64_t sender,
	uint64_t receiver,
	int msgType,
	void* ExtraInfo);

	void DispatchDelayedMessage();
};
#define MESSAGE_DISPATCHER_INSTANCE MessageDispatcher::GetInstance()

