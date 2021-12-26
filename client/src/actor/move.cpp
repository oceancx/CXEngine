#include "move.h"
#include "window.h"
#include "actor/actor.h"
#include "cxmath.h"
#include "scene/base_scene.h"
#include "scene/game_map.h"
#include <actor/action.h>
#include <logger.h>

MoveHandle::MoveHandle(Actor* actor)
	:m_Actor(actor)
{
	m_bMove = false;
	m_bMoveWithDuration = false;
	m_bKeepDir = false;
}

MoveHandle::~MoveHandle()
{

}

void MoveHandle::Update()
{
	if (!m_Actor->GetProperty(PROP_CAN_MOVE).toBool())return;
	if (m_bMove) {
		Pos moveTo = m_Actor->GetMoveToPos();
		Pos pos = m_Actor->GetPos();
		if (moveTo.x != pos.x || moveTo.y != pos.y) {
			float dt = WINDOW_INSTANCE->GetDeltaTime();
			float localVelocity = 0.f;
			if (m_bMoveWithDuration) {
				localVelocity = m_MoveVelocity * dt;
			}
			else {
				localVelocity = m_Actor->GetProperty(PROP_MOVE_VELOCITY).toFloat() * dt;
			}

			bool useMoveList = !m_MoveList.empty();
			Pos dest;
			if (useMoveList)
			{
				Pos d = m_MoveList.front();
				dest.x = d.x * 20 + 10;
				dest.y = d.y * 20 + 10;
			}
			else
			{
				dest.x = m_Actor->GetMoveToPos().x;
				dest.y = m_Actor->GetMoveToPos().y;
			}

			if (m_Actor->GetMoveDestDistSquare(dest) > localVelocity * localVelocity) {
				float degree = m_Actor->GetMoveDestAngle(dest);
				int m_Dir = m_Actor->GetDirByDegree(degree);
				float stepRangeX = cos(DegreeToRadian(degree));
				float stepRangeY = sin(DegreeToRadian(degree));
				m_Actor->TranslateX(stepRangeX * localVelocity);
				m_Actor->TranslateY(stepRangeY * localVelocity);
				if (!m_bKeepDir) {
					m_Actor->SetDir(m_Dir);
				}
			}
			else
			{
				if (useMoveList)
				{
					m_MoveList.pop_front();
				}
				m_Actor->SetX(dest.x);
				m_Actor->SetY(dest.y);
			}
		}
		else {
			Pos dest;
			dest.x = m_Actor->GetMoveToPos().x;
			dest.y = m_Actor->GetMoveToPos().y;
			m_Actor->SetPos(dest);
			m_bMove = false;
			m_bKeepDir = false;
			m_bMoveWithDuration = false;

			if (!m_Actor->IsCombat()) {
				m_Actor->GetASM()->ClearAction();
				ActionInfo info;
				info.actionID = ACTION_IDLE;
				m_Actor->GetASM()->PushAction(info);
			}
		}
	}

}

void MoveHandle::MoveOnScreen(float x, float y)
{
	m_MoveDuration = 0.0f;
	m_Actor->SetMoveToPos({ x,y });
	m_bMove = true;
}

void MoveHandle::MoveOnScreenWithDuration(Pos offset, float move_dur, bool keepdir)
{
	Pos src = m_Actor->GetPos();
	Pos dest = src + offset;
	cxlog_info("%s move on screen %.1f,%.1f => %.1f,%.1f dur %.1f keepdir %d moveList %d\n",
		m_Actor->GetLogName(), src.x, src.y, dest.x, dest.y, move_dur, keepdir, m_MoveList.size());
	m_MoveDuration = move_dur;
	m_Actor->SetMoveToPos(dest);
	float dist = std::sqrt(m_Actor->GetMoveDestDistSquare(dest));
	m_MoveVelocity = dist / m_MoveDuration;
	m_bMove = true;
	m_bMoveWithDuration = true;
	m_bKeepDir = keepdir;
}
void MoveHandle::MoveTo(float x, float y)
{
	Pos pos = m_Actor->GetPos();
	if (GMath::Astar_GetDistanceSquare(pos.x, pos.y, x, y) < 16) {
		m_Actor->SetPos(x, y);
		m_bMove = false;
		m_BackupMoveList.clear();
		m_MoveList.clear();
		return;
	}
	if (!m_Actor->GetProperty(PROP_CAN_MOVE).toBool())return;
	if (m_Actor->IsCombat()) {
		m_BackupMoveList.clear();
		m_MoveList.clear();
		return;
	}
	if (m_Actor->GetScene() != nullptr && m_Actor->GetScene()->GetGameMap() != nullptr) {
		auto* map = m_Actor->GetScene()->GetGameMap();
		m_BackupMoveList.clear();
		m_BackupMoveList = map->Move(m_Actor->GetBoxX(), m_Actor->GetBoxY(), (int)(x / 20), (int)(y / 20));
		if (m_BackupMoveList.size() < 2)return;
		m_BackupMoveList.pop_front();
		m_BackupMoveList.pop_back();
		m_MoveList = m_BackupMoveList;
		m_Actor->SetMoveToPos({ x,y });
		if (!m_Actor->IsCombat()) {
			m_Actor->GetASM()->ClearAction();
			ActionInfo info;
			info.actionID = ACTION_WALK;
			m_Actor->GetASM()->PushAction(info);
		}
		m_bMoveWithDuration = false;
		m_bKeepDir = false;
		m_bMove = true;
	}
	else {
		m_BackupMoveList.clear();
		float sx = m_Actor->GetBoxX();
		float sy = m_Actor->GetBoxY();
		float dx = (int)(x / 20);
		float dy = (int)(y / 20);
		Pos src(sx, sy);
		Pos dst(dx, dy);
		m_BackupMoveList.push_back(src);
		m_BackupMoveList.push_back(dst);

		m_BackupMoveList.pop_front();
		m_BackupMoveList.pop_back();
		m_MoveList = m_BackupMoveList;
		m_Actor->SetMoveToPos({ x,y });
		if (!m_Actor->IsCombat()) {
			m_Actor->GetASM()->ClearAction();
			ActionInfo info;
			info.actionID = ACTION_WALK;
			m_Actor->GetASM()->PushAction(info);
		}
		m_bMoveWithDuration = false;
		m_bKeepDir = false;
		m_bMove = true;
	}



}

void MoveHandle::StopMove()
{
	m_bMove = false;
}

