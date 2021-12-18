#include "message.h"
#include <chrono>
#include "entity.h"

void MessageDispatcher::Discharge(BaseGameEntity* pReceiver, const Telegram& msg)
{
    if(pReceiver)
	pReceiver->HandleMessage(msg);
}


void MessageDispatcher::DispatchMessageX(int delay,
	uint64_t sender,
	uint64_t receiver,
	int msgType,
	void* extraInfo)
{
	BaseGameEntity* pReceiver = GAME_ENTITY_MANAGER_INSTANCE->GetEntityByID(receiver);

    Telegram telegram;
    telegram.Sender = sender;
    telegram.Receiver = receiver;
    telegram.MsgType = msgType;
    telegram.ExtraInfo = extraInfo;
	if(delay <= 0 )
	{
		Discharge(pReceiver,telegram);
	}
	else
	{
    	auto now = std::chrono::system_clock::now();
   		auto currentTime = now.time_since_epoch().count() /1000;
		telegram.DispatchTime = currentTime + delay;
		m_MessageQueue.insert(telegram);
	}
}

void MessageDispatcher::DispatchDelayedMessage()
{
   	auto now = std::chrono::system_clock::now();
   	auto currentTime = now.time_since_epoch().count() /1000;

	while(m_MessageQueue.size()>0&& (m_MessageQueue.begin()->DispatchTime < currentTime) &&
		(m_MessageQueue.begin()->DispatchTime > 0) )
	{
		Telegram telegram = *m_MessageQueue.begin();
		BaseGameEntity* pReceiver = GAME_ENTITY_MANAGER_INSTANCE->GetEntityByID(telegram.Receiver);
		Discharge(pReceiver,telegram);
		m_MessageQueue.erase(m_MessageQueue.begin());
	}
}
