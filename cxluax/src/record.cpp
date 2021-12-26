
#include "record.h"
#include <sstream>


class RecordFileVer
{
public:
	static float V0_1;
	static float V0_2;
	static float V0_3;
	static float V0_4;
	static float V0_5;
};


float RecordFileVer::V0_1 = 0.1f;
float RecordFileVer::V0_2 = 0.2f;
float RecordFileVer::V0_3 = 0.3f;
float RecordFileVer::V0_4 = 0.4f;
float RecordFileVer::V0_5 = 0.5f;


const int64_t TicksPerDay = 864000000000;
const int64_t TicksPerHour = 36000000000;
const int64_t TicksPerMillisecond = 10000;
const int64_t TicksPerMinute = 600000000;
const int64_t TicksPerSecond = 10000000;

unsigned __int64 DTFromCDull = 0x08ccc9b45a610000;  //Tuesday, May 25, 2010 12:00:00 AM
unsigned __int64 UnixEpoch = 0x089f7ff5f7b58000;  //Thursday, January 01, 1970 12:00:00 AM


void readString(std::ifstream& fin, std::string& strout) {
	int strlen = 0;
	fin.read((char*)&strlen, 4);
	if (strlen == 0) {
		strout = "";
		return;
	}
	char* name = new char[strlen + 2];
	fin.read((char*)name, (strlen + 1));
	name[strlen + 1] = '\0';
	strout = (name + 1);
	delete[] name;
}


std::vector<std::string> BLENDSHAPE_NAMES = {
	"browDownLeft", "browDownRight", "browInnerUp", "browOuterUpLeft",
	"browOuterUpRight", "cheekPuff", "cheekSquintLeft", "cheekSquintRight",
	"eyeBlinkLeft", "eyeBlinkRight", "eyeLookDownLeft", "eyeLookDownRight",
	"eyeLookInLeft", "eyeLookInRight", "eyeLookOutLeft", "eyeLookOutRight",
	"eyeLookUpLeft", "eyeLookUpRight", "eyeSquintLeft", "eyeSquintRight",
	"eyeWideLeft", "eyeWideRight", "jawForward", "jawLeft", "jawOpen",
	"jawRight", "mouthClose", "mouthDimpleLeft", "mouthDimpleRight",
	"mouthFrownLeft", "mouthFrownRight", "mouthFunnel", "mouthLeft",
	"mouthLowerDownLeft", "mouthLowerDownRight", "mouthPressLeft",
	"mouthPressRight", "mouthPucker", "mouthRight", "mouthRollLower",
	"mouthRollUpper", "mouthShrugLower", "mouthShrugUpper", "mouthSmileLeft",
	"mouthSmileRight", "mouthStretchLeft", "mouthStretchRight",
	"mouthUpperUpLeft", "mouthUpperUpRight", "noseSneerLeft", "noseSneerRight",
	"tongueOut"
};

void LoadRecordFile(const char* path, RecordHeader* header, std::map<int, std::vector<RecordFrame*>>& objectsFrames)
{
	std::ifstream fin(path, std::ios::binary | std::ios::in);

	fin.read((char*)&header->version, 4);
	readString(fin, header->name);
	printf("%s\n", header->name.c_str());

	fin.read((char*)&header->frameCount, 4);
	fin.read((char*)&header->startTime, 8);
	fin.read((char*)&header->endTime, 8);
	fin.read((char*)&header->objectCount, 4);

	if (header->objectCount > 0) {
		for (int i = 0; i < header->objectCount; i++) {
			RecordObject* recobj = new RecordObject();
			header->recordObjects.push_back(recobj);
			fin.read((char*)&recobj->index, 4);
			readString(fin, recobj->name);

			recobj->skeletonDef = new SkeletonDefinition();
			auto* skelDef = recobj->skeletonDef;

			fin.read((char*)&skelDef->Id, 4);
			readString(fin, skelDef->Name);

			int boneCount = 0;
			fin.read((char*)&boneCount, 4);
			if (boneCount > 0) {
				for (int bi = 0; bi < boneCount; bi++) {
					BoneDefinition* boneDef = new BoneDefinition();
					skelDef->Bones.push_back(boneDef);

					fin.read((char*)&boneDef->Id, 4);
					fin.read((char*)&boneDef->ParentId, 4);

					readString(fin, boneDef->Name);

					fin.read((char*)&boneDef->Offset.x, 4);
					fin.read((char*)&boneDef->Offset.y, 4);
					fin.read((char*)&boneDef->Offset.z, 4);
				}
			}
		}
	}

	if (header->frameCount > 0) {
		for (int i = 0; i < header->frameCount; i++) {
			for (int oi = 0; oi < header->objectCount; oi++)
			{
				RecordObject* obj = header->recordObjects[oi];
				RecordFrame* frame = new RecordFrame();
				objectsFrames[obj->index].push_back(frame);

				fin.read((char*)&frame->frameIndex, 4);
				fin.read((char*)&frame->objectId, 4);
				if (header->version > 0.1f) {
					fin.read((char*)&frame->frameTime, 8);
				}
				fin.read((char*)&frame->poseCount, 4);
				if (frame->poseCount > 0) {
					for (int pi = 0; pi < frame->poseCount; pi++) {
						RecordBonePose* pose = new RecordBonePose();
						fin.read((char*)&pose->boneId, 4);
						BonePose* bp = new BonePose();
						pose->pose = bp;

						fin.read((char*)&bp->Position.x, 4);
						fin.read((char*)&bp->Position.y, 4);
						fin.read((char*)&bp->Position.z, 4);

						fin.read((char*)&bp->Orientation.x, 4);
						fin.read((char*)&bp->Orientation.y, 4);
						fin.read((char*)&bp->Orientation.z, 4);
						fin.read((char*)&bp->Orientation.w, 4);

						frame->poses.push_back(pose);
					}
				}
				fin.read((char*)&frame->blendshapeCount, 4);
				if (frame->blendshapeCount > 0) {
					for (int bs = 0; bs < frame->blendshapeCount; bs++) {
						RecordBlendshape* rbs = new RecordBlendshape();
						readString(fin, rbs->name);
						fin.read((char*)&rbs->val, 4);
						frame->blendshapes.push_back(rbs);
					}
				}
			}
		}
	}
	printf("read object!\n");
}

void LoadFaceRecordFile(const char* path, RecordHeader* header, std::vector<RecordFaceClientFrame*>& faceFrames)
{
	std::ifstream fin(path, std::ios::binary | std::ios::in);
	fin.read((char*)&header->version, 4);
	readString(fin, header->name);
	printf("%s\n", header->name.c_str());

	fin.read((char*)&header->frameCount, 4);
	fin.read((char*)&header->startTime, 8);
	fin.read((char*)&header->endTime, 8);
	fin.read((char*)&header->objectCount, 4);

	printf("read header done!\n");
	assert(header->objectCount == 0);

	if (header->frameCount > 0) {
		for (int i = 0; i < header->frameCount; i++) {
			RecordFaceClientFrame* faceFrame = new RecordFaceClientFrame();
			fin.read((char*)&faceFrame->sendtime, 8);
			fin.read((char*)&faceFrame->recvtime, 8);
			fin.read((char*)&faceFrame->blendshapeCount, 4);
			if (faceFrame->blendshapeCount > 0) {
				for (int bi = 0; bi < faceFrame->blendshapeCount; bi++) {
					RecordBlendshape* shape = new RecordBlendshape();
					readString(fin, shape->name);
					fin.read((char*)&shape->val, 4);
					faceFrame->blendshapes.push_back(shape);
				}
			}
			faceFrames.push_back(faceFrame);
		}
	}
	printf("read object done!\n");
}


class utils
{
public:
	static float toSec(long ticks) {
		return ticks * 1.0f / TicksPerSecond;
	}
};

CXString utils_record_get_timestr(int64_t csharpTicks) {
	std::time_t t = (time_t)((csharpTicks - UnixEpoch) / 10000000);
	std::tm* now = gmtime(&t);
	std::stringstream ss;
	ss << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' << now->tm_mday << "-" << now->tm_hour << "-" << now->tm_min << "-" << now->tm_sec;
	return  ss.str();
}



int read_record_file_header(lua_State* L) {
	const char* path = lua_tostring(L, 1);
	std::ifstream fin;
	fin.exceptions(std::ios::failbit | std::ios::badbit);
	fin.open(path, std::ios::binary | std::ios::in);

	RecordHeader* header = new RecordHeader();

	fin.read((char*)&header->version, 4);
	readString(fin, header->name);
	printf("%s\n", header->name.c_str());

	fin.read((char*)&header->frameCount, 4);
	fin.read((char*)&header->startTime, 8);
	fin.read((char*)&header->endTime, 8);
	if (header->version >= RecordFileVer::V0_5) {
		fin.read((char*)&header->totalTime, 4);
		fin.read((char*)&header->hasSceneInfo, 4);
		if (header->hasSceneInfo != 0) {
			header->sceneInfo = new RecSceneInfo();
			readString(fin, header->sceneInfo->name);
		}
		fin.read((char*)&header->hasCamera, 4);
		if (header->hasCamera != 0) {
			int len = 3 * 4 + 3 * 4 + 4;
			char* buf = new char[len];
			fin.read(buf, len);
			delete[] buf;
			//fin.seekg(len, std::ios_base::cur);
		}
		fin.read((char*)&header->hasMedia, 4);
		if (header->hasMedia != 0) {

			std::string tmp_s;
			readString(fin, tmp_s);
			int len = 3 * 4;
			//fin.seekg(len, std::ios_base::cur);

			char* buf = new char[len];
			fin.read(buf, len);
			delete[] buf;
		}

	}
	else {
		header->totalTime = utils::toSec(header->endTime - header->startTime);
	}
	fin.read((char*)&header->objectCount, 4);


	std::string outstr = "";
	std::stringstream ss;
	ss << "header->version:\t" << header->version << std::endl;
	ss << "header->name:\t" << header->name << std::endl;
	ss << "header->frameCount:\t" << header->frameCount << std::endl;
	ss << "header->startTime:\t" << utils_record_get_timestr(header->startTime) << std::endl;
	ss << "header->endTime:\t" << utils_record_get_timestr(header->endTime) << std::endl;
	ss << "header->objectCount:\t" << header->objectCount << std::endl;
	ss << "=========================================================================" << std::endl;

	if (header->objectCount > 0) {
		for (int i = 0; i < header->objectCount; i++) {
			RecordObject* recobj = new RecordObject();
			header->recordObjects.push_back(recobj);
			fin.read((char*)&recobj->index, 4);
			readString(fin, recobj->name);


			if (header->version >= RecordFileVer::V0_4) {
				fin.read((char*)&recobj->modelID, 4);
				recobj->posOffset = new float[3];
				fin.read((char*)&recobj->posOffset[0], 4);
				fin.read((char*)&recobj->posOffset[1], 4);
				fin.read((char*)&recobj->posOffset[2], 4);


				recobj->rotOffset = new float[3];
				fin.read((char*)&recobj->rotOffset[0], 4);
				fin.read((char*)&recobj->rotOffset[1], 4);
				fin.read((char*)&recobj->rotOffset[2], 4);
			}

			ss << "recobj->index:" << recobj->index << std::endl;
			ss << "recobj->name:" << recobj->name << std::endl;

			recobj->skeletonDef = new SkeletonDefinition();
			SkeletonDefinition* skelDef = recobj->skeletonDef;

			fin.read((char*)&skelDef->Id, 4);
			readString(fin, skelDef->Name);
			int boneCount = 0;
			fin.read((char*)&boneCount, 4);

			ss << "skelDef->id:" << skelDef->Id << std::endl;
			ss << "skelDef->name:" << skelDef->Name << std::endl;
			ss << "boneCount:" << boneCount << std::endl;

			if (boneCount > 0) {
				for (int bi = 0; bi < boneCount; bi++) {
					BoneDefinition* boneDef = new BoneDefinition(); ;

					fin.read((char*)&boneDef->Id, 4);
					fin.read((char*)&boneDef->ParentId, 4);

					readString(fin, boneDef->Name);

					fin.read((char*)&boneDef->Offset.x, 4);
					fin.read((char*)&boneDef->Offset.y, 4);
					fin.read((char*)&boneDef->Offset.z, 4);

					skelDef->Bones.push_back(boneDef);

					ss << "\tbone Id:\t" << boneDef->Id << "\tParentId:" << boneDef->ParentId << "\tName:" << boneDef->Name << std::endl;
					ss << "\tbone Offset:\t" << boneDef->Offset.x << "\t" << boneDef->Offset.y << "\t" << boneDef->Offset.z << std::endl;
				}
			}
		}
	}

	ss << "=========================================================================" << std::endl;
	fin.close();

	outstr = ss.str();

	lua_pushlstring(L, outstr.c_str(), outstr.size());
	lua_newtable(L);
	for (int i = 0; i < header->objectCount; i++) {
		auto* obj = header->recordObjects[i];

		lua_newtable(L);
		lua_pushinteger(L, obj->index);
		lua_setfield(L, -2, "index");

		lua_pushstring(L, obj->name.c_str());
		lua_setfield(L, -2, "name");

		lua_pushinteger(L, obj->modelID);
		lua_setfield(L, -2, "modelID");

		lua_seti(L, -2, i + 1);
	}
	delete header;
	header = nullptr;
	return 2;
}

void luaopen_recorder(lua_State* L) {
	script_system_register_luac_function(L, read_record_file_header);
}