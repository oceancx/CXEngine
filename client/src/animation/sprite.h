
#pragma once
#include "ui.h"
#include <functional>

struct NE::Sprite;
struct NE::Sprite::Sequence;
using CXPos = Pos;
using NE::PalSchemePart;
struct PalSpriteInfo;
class BaseSprite : public View
{
public:
	BaseSprite(uint64_t resoureID = 0, std::vector<PalSchemePart>* patMatrix = nullptr);
	BaseSprite(uint32_t pkg, uint32_t wasID, std::vector<PalSchemePart>* patMatrix = nullptr);
	virtual ~BaseSprite();

	virtual void Update() {};
	virtual void Draw() {};
	
	NE::Sprite::Sequence* GetFrame(int index = -1);
	int GetFrameKeyX(int index = -1);
	int GetFrameKeyY(int index = -1);
	int GetFrameWidth(int index = -1);
	int GetFrameHeight(int index = -1);

	void EnableDrag(bool enable);
	float GetGroupFrameTime();

	int TotalFrames;
	int CurrentFrame;
	int GroupFrameCount;
	int GroupCount;
	int Width;
	int Height;
	int KeyX;
	int KeyY;
	int Dir;
	uint64_t ResID;
	CXPos Pos;
	CXPos Offset;
	PalSpriteInfo* spritInfo;
	NE::Sprite* m_pSprite;
	std::vector<PalSchemePart>* m_PalPart;
	float FrameInterval;
	float PlayTime;
	
	bool bEnableDrag;
	Bound GetViewBounds() override;
	bool CheckDrag(int x, int y) override;
	void OnDragMove(int x, int y)override;

};



enum EAnimationState
{
	ANIMATION_PLAY = 0,
	ANIMATION_STOP,
	ANIMATION_PAUSE
};

enum EAnimationCBFuncType
{
	ANIMATION_CBFUNC_C = 0,
	ANIMATION_CBFUNC_LUA
};
enum EAnimationLoopMode
{
	ANIMATION_LOOPMODE_RESTART=0,
	ANIMATION_LOOPMODE_STOPFIX,
	ANIMATION_LOOPMODE_CHANGE_DIR,
};

class Animation : public BaseSprite
{
public:
	

	struct CBDataLua{
		float dur;
		int func;
		bool markd;
		CBDataLua(float d,int f){
			dur = d;
			func = f;
			markd = false;
		}
	};

	struct CBData{
		float dur;
		function<void()> func;
		bool markd;
		CBData(float d,function<void()> f){
			dur = d;
			func = f;
			markd = false;
		}
	};

	
	Animation(uint64_t resoureID = 0, std::vector<PalSchemePart>* patMatrix = nullptr);
	Animation(uint32_t pkg, uint32_t wasID, std::vector<PalSchemePart>* patMatrix = nullptr);
	virtual ~Animation() override;
	
	Animation** GetUserData() { return m_UserData; };
	
	void Update() override;
	void Draw() override;
	void SetVisible(bool visible) { m_Visible = visible; };
	bool GetVisible() { return m_Visible; };
	void SetLuaRef(int ref);
	int GetLuaRef() { return m_LuaRef; };

	void Pause(int ms);
	void SetLoop(int loop,int mode = ANIMATION_LOOPMODE_RESTART);
	void Reset();
	void Stop();
	void Play();
	void Replay();
	
	void AddCallbackLua(float dur,int funcRef);
	void AddCallback(float dur, function<void()> func);
	void AddFrameCallback(int frame, std::function<void()> callback);
	void RemoveFrameCallback(int frame);

	void SetFrameInterval(float interval) { FrameInterval = interval; };
	float GetFrameInterval() { return FrameInterval; };

	void SetDir(int dir) { Dir = dir; };
	int GetDir() { return Dir; };
	bool IsGroupEndUpdate() { return m_bGroupEndUpdate; };
	bool IsFrameUpdate() { return m_bFrameUpdate; };
	int GetAttackKeyFrame() { return AttackKeyFrame; };
	
	void TranslateTo(CXPos pos, int duration);
	void Translate(CXPos pos, int duration);
	void LockFrame(int frame);
	void UnLockFrame();
	int GetState() { return m_State; };

	void AddStopCallback(int funcRef);
	void AddStartCallback(int funcRef);
	void AddLoopCallback(int funcRef);
	void AddUpdateCallback(int funcRef);
	void RemoveLoopCallback();
	void RemoveUpdateCallback();

	void CallLuaFunByRef(int& ref);
	void SetRotation(float rotate) { m_Rotation = rotate; };
private:
	float m_Rotation;
	bool m_bGroupEndUpdate;
	bool m_bFrameUpdate;
	int m_LoopCount;
	int m_LoopCounter;
	int AttackKeyFrame;
	int bPlay;
	int m_PreviousState;
	int m_State;
	bool m_Visible;
	int m_PauseTime;
	int m_StopCBRef;
	int m_StartCBRef;
	int m_LoopCBRef;
	int m_UpdateCBRef;
	int m_LoopMode;
	map<int, std::function<void()>> m_Callbacks;
	
	deque<CBDataLua> m_CallbackQueueLua;
	deque<CBData> m_CallbackQueue;
	
	int m_LockFrame;
	bool m_bLockFrame;
	bool m_bTranslate;
	bool m_bTranslatePos;
	CXPos m_TranslationPos;
	CXPos m_TranslationToPos;
	float m_Duration;
	CXPos m_Velocity;
	bool m_bLoop;
	int m_LuaRef;
	Animation** m_UserData;
};



class BeatNumber 
{
public:
	struct Digit
	{
		int8_t digit;
		float y;
	};
	BeatNumber();
	virtual ~BeatNumber() { }
	void Update();
	void Draw();
	void SetPos(float x, float y);
	void SetNumber(float num);
	void Beat();
	void SetVisible(bool visible) { m_Visible = visible; };
	bool GetVisible() { return m_Visible; };
	void AddPlayCallback(int ms, std::function<void()> cb) { m_Callbacks[ms] = cb; };
	
private:
	std::map<int, std::function<void()>> m_Callbacks;
	bool m_bBeat;
	bool m_Visible;
	float m_PlayTime;
	float m_BeatTime;
	float m_PauseTime;
	float m_BeatHeights;
	float m_AdvanceX;
	int m_Number;
	CXPos m_Pos;
	std::deque<Digit>m_Digits;
	Animation m_HealAnim;
	Animation m_HitAnim;
	bool m_ShowHeal;
};

class AnimationManager :public Singleton<AnimationManager>
{
public:
	AnimationManager() ;
	~AnimationManager() ;
	void AddAnimation(Animation* animation);
	void AddBeatNumber(BeatNumber* bn);
	void Update();
	void Draw();
	void Clear();
private:
	deque<Animation*> m_Animations;
	deque<BeatNumber*> m_BeatNumbers;
	
};

void lua_push_base_sprite(lua_State* L, BaseSprite* sprite);
void lua_push_animation(lua_State*L, Animation* sprite);

Animation* lua_check_animation(lua_State* L, int index);

void luaopen_sprite(lua_State* L);