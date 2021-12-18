#pragma once
#include <sstream>





struct MocapQuaternion
{
public:
	float x;
	float y;
	float z;
	float w;
};

struct MocapVector3
{
public:
	float x;
	float y;
	float z;
};


class BonePose
{
public:
	MocapVector3 Position;
	MocapQuaternion Orientation;
};

class RecordBonePose
{
public:
	int32_t boneId;
	BonePose* pose;
};

class RecordBlendshape
{
public:
	std::string name;
	float val;
};


class RecordFrame
{
public:
	int frameIndex;
	int objectId;
	uint64_t frameTime;
	int poseCount;
	std::vector< RecordBonePose* > poses;
	int blendshapeCount;
	std::vector< RecordBlendshape*> blendshapes;
	float frameTimeSec;
};

class BoneDefinition
{
public:
	int32_t Id;
	int32_t ParentId;
	std::string Name;
	MocapVector3 Offset;
};

class SkeletonDefinition
{
public:
	int32_t Id;
	std::string Name;
	std::vector< BoneDefinition*> Bones;
};

class RecordObject
{
public:
	int index;
	std::string name;

	//-----V0.4版本添加---------------------------------------------
	int modelID;
	float* posOffset;
	float* rotOffset;
	//-----V0.4版本添加---------------------------------------------

	SkeletonDefinition* skeletonDef;
};

class RecSceneInfo
{
public:
	string name;
};

class RecCameraInfo
{
public:
	float* pos;
	float* rot;
	float fov;
};

class RecMediaInfo
{
public:
	string name = "";
	float totalTime;
	float st;
	float et;
};

class RecordHeader
{
public:
	float version;
	std::string name;
	int frameCount;
	int64_t startTime;
	int64_t endTime;

	//------------------V05添加------------------
	float totalTime;
	int hasSceneInfo;
	RecSceneInfo* sceneInfo;
	int hasCamera;
	RecCameraInfo* cameraInfo;
	int hasMedia;
	RecMediaInfo* mediaInfo;
	//------------------V05添加------------------

	int objectCount;
	std::vector<RecordObject*> recordObjects;
};

class RecordFaceClientFrame
{
public:
	int64_t sendtime;
	int64_t recvtime;
	int blendshapeCount;
	std::vector<RecordBlendshape*> blendshapes;
};


void luaopen_recorder(lua_State* L);