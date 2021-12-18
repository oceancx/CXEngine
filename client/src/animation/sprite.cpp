#include "animation/sprite.h"
#include <NESupport.h>
#include "texture_manager.h"
#include "resource_manager.h"
#include "window.h"
#include "sprite_renderer.h"
#include "input_manager.h"
#include "logger.h"
#include "cxmath.h"
#include "actor/actor_manager.h"
#include "graphics/ui_renderer.h"


String UtilsGetFramePath(Sprite* m_pSprite, int index)
{
	if (m_pSprite)
	{
		return m_pSprite->Path + std::string("/" + std::to_string(index));
	}
	return "";
}

Texture* UtilsGetFrameTexture(Sprite* m_pSprite, int index)
{
	if (!m_pSprite)return nullptr;
	if (index >= m_pSprite->GroupFrameCount * m_pSprite->GroupCount)return nullptr;
	auto path = UtilsGetFramePath(m_pSprite, index);
	auto& frame = m_pSprite->Frames[index];
	return TextureManager::GetInstance()->LoadTexture(path, frame.Width, frame.Height, true, (uint8_t*)frame.Src.data());
}

BaseSprite::BaseSprite(uint64_t resoureID, std::vector<PalSchemePart>* patMatrix)
	:ResID(resoureID)
{
	if (resoureID == 0) { m_pSprite = nullptr; return; }
	spritInfo = RESOURCE_MANAGER_INSTANCE->LoadSprite(resoureID, patMatrix);
	spritInfo->refCount++;

	m_pSprite = spritInfo->sprite;
	Width = m_pSprite->Width;
	Height = m_pSprite->Height;
	KeyX = m_pSprite->KeyX;
	KeyY = m_pSprite->KeyY;
	TotalFrames = m_pSprite->GroupCount * m_pSprite->GroupFrameCount;
	GroupFrameCount = m_pSprite->GroupFrameCount;
	GroupCount = m_pSprite->GroupCount;
	m_pSprite->Path = std::to_string(resoureID) + "/" + std::to_string(spritInfo->pati);

	CurrentFrame = 0;
	for (int i = 0; i < TotalFrames; i++)
	{
		UtilsGetFrameTexture(m_pSprite, i);
	}
	Pos = { 0,0 };
	Offset = { 0,0 };
	FrameInterval = 0.064f;
	PlayTime = 0.f;
	Dir = 0;
	bEnableDrag = false;
}

BaseSprite::BaseSprite(uint32_t pkg, uint32_t wasID, std::vector<PalSchemePart>* patMatrix) :BaseSprite(RESOURCE_MANAGER_INSTANCE->EncodeWAS(pkg, wasID), patMatrix) {}

BaseSprite::~BaseSprite()
{
	INPUT_MANAGER_INSTANCE->UnRegisterView(this);
	spritInfo->refCount--;
	if (spritInfo->refCount == 0) {
		for (int i = 0; i < m_pSprite->GroupFrameCount * m_pSprite->GroupCount; i++) {
			TEXTURE_MANAGER_INSTANCE->UnloadTexture(UtilsGetFramePath(m_pSprite, i));
		}
		RESOURCE_MANAGER_INSTANCE->UnLoadSprite(ResID, &spritInfo->pat);
	}
	m_pSprite = nullptr;
}

NE::Sprite::Sequence* BaseSprite::GetFrame(int index /*= -1*/)
{
	if (!m_pSprite)return nullptr;
	if (index == -1)index = CurrentFrame;
	if (index >= m_pSprite->Frames.size())return 0;
	return &m_pSprite->Frames[index];
}
int BaseSprite::GetFrameKeyX(int index /*= -1*/)
{
	if (!m_pSprite)return 0;
	if (index == -1)index = CurrentFrame;
	int frame = Dir * GroupFrameCount + index;
	if (frame >= m_pSprite->Frames.size())return 0;
	return m_pSprite->Frames[frame].KeyX;
}

int BaseSprite::GetFrameKeyY(int index /*= -1*/)
{
	if (!m_pSprite)return 0;
	if (index == -1)index = CurrentFrame;
	int frame = Dir * GroupFrameCount + index;
	if (frame >= m_pSprite->Frames.size())return 0;
	return m_pSprite->Frames[frame].KeyY;
}

int BaseSprite::GetFrameWidth(int index /*= -1*/)
{
	if (!m_pSprite)return 0;
	if (index == -1)index = CurrentFrame;
	int frame = Dir * GroupFrameCount + index;
	if (frame >= m_pSprite->Frames.size())return 0;
	return m_pSprite->Frames[frame].Width;
}


int BaseSprite::GetFrameHeight(int index /*= -1*/)
{
	if (!m_pSprite)return 0;
	if (index == -1)index = CurrentFrame;
	int frame = Dir * GroupFrameCount + index;
	if (frame >= m_pSprite->Frames.size())return 0;
	return m_pSprite->Frames[frame].Height;
}

void BaseSprite::EnableDrag(bool enable)
{
	bEnableDrag = enable;
	if (bEnableDrag) {
		INPUT_MANAGER_INSTANCE->RegisterView(this);
	}
	else {
		INPUT_MANAGER_INSTANCE->UnRegisterView(this);
	}
}

float BaseSprite::GetGroupFrameTime()
{
	return FrameInterval * GroupFrameCount;
}
Bound BaseSprite::GetViewBounds()
{
	return Bound{ Pos.x - KeyX, Pos.x - KeyX + Width,
		Pos.y - KeyY,Pos.y - KeyY + Height };
}

bool BaseSprite::CheckDrag(int dx, int dy)
{
	return bEnableDrag && pow(dx, 2) + pow(dy, 2) >= 16;
}

void BaseSprite::OnDragMove(int dx, int dy)
{
	Pos.x += (float)dx;
	Pos.y += (float)dy;
}

Animation::Animation(uint64_t resoureID /*= 0*/, std::vector<PalSchemePart>* patMatrix) :BaseSprite(resoureID, patMatrix)
{
	m_Visible = true;
	m_State = ANIMATION_STOP;
	m_LoopCount = 0;
	m_LoopCounter = 0;
	m_bFrameUpdate = false;
	m_bGroupEndUpdate = false;
	int max_dx = 0;
	int max_frame = 0;
	for (int i = std::max(GroupFrameCount / 2 - 1, 0); i < GroupFrameCount - 1; i++) {
		int dx = (int)std::pow((m_pSprite->Frames[i].KeyX - m_pSprite->Frames[i + 1].KeyX), 2);// + std::abs(m_pSprite->Frames[i].width*m_pSprite->Frames[i].height - m_pSprite->Frames[i + 1].width*m_pSprite->Frames[i + 1].height);
		if (max_dx < dx) {
			max_dx = dx;
			max_frame = i + 1;
		}
	}
	AttackKeyFrame = max_frame;
	m_bTranslate = false;
	m_TranslationPos.x = Pos.x;
	m_TranslationPos.y = Pos.y;
	m_Velocity.x = 0;
	m_Velocity.y = 0;
	m_bLockFrame = false;
	m_LockFrame = CurrentFrame;
	m_bLoop = true;
	m_StopCBRef = -1;
	m_StartCBRef = -1;
	m_LoopCBRef = -1;
	m_UpdateCBRef = -1;
	m_LoopMode = ANIMATION_LOOPMODE_RESTART;

	lua_State* L = script_system_get_luastate();
	m_UserData = (Animation**)lua_newuserdata(L, sizeof(Animation*));
	*m_UserData = this;
	lua_newtable(L);
	lua_setuservalue(L, -2);
	m_LuaRef = luaL_ref(L, LUA_REGISTRYINDEX);
	m_Rotation = 0;
	//lua_pop(L, 1);
}

Animation::Animation(uint32_t pkg, uint32_t wasID, std::vector<PalSchemePart>* patMatrix)
	:Animation(RESOURCE_MANAGER_INSTANCE->EncodeWAS(pkg, wasID), patMatrix)
{

}

Animation::~Animation()
{
	lua_State* L = script_system_get_luastate();
	for (auto& data : m_CallbackQueueLua) {
		luaL_unref(L, LUA_REGISTRYINDEX, data.func);
	}
	if (m_LuaRef != LUA_NOREF) {
		luaL_unref(L, LUA_REGISTRYINDEX, m_LuaRef);
	}

}

void Animation::SetLoop(int loop, int mode)
{
	if (loop < 0) {
		m_LoopCounter = 0;
		m_LoopCount = 0;
		m_bLoop = false;
	}
	else {
		m_LoopCounter = 0;
		m_LoopCount = loop;
		m_bLoop = true;
	}
	m_LoopMode = mode;
}

void Animation::Reset()
{
	PlayTime = 0;
	CurrentFrame = 0;
	m_bGroupEndUpdate = false;
}

void Animation::Stop()
{
	m_PreviousState = m_State;
	m_State = ANIMATION_STOP;
	CallLuaFunByRef(m_StopCBRef);
}

void Animation::Play()
{
	m_PreviousState = m_State;
	m_State = ANIMATION_PLAY;
	CallLuaFunByRef(m_StartCBRef);
}

void Animation::Replay()
{
	Reset();
	Play();
	m_Visible = true;
}

void Animation::AddCallbackLua(float dur, int funcRef)
{
	CBDataLua data(dur, funcRef);
	m_CallbackQueueLua.push_back(data);
}

void Animation::AddCallback(float dur, function<void()> func)
{
	CBData data(dur, func);
	m_CallbackQueue.push_back(std::move(data));
}

void Animation::AddFrameCallback(int frame, std::function<void()> callback)
{
	m_Callbacks[frame] = callback;
}

void Animation::TranslateTo(CXPos pos, int duration)
{
	m_bTranslatePos = true;
	m_Duration = duration / 1000.f;
	m_TranslationToPos = pos;
	m_Velocity = CXPos((m_TranslationToPos.x - Pos.x) / m_Duration, (m_TranslationToPos.y - Pos.y) / m_Duration);
}

void Animation::Translate(CXPos pos, int duration)
{
	m_bTranslate = true;
	m_Duration = duration / 1000.f;
	m_TranslationPos = Pos;
	m_TranslationToPos = pos;
	m_Velocity = CXPos((m_TranslationToPos.x - m_TranslationPos.x) / m_Duration, (m_TranslationToPos.y - m_TranslationPos.y) / m_Duration);
}


void Animation::LockFrame(int frame)
{
	m_bLockFrame = true;
	CurrentFrame = frame;
	PlayTime = 0;
}

void Animation::UnLockFrame()
{
	m_bLockFrame = false;
}


void Animation::AddStopCallback(int funcRef)
{
	m_StopCBRef = funcRef;
}


void Animation::AddStartCallback(int funcRef)
{
	m_StartCBRef = funcRef;
}

void Animation::AddLoopCallback(int funcRef)
{
	m_LoopCBRef = funcRef;
}


void Animation::RemoveLoopCallback()
{
	if (m_LoopCBRef != -1) {
		auto* L = script_system_get_luastate();
		luaL_unref(L, LUA_REGISTRYINDEX, m_LoopCBRef);
		m_LoopCBRef = -1;
	}
}


void Animation::AddUpdateCallback(int funcRef)
{
	m_UpdateCBRef = funcRef;
}

void Animation::RemoveUpdateCallback()
{
	if (m_UpdateCBRef != -1) {
		auto* L = script_system_get_luastate();
		luaL_unref(L, LUA_REGISTRYINDEX, m_UpdateCBRef);
		m_UpdateCBRef = -1;
	}
}

void Animation::CallLuaFunByRef(int& ref)
{
	if (ref != -1) {
		lua_State* L = script_system_get_luastate();
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		lua_push_animation(L, this);
		int res = lua_pcall(L, 1, 0, 0);
		check_lua_error(L, res);
		luaL_unref(L, LUA_REGISTRYINDEX, ref);
		ref = -1;
	}
}

void Animation::RemoveFrameCallback(int frame)
{
	m_Callbacks.erase(frame);
}

void Animation::Update()
{
	if (!m_pSprite)return;
	m_bGroupEndUpdate = false;
	m_bFrameUpdate = false;
	float dt = WINDOW_INSTANCE->GetDeltaTime();
	if (m_State == ANIMATION_PLAY) {
		if (m_bLockFrame) return;
		if (m_bTranslate) {
			float dist = std::pow(m_Velocity.x * dt, 2) + std::pow(m_Velocity.y * dt, 2);
			if (GMath::Astar_GetDistanceSquare(m_TranslationPos.x, m_TranslationPos.y, m_TranslationToPos.x, m_TranslationToPos.y) > dist) {
				m_TranslationPos.x = m_TranslationPos.x + m_Velocity.x * dt;
				m_TranslationPos.y = m_TranslationPos.y + m_Velocity.y * dt;
			}
			else {
				m_TranslationPos.x = m_TranslationToPos.x;
				m_TranslationPos.y = m_TranslationToPos.y;
				m_bTranslate = false;
			}
		}

		if (m_bTranslatePos) {
			float dist = std::pow(m_Velocity.x * dt, 2) + std::pow(m_Velocity.y * dt, 2);
			if (GMath::Astar_GetDistanceSquare(Pos.x, Pos.y, m_TranslationToPos.x, m_TranslationToPos.y) > dist) {
				Pos.x = Pos.x + m_Velocity.x * dt;
				Pos.y = Pos.y + m_Velocity.y * dt;
			}
			else {
				Pos.x = m_TranslationToPos.x;
				Pos.y = m_TranslationToPos.y;
				m_bTranslatePos = false;
			}
		}

		for (auto& wrap : m_CallbackQueue) {
			wrap.dur -= dt;
			if (wrap.dur <= 0) {
				wrap.func();
				wrap.markd = true;
			}
		}
		for (auto it = m_CallbackQueue.begin(); it != m_CallbackQueue.end();) {
			if (it->markd) {
				it = m_CallbackQueue.erase(it);
			}
			else {
				it++;
			}
		}

		for (auto& wrap : m_CallbackQueueLua) {
			wrap.dur -= dt;
			if (wrap.dur <= 0) {
				lua_State* L = script_system_get_luastate();
				int ref = wrap.func;
				CallLuaFunByRef(ref);
				wrap.markd = true;
			}
		}
		for (auto it = m_CallbackQueueLua.begin(); it != m_CallbackQueueLua.end();) {
			if (it->markd) {
				it = m_CallbackQueueLua.erase(it);
			}
			else {
				it++;
			}
		}

		PlayTime = PlayTime + dt;
		if (PlayTime >= FrameInterval)
		{
			m_bFrameUpdate = true;
			PlayTime = (PlayTime - std::floor(PlayTime / FrameInterval) * FrameInterval);
			CurrentFrame = CurrentFrame + 1;
			if (m_Callbacks.find(CurrentFrame) != m_Callbacks.end()) {
				m_Callbacks[CurrentFrame]();
			}
			if (CurrentFrame >= GroupFrameCount) {
				m_bGroupEndUpdate = true;
				if (m_bLoop) {
					if (m_LoopMode == ANIMATION_LOOPMODE_RESTART) {
						CurrentFrame = 0;
					}
					else if (m_LoopMode == ANIMATION_LOOPMODE_STOPFIX) {
						CurrentFrame = GroupFrameCount - 1;
					}
					else if (m_LoopMode == ANIMATION_LOOPMODE_CHANGE_DIR) {
						CurrentFrame = 0;
						if (Dir < GroupCount) {
							Dir = Dir + 1;
						}
						if (Dir == GroupCount) {
							Dir = 0;
						}
					}

					if (m_LoopCounter < m_LoopCount) {
						m_LoopCounter = m_LoopCounter + 1;
						if (m_LoopCBRef != -1) {
							lua_State* L = script_system_get_luastate();
							lua_rawgeti(L, LUA_REGISTRYINDEX, m_LoopCBRef);
							lua_push_animation(L, this);
							lua_pushinteger(L, m_LoopCounter);
							int res = lua_pcall(L, 2, 0, 0);
							check_lua_error(L, res);
						}
						if (m_LoopCounter == m_LoopCount) {
							m_bLoop = false;
							if (m_LoopCBRef != -1) {
								lua_State* L = script_system_get_luastate();
								luaL_unref(L, LUA_REGISTRYINDEX, m_LoopCBRef);
								m_LoopCBRef = -1;
							}

							Stop();
							return;
						}
					}
				}
				else {
					CurrentFrame = CurrentFrame - 1;
					Stop();
					return;
				}
			}
		}
	}
	else if (m_State == ANIMATION_STOP) {

	}
	else if (m_State == ANIMATION_PAUSE) {
		int ms = (int)(dt * 1000);
		m_PauseTime = m_PauseTime - ms;
		if (m_PauseTime <= 0) {
			m_State = m_PreviousState;
		}
	}
	if (m_UpdateCBRef != -1) {
		lua_State* L = script_system_get_luastate();
		lua_rawgeti(L, LUA_REGISTRYINDEX, m_UpdateCBRef);
		lua_push_animation(L, this);
		int res = lua_pcall(L, 1, 0, 0);
		check_lua_error(L, res);
	}

}

void Animation::Draw()
{
	if (!m_pSprite)return;
	if (!m_Visible)return;

	auto* texture = UtilsGetFrameTexture(m_pSprite, Dir * GroupFrameCount + CurrentFrame);
	if (texture)
	{
		auto& frame = m_pSprite->Frames[Dir * GroupFrameCount + CurrentFrame];
		if (m_bTranslate) {
			SPRITE_RENDERER_INSTANCE->DrawFrameSprite(texture,
				glm::vec2(m_TranslationPos.x - frame.KeyX, m_TranslationPos.y - frame.KeyY),
				glm::vec2(frame.Width, frame.Height), DegreeToRadian(m_Rotation), glm::vec3(1.0f, 1.0f, 1.0f));
		}
		else {
			SPRITE_RENDERER_INSTANCE->DrawFrameSprite(texture,
				glm::vec2(Pos.x - frame.KeyX, Pos.y - frame.KeyY),
				glm::vec2(frame.Width, frame.Height), DegreeToRadian(m_Rotation), glm::vec3(1.0f, 1.0f, 1.0f));
		}

	}
}

void Animation::SetLuaRef(int ref)
{
	m_LuaRef = ref;
}

void Animation::Pause(int ms)
{
	m_PauseTime = ms;
	m_PreviousState = m_State;
	m_State = ANIMATION_PAUSE;
}

BeatNumber::BeatNumber() :
	m_HitAnim(MISCWDF, 0x30F737D8),
	m_HealAnim(MISCWDF, 0x3CF8F9FE),
	m_Number(0),
	m_Pos(0, 0),
	m_PlayTime(0),
	m_BeatTime(0.2f),
	m_AdvanceX(12.f),
	m_BeatHeights(2.5f),
	m_Visible(false),
	m_PauseTime(0.2f)
{

}

void BeatNumber::Update()
{
	if (!m_Visible)return;
	float dt = WINDOW_INSTANCE->GetDeltaTime();
	m_PlayTime += dt;
	if (m_bBeat) {
		for (int i = 0; i < m_Digits.size(); i++)
		{
			Digit& dig = m_Digits[i];
			float start_time = i * (m_BeatTime / m_BeatHeights);
			if (m_PlayTime > start_time) {
				float dur = m_PlayTime - start_time;
				auto& anim = m_ShowHeal ? m_HealAnim : m_HitAnim;
				dig.y = (anim.m_pSprite->Height * m_BeatHeights / m_BeatTime) * dur;
				if (dig.y > anim.m_pSprite->Height * m_BeatHeights * 2) {
					dig.y = 0;
					if (i == m_Digits.size() - 1) {
						m_bBeat = false;
						m_PlayTime = -m_PauseTime;
					}
				}
				else if (dig.y > anim.m_pSprite->Height * m_BeatHeights) {
					dig.y = anim.m_pSprite->Height * m_BeatHeights * 2 - dig.y;
				}
			}
		}
	}
	else {
		if (m_PlayTime >= 0) {
			m_PlayTime = 0;
			m_Visible = false;
		}
	}
}

void BeatNumber::Draw()
{
	if (!m_Visible)return;

	float px = m_Pos.x - m_Digits.size() * m_AdvanceX / 2.f;
	for (int i = 0; i < m_Digits.size(); i++)
	{
		Digit& dig = m_Digits[i];
		float x = px + i * m_AdvanceX;
		float y = m_Pos.y - dig.y;
		auto& anim = m_ShowHeal ? m_HealAnim : m_HitAnim;
		auto* texture = UtilsGetFrameTexture(anim.m_pSprite, dig.digit);
		if (texture)
		{
			auto& frame = anim.m_pSprite->Frames[dig.digit];
			SPRITE_RENDERER_INSTANCE->DrawFrameSprite(texture,
				glm::vec2(x - frame.KeyX, y - frame.KeyY),
				glm::vec2(frame.Width, frame.Height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
		}
	}
}

void BeatNumber::SetPos(float x, float y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

void BeatNumber::SetNumber(float number)
{
	if (number > 0)
		m_ShowHeal = true;
	else
		m_ShowHeal = false;
	number = std::abs(number);
	m_Number = (int)std::floor(number);
	int num = m_Number;
	m_Digits.clear();
	int i = 0;
	do {
		int mod = num % 10;
		Digit d;
		d.y = 0;
		d.digit = mod;
		m_Digits.push_front(d);
		num = num / 10;
	} while (num);
}

void BeatNumber::Beat()
{
	m_Visible = true;
	m_PlayTime = -m_PauseTime;
	m_bBeat = true;
}


AnimationManager::AnimationManager()
{
	m_Animations.clear();
	m_BeatNumbers.clear();
}

AnimationManager::~AnimationManager()
{
	for (auto& it : m_Animations) {
		delete it;
	}
	m_Animations.clear();

	for (auto& it : m_BeatNumbers) {
		delete it;
	}
	m_BeatNumbers.clear();
}

void AnimationManager::AddAnimation(Animation* animation)
{
	m_Animations.push_back(animation);
}

void AnimationManager::AddBeatNumber(BeatNumber* bn)
{
	m_BeatNumbers.push_back(bn);
}

void AnimationManager::Update()
{
	for (auto& anim : m_Animations) {
		anim->Update();
	}
	for (auto it = m_Animations.begin(); it != m_Animations.end();) {
		if ((*it)->GetState() == ANIMATION_STOP) {
			delete* it;
			it = m_Animations.erase(it);
		}
		else {
			it++;
		}
	}

	for (auto& beatnum : m_BeatNumbers) {
		beatnum->Update();
	}
	for (auto it = m_BeatNumbers.begin(); it != m_BeatNumbers.end();) {
		if ((*it)->GetVisible() == false) {
			delete* it;
			it = m_BeatNumbers.erase(it);
		}
		else {
			it++;
		}
	}
}


void AnimationManager::Draw()
{
	for (auto& it : m_Animations) {
		it->Draw();
	}
	for (auto& it : m_BeatNumbers)
	{
		it->Draw();
	}
}

void AnimationManager::Clear()
{
	m_Animations.clear();
	m_BeatNumbers.clear();
}

BaseSprite* lua_check_base_sprite(lua_State* L, int index)
{
	return lua_check_pointer<BaseSprite>(L, index);
}


Animation* lua_check_animation(lua_State* L, int index)
{
	return lua_check_pointer<Animation>(L, index);
}

int base_sprite_update(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	sp->Update();
	return 0;
}

int base_sprite_draw(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	sp->Draw();
	return 0;
}

int base_sprite_set_pos(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	auto x = (float)lua_tonumber(L, 2);
	auto y = (float)lua_tonumber(L, 3);
	sp->Pos.x = x;
	sp->Pos.y = y;
	return 0;
}

int base_sprite_get_pos(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushnumber(L, sp->Pos.x);
	lua_pushnumber(L, sp->Pos.y);
	return 2;
}

int base_sprite_set_dir(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	auto dir = (int)lua_tointeger(L, 2);
	sp->Dir = dir;
	return 0;
}

int base_sprite_get_dir(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->Dir);
	return 1;
}


int base_sprite_set_frame_interval(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	auto interval = (float)lua_tonumber(L, 2);
	sp->FrameInterval = interval;
	sp->CurrentFrame = 0;
	sp->PlayTime = 0.f;
	return 0;
}

int base_sprite_get_frame_interval(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushnumber(L, sp->FrameInterval);
	return 1;
}

int base_sprite_get_width(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->Width);
	return 1;
}

int base_sprite_get_height(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->Height);
	return 1;
}


int base_sprite_set_width(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	float w = (float)lua_tonumber(L, 2);
	sp->Width = (int)w;
	return 0;
}

int base_sprite_set_height(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	float h = (float)lua_tonumber(L, 2);
	sp->Height = (int)h;
	return 0;
}

int base_sprite_get_KeyX(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->KeyX);
	return 1;
}
int base_sprite_get_KeyY(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->KeyY);
	return 1;
}
int base_sprite_get_frame_KeyX(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	int index = (int)luaL_optinteger(L, 2, -1);
	lua_pushinteger(L, sp->GetFrameKeyX(index));
	return 1;
}
int base_sprite_get_frame_KeyY(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	int index = (int)luaL_optinteger(L, 2, -1);
	lua_pushinteger(L, sp->GetFrameKeyY(index));
	return 1;
}
int base_sprite_get_frame_width(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	int index = (int)luaL_optinteger(L, 2, -1);
	lua_pushinteger(L, sp->GetFrameWidth(index));
	return 1;
}
int base_sprite_get_frame_height(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	int index = (int)luaL_optinteger(L, 2, -1);
	lua_pushinteger(L, sp->GetFrameHeight(index));
	return 1;
}

int base_sprite_get_play_time(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushnumber(L, sp->PlayTime);
	return 1;
}

int base_sprite_get_dir_cnt(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->GroupCount);
	return 1;
}
int base_sprite_get_total_frames(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->TotalFrames);
	return 1;
}
int base_sprite_get_current_frame(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->CurrentFrame);
	return 1;
}
int base_sprite_set_current_frame(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	int frame = (int)lua_tointeger(L, 2);
	sp->CurrentFrame = frame;
	return 0;
}

int base_sprite_get_group_frame_count(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->GroupFrameCount);
	return 1;
}

int base_sprite_get_group_frame_time(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushnumber(L, sp->GetGroupFrameTime());
	return 1;
}


int base_sprite_get_group_count(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->GroupCount);
	return 1;
}

int base_sprite_enable_drag(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	bool enable = lua_toboolean(L, 2);
	sp->EnableDrag(enable);
	return 0;
}

int base_sprite_export(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	const char* dir = lua_tostring(L, 2);
	const char* prefix = lua_tostring(L, 3);
	int start = animation->CurrentFrame / animation->GroupFrameCount;
	for (int i = start; i < start + animation->GroupFrameCount; i++) {
		std::string filename(dir);
		filename = filename + prefix + std::to_string(i) + ".tga";
		animation->m_pSprite->SaveImage(filename.c_str(), i);
	}

	return 0;
}


int base_sprite_export_was(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	const char* path = lua_tostring(L, 2);
	RESOURCE_MANAGER_INSTANCE->ExportWas(sp->ResID, path);
	return 0;
}


int base_sprite_destroy(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	delete sp;
	return 0;
}

luaL_Reg MT_BASE_SPRITE[] = {
	{ "Update",base_sprite_update },
{ "Draw",base_sprite_draw },
{ "SetPos", base_sprite_set_pos },
{ "GetPos", base_sprite_get_pos },
{ "SetDir",base_sprite_set_dir },
{ "GetDir",base_sprite_get_dir },
{ "SetFrameInterval",base_sprite_set_frame_interval },
{ "GetFrameInterval",base_sprite_get_frame_interval },
{ "GetWidth", base_sprite_get_width },
{ "GetHeight", base_sprite_get_height },
{ "SetWidth", base_sprite_set_width },
{ "SetHeight", base_sprite_set_height },
{ "GetKeyX", base_sprite_get_KeyX },
{ "GetKeyY", base_sprite_get_KeyY },
{ "GetFrameKeyX", base_sprite_get_frame_KeyX },
{ "GetFrameKeyY", base_sprite_get_frame_KeyY },
{ "GetFrameHeight", base_sprite_get_frame_height },
{ "GetFrameWidth", base_sprite_get_frame_width },
{ "GetPlayTime", base_sprite_get_play_time },
{ "GetDirCnt", base_sprite_get_dir_cnt },
{ "GetTotalFrames", base_sprite_get_total_frames },
{ "GetCurrentFrame", base_sprite_get_current_frame },
{ "SetCurrentFrame", base_sprite_set_current_frame },
{ "GetGroupFrameCount", base_sprite_get_group_frame_count },
{ "GetGroupFrameTime", base_sprite_get_group_frame_time},
{ "GetGroupCount", base_sprite_get_group_count },
{ "EnableDrag", base_sprite_enable_drag },
{ "Export", base_sprite_export },
{ "ExportWas", base_sprite_export_was },
{ "Destroy", base_sprite_destroy },
{ NULL, NULL }
};


int animation_pause(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	int ms = (int)lua_tointeger(L, 2);
	animation->Pause(ms);
	return 0;
}

int animation_stop(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	animation->Stop();
	return 0;
}

int animation_start(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	animation->Play();
	return 0;
}

int animation_lock_frame(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	int frame = (int)luaL_optinteger(L, 2, animation->CurrentFrame);
	animation->LockFrame(frame);
	return 0;
}

int animation_unlock_frame(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	animation->UnLockFrame();
	return 0;
}

int animation_set_loop(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	int loop = (int)lua_tointeger(L, 2);
	int loop_mode = (int)luaL_optinteger(L, 3, ANIMATION_LOOPMODE_RESTART);
	animation->SetLoop(loop, loop_mode);
	return 0;
}

int animation_get_visible(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	lua_pushinteger(L, animation->GetVisible());
	return 1;
}

int animation_set_visible(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	int visible = (int)lua_tointeger(L, 2);
	animation->SetVisible(visible);
	return 0;
}

int animation_translate(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	float dur = (float)lua_tonumber(L, 4);
	Pos tpos(animation->Pos.x + x, animation->Pos.y + y);
	animation->Translate(tpos, (int)(dur * 1000));

	if (lua_isfunction(L, 5)) {
		lua_pushvalue(L, 5);

		int ref = luaL_ref(L, LUA_REGISTRYINDEX);
		animation->AddCallbackLua(dur, ref);
	}
	return 0;
}



int animation_add_loop_callback(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	if (lua_isfunction(L, 2)) {
		lua_pushvalue(L, 2);
		int ref = luaL_ref(L, LUA_REGISTRYINDEX);
		animation->AddLoopCallback(ref);
	}

	return 0;
}


int animation_add_callback(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	float dur = (float)lua_tonumber(L, 2);
	if (lua_isfunction(L, 3)) {
		lua_pushvalue(L, 3);
		//lua_pushvalue(L, 1);
		//lua_setupvalue(L, -2, 1);

		int ref = luaL_ref(L, LUA_REGISTRYINDEX);
		animation->AddCallbackLua(dur, ref);
	}
	return 0;
}

int animation_add_frame_callback(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	int frame = (int)lua_tointeger(L, 2);
	if (lua_isfunction(L, 3)) {
		lua_pushvalue(L, 3);
		//	lua_pushvalue(L, 1);
		//	lua_setupvalue(L, -2, 1);

		int ref = luaL_ref(L, LUA_REGISTRYINDEX);
		animation->AddCallbackLua(animation->FrameInterval * frame, ref);
	}
	return 0;
}

int animation_add_start_callback(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	if (lua_isfunction(L, 2)) {
		lua_pushvalue(L, 2);
		int ref = luaL_ref(L, LUA_REGISTRYINDEX);
		animation->AddStartCallback(ref);
	}
	return 0;
}

int animation_add_update_callback(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	if (lua_isfunction(L, 2)) {
		lua_pushvalue(L, 2);
		int ref = luaL_ref(L, LUA_REGISTRYINDEX);
		animation->AddUpdateCallback(ref);
	}
	return 0;
}

int animation_remove_update_callback(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	animation->RemoveUpdateCallback();
	return 0;
}

int animation_remove_loop_callback(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	animation->RemoveLoopCallback();
	return 0;
}


int animation_get_key_frame(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	lua_pushinteger(L, animation->GetAttackKeyFrame());
	return 1;
}

int animation_reset(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	animation->Reset();
	return 0;
}
int animation_add_stop_callback(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	if (lua_isfunction(L, 2)) {
		lua_pushvalue(L, 2);
		int ref = luaL_ref(L, LUA_REGISTRYINDEX);
		animation->AddStopCallback(ref);
	}
	return 0;
}

int animation_set_offset_x(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	float offx = (float)lua_tonumber(L, 2);
	animation->Offset.x = offx;
	return 0;
}

int animation_set_offset_y(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	float offy = (float)lua_tonumber(L, 2);
	animation->Offset.y = offy;
	return 0;
}

int animation_is_group_end_update(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	lua_pushboolean(L, animation->IsGroupEndUpdate());
	return 1;
}
int animation_is_frame_update(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	lua_pushboolean(L, animation->IsFrameUpdate());
	return 1;
}

int animation_set_rotation(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	float degree = (float)lua_tonumber(L, 2);
	animation->SetRotation(degree);
	return 0;
}


int animation_index(lua_State* L) {
	// 1 = table, 2 = key
	const char* key = lua_tostring(L, 2);
	//cxlog_info("animation_index , %s\n", key);
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, key);
	if (!lua_isnil(L, -1)) {
		//cxlog_info("index type %d %s\n", lua_type(L, -1), key);
		return 1;
	}
	else {
		lua_getuservalue(L, 1);
		lua_getfield(L, -1, key);
		if (!lua_isnil(L, -1)) {
			return 1;
		}
		else {
			lua_pushnil(L);
			return 1;
		}
	}
}

int animation_newindex(lua_State* L) {
	// 1 = table, 2 = key, 3 = value
	lua_getuservalue(L, 1);
	lua_replace(L, 1);
	lua_settable(L, 1);
	return 1;
}


int animation_destroy(lua_State* L) {
	Animation* ptr = lua_check_animation(L, 1);
	delete ptr;
	return 0;
}


luaL_Reg MT_ANIMATION[] = {
{ "Pause",animation_pause },
{ "Stop",animation_stop},
{ "Play",animation_start },
{ "LockFrame",animation_lock_frame },
{ "UnLockFrame",animation_unlock_frame },
{ "SetLoop",animation_set_loop },
{ "GetVisible", animation_get_visible },
{ "SetVisible", animation_set_visible },
{ "Translate", animation_translate },
{ "AddCallback", animation_add_callback},
{ "AddStopCallback", animation_add_stop_callback},
{ "AddFrameCallback", animation_add_frame_callback},
{ "AddStartCallback", animation_add_start_callback},
{ "AddLoopCallback", animation_add_loop_callback},
{ "AddUpdateCallback", animation_add_update_callback},
{ "RemoveUpdateCallback", animation_remove_update_callback},
{ "RemoveLoopCallback", animation_remove_loop_callback},
{ "GetKeyFrame", animation_get_key_frame},
{ "Reset", animation_reset},
{ "SetOffsetX", animation_set_offset_x},
{ "SetOffsetY", animation_set_offset_y},
{ "IsGroupEndUpdate", animation_is_group_end_update},
{ "IsFrameUpdate", animation_is_frame_update},
{ "SetRotation", animation_set_rotation},
//{ "__gc", animation_destroy},
{ "__index", animation_index},
{ "__newindex", animation_newindex},
{ NULL, NULL }
};


luaL_Reg MT_NE_IAMGEVIEW[] = {
	{NULL,NULL}
};

void lua_push_base_sprite(lua_State* L, BaseSprite* sprite)
{
	lua_push_pointer(L, sprite);
	if (luaL_newmetatable(L, "MT_BASE_SPRITE")) {
		luaL_setfuncs(L, MT_BASE_SPRITE, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}

void lua_push_animation(lua_State* L, Animation* sprite)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, sprite->GetLuaRef());
	if (luaL_newmetatable(L, "MT_ANIMATION")) {
		luaL_setfuncs(L, MT_BASE_SPRITE, 0);
		luaL_setfuncs(L, MT_ANIMATION, 0);
	}
	lua_setmetatable(L, -2);
}


int animation_create(lua_State* L)
{
	int argn = lua_gettop(L);
	if (argn == 2) {
		uint32_t pack = (uint32_t)lua_tointeger(L, 1);
		uint32_t wasid = (uint32_t)lua_tointeger(L, 2);
		lua_push_animation(L, new Animation(pack, wasid));
	}
	else if (argn == 1) {
		uint64_t resid = (uint64_t)lua_tointeger(L, 1);
		lua_push_animation(L, new Animation(resid));
	}
	else {
		return 0;
	}

	return 1;
}



int base_sprite_get_metatable(lua_State* L) {
	if (luaL_newmetatable(L, "MT_BASE_SPRITE")) {
		luaL_setfuncs(L, MT_BASE_SPRITE, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	return 1;
}

int beat_number_update(lua_State* L) {
	auto* bn = lua_check_pointer<BeatNumber>(L, 1);
	bn->Update();
	return 0;
}

int beat_number_draw(lua_State* L) {
	auto* bn = lua_check_pointer<BeatNumber>(L, 1);
	bn->Draw();
	return 0;
}

int beat_number_beat(lua_State* L) {
	auto* bn = lua_check_pointer<BeatNumber>(L, 1);
	bn->Beat();
	return 0;
}

int beat_number_set_pos(lua_State* L) {
	auto* bn = lua_check_pointer<BeatNumber>(L, 1);
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	bn->SetPos(x, y);
	return 0;
}

int beat_number_set_number(lua_State* L) {
	auto* bn = lua_check_pointer<BeatNumber>(L, 1);
	float  num = (float)lua_tonumber(L, 2);
	bn->SetNumber(num);
	return 0;
}


luaL_Reg MT_BEAT_NUMBER[] = {
	{ "Update",beat_number_update },
	{ "Draw",beat_number_draw },
	{ "Beat",beat_number_beat },
	{ "SetPos",beat_number_set_pos},
	{ "SetNumber",beat_number_set_number },
};

int beat_number_create(lua_State* L)
{
	lua_push_pointer(L, new BeatNumber());
	if (luaL_newmetatable(L, "MT_BEAT_NUMBER")) {
		luaL_setfuncs(L, MT_BEAT_NUMBER, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
	return 1;
}

void animation_manager_update()
{
	AnimationManager::GetInstance()->Update();
}

void animation_manager_draw()
{
	AnimationManager::GetInstance()->Draw();
}

void animation_manager_clear()
{
	AnimationManager::GetInstance()->Clear();
}

void luaopen_sprite(lua_State* L)
{
	script_system_register_luac_function(L, animation_create);
	script_system_register_luac_function(L, animation_destroy);

	script_system_register_luac_function(L, base_sprite_get_metatable);
	script_system_register_luac_function(L, beat_number_create);

	script_system_register_function(L, animation_manager_update);
	script_system_register_function(L, animation_manager_draw);
	script_system_register_function(L, animation_manager_clear);

}

