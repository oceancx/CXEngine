#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <sstream>
#include <memory>
#include <nlohmann/json.hpp>
#include <unordered_map>
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
using Vec2 = glm::vec2;

using Vec3i = glm::tvec3<int>;
using Vec4i = glm::tvec4<int>;
using Vec2i = glm::tvec2<int>;

using Mat3 = glm::mat3;
using Mat4 = glm::mat4;
using Mat2 = glm::mat2;

using String = std::string;
using CXString = std::string;
using Json = nlohmann::json;
using std::vector;
using std::string;
using std::shared_ptr;
using std::map;
using std::unordered_map;
using std::deque;
using std::list;
using std::array;
using std::make_shared;
using std::function;
typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;

typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

typedef float float32;
typedef double float64;
typedef long double float128;

typedef uint8_t byte;

#ifndef TGA_FILE_HEADER_H
#define TGA_FILE_HEADER_H
#pragma pack(push)
#pragma pack(1)
struct TGA_FILE_HEADER
{
	uint8 IdLength;
	uint8 ColorMapType;
	uint8 ImageType;
	uint16 ColorMapFirstIndex;
	uint16 ColorMapLength;
	uint8 ColorMapEntrySize;
	uint16 XOrigin;
	uint16 YOrigin;
	uint16 ImageWidth;
	uint16 ImageHeight;
	uint8 PixelDepth;
	uint8 ImageDescruptor;
};
#pragma pack(pop)
#endif

#ifdef _T
#undef _T
#endif

#define _T(x) std::string(x)



class Direction
{
public:
	enum Dir
	{
		N = 6,
		S = 4,
		W = 5,
		E = 7,
		N_E = 3,
		N_W = 2,
		S_E = 0,
		S_W = 1,
	};
};

enum EDir
{
	DIR_N = 6,
	DIR_S = 4,
	DIR_W = 5,
	DIR_E = 7,
	DIR_NE = 3,
	DIR_NW = 2,
	DIR_SE = 0,
	DIR_SW = 1,
};
enum ActionEnums
{
	ACTION_IDLE = 0,
	ACTION_WALK,
	ACTION_SIT,
	ACTION_ANGRY,
	ACTION_SAYHI,
	ACTION_DANCE,
	ACTION_SALUTE,
	ACTION_CLPS,
	ACTION_CRY,
	ACTION_BATIDLE,
	ACTION_ATTACK,
	ACTION_CAST,
	ACTION_BEHIT,
	ACTION_RUNTO,
	ACTION_RUNBACK,
	ACTION_DEFEND,
	ACTION_COUNT
};

enum EASMActionType
{
	ASM_ACTION,
	ASM_ATTACK_ACTION,
	ASM_CAST_ACTION,
	ASM_BEHIT_ACTION,
	ASM_BECAST_ACTION,
	ASM_DEAD_FLY_ACTION,
	ASM_PATH_MOVE_ACTION
};

enum EActorType
{
	ACTOR_TYPE_DEFAULT = 0,
	ACTOR_TYPE_PLAYER,
	ACTOR_TYPE_SUMMON,
	ACTOR_TYPE_NPC
};

enum EWDFFilePack
{
	ADDONWDF = 0,
	ATOMWDF,
	CHATWDF,
	FIREWORKWDF,
	GOODSWDF,
	ITEMWDF,
	MAGICWDF,
	MAPANIWDF,
	MHIMAGEWDF,
	MISCWDF,
	MUSICWDF,
	SCENEWDF,
	SHAPEWDF,
	SHAPEWD1,
	SHAPEWD2,
	SHAPEWD3,
	SHAPEWD4,
	SHAPEWD5,
	SHAPEWD6,
	SHAPEWD7,
	SMAPWDF,
	SOUNDWDF,
	STOCKWDF,
	WADDONWDF,
	WZIFEWDF,
	WZIFEWD1
};

enum EWeaponTypes
{
	WEAPON_SPEAR = 0,
	WEAPON_AXE,
	WEAPON_SWORD,
	WEAPON_DBSWORDS,
	WEAPON_RIBBON,
	WEAPON_CLAW,
	WEAPON_FAN,
	WEAPON_WAND,
	WEAPON_HAMMER,
	WEAPON_WHIP,
	WEAPON_RING,
	WEAPON_KNIFE,
	WEAPON_STAFF,
	WEAPON_JEWEL,
	WEAPON_BOW
};

enum ERoleTypes
{
	ROLE_FYN,
	ROLE_YNX,
	ROLE_XYS,
	ROLE_JXK,
	ROLE_HMR,
	ROLE_GJL,
	ROLE_JMW,
	ROLE_HTG,
	ROLE_WTJ,
	ROLE_XCE,
	ROLE_STB,
	ROLE_LTZ
};