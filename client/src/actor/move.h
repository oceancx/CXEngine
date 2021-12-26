#pragma once
#include "pos.h"
#include <list>


class GameMap;
class Actor;

class MoveHandle
{
public:
	MoveHandle(Actor* actor);
	virtual ~MoveHandle();
	void Update();
	void MoveOnScreen(float x, float y);
	void MoveOnScreenWithDuration(Pos offset,float move_dur,bool keep_dir);
	void MoveTo(float x, float y);
	bool IsMove() { return m_bMove; };
	void StopMove();
	
private:
	std::list<Pos> m_MoveList;
	std::list<Pos> m_BackupMoveList;
	bool m_bMove;
	Actor * m_Actor;
	float m_MoveDuration;
	float m_MoveVelocity;
	bool m_bMoveWithDuration;
	bool m_bKeepDir;
};