#pragma once

#include "define_types.h"
#include "entity.h"

template <class EntityType>
class State
{
public:
	virtual void Enter(EntityType* ) = 0;
	virtual void Execute(EntityType* ) = 0;
 	virtual void Exit(EntityType* ) = 0;
	virtual bool OnMessage(EntityType* , const Telegram& ) = 0;
	virtual ~State(){};
};


template <class EntityType>
class StateMachine
{
private:
	EntityType* m_pOwner;
	State<EntityType>* m_pCurrentState;
	State<EntityType>* m_pPreviousState;
	State<EntityType>* m_pGlobalState;
public:
	StateMachine(EntityType* owner): 
		m_pOwner(owner),
		m_pCurrentState(nullptr),
		m_pPreviousState(nullptr),
		m_pGlobalState(nullptr)
	{}

	~StateMachine()
	{
		m_pOwner = nullptr;
	}
	

	void SetCurrentState(State<EntityType>* state) { m_pCurrentState = state; };
	void SetPreviousState(State<EntityType>* state) { m_pPreviousState = state; };
	void SetGlobalState(State<EntityType>* state) { m_pGlobalState = state; };
	State<EntityType>* GetCurrentState() { return m_pCurrentState; }
	State<EntityType>* GetPreviousState() { return m_pPreviousState; }
	State<EntityType>* GetGlobalState() { return m_pGlobalState; }


	void Update() const
	{
		if(m_pGlobalState) m_pGlobalState->Execute(m_pOwner);
		if(m_pCurrentState) m_pCurrentState->Execute(m_pOwner);
	}

	void ChangeState(State<EntityType>* pNewState)
	{
		assert(pNewState && "StateMachine trying to change to a null state!");

		m_pPreviousState = m_pCurrentState;
		m_pPreviousState->Exit(m_pOwner);
		m_pCurrentState = pNewState;
		m_pCurrentState->Enter(m_pOwner);
	}

	void RevertToPreviousState()
	{
		ChangeState(m_pPreviousState);
	}
	
	bool HandleMessage(const Telegram& msg) const
	{
		if(m_pCurrentState && m_pCurrentState->OnMessage(m_pOwner , msg) )
		{
			return true;
		}
		if(m_pGlobalState && m_pGlobalState->OnMessage(m_pOwner , msg) )
		{
			return true;
		}
		return false;
	}

};

