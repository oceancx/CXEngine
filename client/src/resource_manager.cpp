#include "resource_manager.h"
#include "graphics/image.h"
#include "file_system.h"
#include "profile.h"
#include "file_loading.h"
#include <string>
#include "logger.h"
#include "tsv.h"
#include "actor/action.h"
#include "texture_manager.h"
#include "game.h"
#include <graphics/bitmap.h>
#include <utils.h>
#include <stb_image.h>
static std::map<uint32_t, std::string> s_PackPathMap =
{
	{ ADDONWDF,    "addon.wdf" },
{ ATOMWDF,     "atom.wdf" },
{ CHATWDF,     "chat.wdf" },
{ FIREWORKWDF, "firework.wdf" },
{ GOODSWDF,    "goods.wdf" },
{ ITEMWDF,     "item.wdf" },
{ MAGICWDF,    "magic.wdf" },
{ MAPANIWDF,   "mapani.wdf" },
{ MHIMAGEWDF,  "mhimage.wdf" },
{ MISCWDF,     "misc.wdf" },
{ MUSICWDF,    "music.wdf" },
{ SCENEWDF,    "scene.wdf" },
{ SHAPEWDF,    "shape.wdf" },
{ SHAPEWD1,    "shape.wd1" },
{ SHAPEWD2,    "shape.wd2" },
{ SHAPEWD3,    "shape.wd3" },
{ SHAPEWD4,    "shape.wd4" },
{ SHAPEWD5,    "shape.wd5" },
{ SHAPEWD6,    "shape.wd6" },
{ SHAPEWD7,    "shape.wd7" },
{ SMAPWDF,     "smap.wdf" },
{ SOUNDWDF,    "sound.wdf" },
{ STOCKWDF,    "stock.wdf" },
{ WADDONWDF,   "waddon.wdf" },
{ WZIFEWDF,    "wzife.wdf" },
{ WZIFEWD1,    "wzife.wd1" }
};


static std::vector<String> s_PackToName = { "addon.wdf","atom.wdf","chat.wdf","firework.wdf","goods.wdf","item.wdf","magic.wdf","mapani.wdf","mhimage.wdf","misc.wdf","music.wdf","scene.wdf","shape.wd1","shape.wd2","shape.wd3","shape.wd4","shape.wd5","shape.wd6","shape.wd7","shape.wdf","smap.wdf","sound.wdf","stock.wdf","waddon.wdf","wzife.wd1","wzife.wdf","wzimage.wdf" };
static std::map<uint32_t, NE::WDF*> s_Loaders;
namespace utils
{
	std::string GetPathByPackID(uint32 packID)
	{
		auto it = s_PackPathMap.find(packID);
		if (it != s_PackPathMap.end())
		{
			return FileSystem::GetWDFPath(it->second);
		}
		else
		{
			printf("GetPathByPackID error!\n");
			return "";
		}
	}

}

bool utils_is_pal_scheme_part_equal(std::vector<PalSchemePart> pat1, std::vector<PalSchemePart> pat2)
{
	size_t sz1 = pat1.size() * sizeof(PalSchemePart);
	size_t sz2 = pat2.size() * sizeof(PalSchemePart);
	if (sz1 != sz2)return false;
	return memcmp(pat1.data(), pat2.data(), sz1) == 0;
}


ResourceManager::ResourceManager()
	:Singleton<ResourceManager>()
{

}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::Clear()
{

}

Sprite* ResourceManager::LoadWASSpriteByID(uint64_t resID, bool sync, std::vector<PalSchemePart>* patMatrix)
{
	if (!resID)return nullptr;
	std::string path(std::to_string(resID));
	uint32_t pack = 0;
	uint32_t wasID = 0;
	DecodeWAS(resID, pack, wasID);

	auto* iothread = file_loading_thread();
	if (iothread->IsFileLoadOk(path.c_str()))
	{
		return s_Loaders[pack]->LoadSprite(wasID);
	}
	else
	{
		if (sync)
		{
			if (s_Loaders.find(pack) == s_Loaders.end())
			{
				s_Loaders[pack] = new NE::WDF(utils::GetPathByPackID(pack));
			}
			return s_Loaders[pack]->LoadSprite(wasID, patMatrix);
		}
		else
		{
			iothread->PostTask(path.c_str(), [this, pack, wasID, patMatrix](const char* path)->bool
				{
					if (s_Loaders.find(pack) == s_Loaders.end())
					{
						s_Loaders[pack] = new NE::WDF(utils::GetPathByPackID(pack));
					}
					s_Loaders[pack]->LoadSprite(wasID, patMatrix);
					return true;
				});
			return nullptr;
		}
	}
}

void ResourceManager::UnLoadWASSpriteByID(uint64_t resID)
{
	uint32_t pack;
	uint32_t wasID;
	DecodeWAS(resID, pack, wasID);
	if (s_Loaders.find(pack) == s_Loaders.end())
	{
		return;
	}
	else
	{
		s_Loaders[pack]->UnLoadSprite(wasID);
		return;
	}
}


Sprite* ResourceManager::LoadWASSprite(uint32_t pack, uint32 wasID, bool sync, std::vector<PalSchemePart>* patMatrix)
{
	auto resID = EncodeWAS(pack, wasID);
	return LoadWASSpriteByID(resID, sync, patMatrix);
}

void ResourceManager::UnLoadWASSprite(uint32_t pack, uint32 wasID)
{
	auto resID = EncodeWAS(pack, wasID);
	return UnLoadWASSpriteByID(resID);
}


void ResourceManager::OnUpdate()
{

}

uint64_t ResourceManager::GetActionResID(int type, CXString id, int action)
{
	lua_State* L = script_system_get_luastate();
	lua_getglobal(L, "utils_get_action_res_id");
	lua_pushinteger(L, type);
	lua_pushstring(L, id.c_str());
	lua_pushinteger(L, action);
	int res = lua_pcall(L, 3, 1, 0);
	check_lua_error(L, res);
	uint64_t res_id = (uint64_t)lua_tointeger(L, -1);
	return res_id;
}




uint64_t ResourceManager::GetActorActionResID(int actorType, CXString roleID, int actionID)
{
	return GetActionResID(actorType, roleID, actionID);
}

uint64_t ResourceManager::GetWeaponActionResID(CXString id, int actionID)
{
	return GetActionResID(AVATAR_TYPE_WEAPON, id, actionID);
}
 
void ResourceManager::ExportWas(uint64_t id, CXString path)
{
	uint32_t pack, wasid;
	DecodeWAS(id, pack, wasid);
	if (s_Loaders.find(pack) == s_Loaders.end())
	{
		s_Loaders[pack] = new NE::WDF(utils::GetPathByPackID(pack));
	} 
	Sprite* sprite = s_Loaders[pack]->UnpackSprite(wasid, {});
	if (sprite != nullptr) {
		nlohmann::json savejo = nlohmann::json::object();
		savejo["key_x"] = sprite->KeyX;
		savejo["key_y"] = sprite->KeyY;
		savejo["width"] = sprite->Width;
		savejo["height"] = sprite->Height;
		savejo["group"] = sprite->GroupCount;
		savejo["children"] = nlohmann::json::array();
		for (int g = 0; g < sprite->GroupCount; g++) {
			for (int f = 0; f < sprite->GroupFrameCount; f++) {
				int i = g * sprite->GroupFrameCount + f;
				auto& frame = sprite->Frames[i];
				char save_path[512];
				sprintf_s(save_path, "%s/%d_%d.bmp", path.c_str(), g, f);
				char name[64];
				sprintf_s(name, "%d_%d.bmp", g, f);
				//printf("export %s \n ", save_path);
				Bitmap::CreateBitmap(save_path, frame.Width, frame.Height, (char*)frame.Src.data(), 32);
				nlohmann::json jo = nlohmann::json::object();
				jo["name"] = name;
				jo["key_x"] = frame.KeyX;
				jo["key_y"] = frame.KeyY;
				jo["width"] = frame.Width;
				jo["height"] = frame.Height;
				savejo["children"].push_back(jo);
			}
		}
		char save_path[512];
		sprintf_s(save_path, "%s/a.json", path.c_str());
		
		std::fstream f(save_path, std::ios::out);
		std::string savejs = savejo.dump(4);
		f << savejs << std::endl;
		f.close();
	}
}

void ResourceManager::ExportBGM(uint64_t resID, CXString path)
{
	uint32_t pack = 0;
	uint32_t wasID = 0;
	DecodeWAS(resID, pack, wasID);
	uint64_t resid = res_encode_was(pack, wasID);
	uint8_t* pData = NULL;
	size_t size = 0;
	int type = RESOURCE_MANAGER_INSTANCE->LoadWDFData(resid, pData, size);
	if (type == NE::eFILE_TYPE_MP3) {
		std::fstream f(path, std::ios::out | std::ios::binary);
		f.write((char*)pData, size);
		f.close();
	}
}

PalSpriteInfo* ResourceManager::LoadSprite(uint64_t resID, std::vector<PalSchemePart>* pat)
{
	auto it = m_Sprites.find(resID);
	if (it != m_Sprites.end()) {
		if (pat == nullptr) {
			//assert(it->second.size() == 1);
			if (it->second[0] != nullptr) {
				return it->second[0];
			}
		}
		else {
			auto& spinfos = it->second;
			for (auto* info : spinfos)
			{
				if (info != nullptr) {
					if (utils_is_pal_scheme_part_equal(*pat, info->pat)) {
						return info;
					}
				}
			}
		}
	}

	uint32_t pack, wasid;
	DecodeWAS(resID, pack, wasid);
	PalSpriteInfo* info = new PalSpriteInfo();
	if (s_Loaders.find(pack) == s_Loaders.end())
	{
		s_Loaders[pack] = new NE::WDF(utils::GetPathByPackID(pack));
	}
	if (pat) {
		info->sprite = s_Loaders[pack]->UnpackSprite(wasid, *pat);
		info->pat = *pat;
		info->pati = (int)m_Sprites[resID].size();
	}
	else {
		info->sprite = s_Loaders[pack]->UnpackSprite(wasid, {});
		info->pat = {};
		info->pati = 0;
	}
	info->refCount = 0;
	bool update = false;
	for (int i = 0; i < m_Sprites[resID].size(); i++) {
		auto* v = m_Sprites[resID][i];
		if (v == nullptr) {
			m_Sprites[resID][i] = info;
			update = true;
			break;
		}
	}
	if (!update) {
		m_Sprites[resID].push_back(info);
	}
	return info;
}



void ResourceManager::UnLoadSprite(uint64_t resID, std::vector<PalSchemePart>* pat)
{
	auto it = m_Sprites.find(resID);
	if (it != m_Sprites.end()) {
		if (pat == nullptr) {
			//assert(it->second.size() == 1);
			auto* info = it->second[0];
			assert(info->refCount == 0);
			delete info->sprite;
			info->sprite = nullptr;
			delete info;
			it->second[0] = nullptr;
		}
		else {
			auto& spinfos = it->second;
			int i = 0;
			for (auto* info : spinfos)
			{
				if (utils_is_pal_scheme_part_equal(*pat, info->pat)) {
					assert(info->refCount == 0);
					delete info->sprite;
					info->sprite = nullptr;
					delete info;
					it->second[i] = nullptr;
				}
				i++;
			}
		}
	}
}

int ResourceManager::LoadWDFData(uint64_t id, uint8_t*& pData, size_t& size)
{
	uint32_t pack = 0;
	uint32_t wasID = 0;
	RESOURCE_MANAGER_INSTANCE->DecodeWAS(id, pack, wasID);

	if (s_Loaders.find(pack) == s_Loaders.end())
	{
		s_Loaders[pack] = new NE::WDF(utils::GetPathByPackID(pack));
	}

	s_Loaders[pack]->LoadFileData(wasID, pData, size);
	int type = NE::check_file_type((char*)pData, size);
	return type;
}

int ResourceManager::GetSpriteCount()
{
	return m_Sprites.size();
}


void resource_manager_init()
{
	RESOURCE_MANAGER_INSTANCE;
}

void resource_manager_update()
{

	RESOURCE_MANAGER_INSTANCE->OnUpdate();

}
void resource_manager_deinit()
{
	RESOURCE_MANAGER_INSTANCE->Clear();
}

int res_get_was(lua_State* L) {
	uint32_t pack = (uint32_t)lua_tointeger(L, 1);
	uint32_t wasid = (uint32_t)lua_tointeger(L, 2);

	uint8_t* pData;
	size_t size;
	int type = RESOURCE_MANAGER_INSTANCE->LoadWDFData(res_encode_was(pack, wasid), pData, size);
	lua_pushinteger(L, type);
	return 1;
}

int res_export_was(lua_State* L) {
	uint32_t pack = (uint32_t)lua_tointeger(L, 1);
	uint32_t wasid = (uint32_t)lua_tointeger(L, 2);
	std::string path = lua_tostring(L, 3);
	RESOURCE_MANAGER_INSTANCE->ExportWas(res_encode_was(pack, wasid), path);
	return 0;
}

int res_export_was_image(lua_State* L) {
	uint64_t resid = (uint64_t)lua_tointeger(L, 1);
	std::string path = lua_tostring(L, 2);
	RESOURCE_MANAGER_INSTANCE->ExportWas(resid, path);
	return 0;
}

int res_export_bgm(lua_State* L) {
	std::string path = lua_tostring(L, 1);
	uint64_t resid = (uint64_t)lua_tointeger(L, 2);
	RESOURCE_MANAGER_INSTANCE->ExportBGM(resid, path);
	
	return 0;
}

void res_export_map(int map_id){
	char outdir[256]{0};
	sprintf_s(outdir, "%s/%d", FileSystem::GetResourcePath("unpack/map").c_str(), map_id);
	char cmd[256];
	sprintf_s(cmd, "mkdir -p \"%s/tile\"", outdir);
	printf("cmd %s", cmd);
	system(cmd);

	sprintf_s(cmd, "mkdir -p \"%s/mask\"", outdir);
	printf("cmd %s", cmd);
	system(cmd);

	std::string path = FileSystem::GetMapPath(std::to_string(map_id));
	auto* xyqMap = new NE::MAP(path.c_str());
	int row = xyqMap->Row();
	int col = xyqMap->Col();

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			int unit = i * col + j;
			xyqMap->ReadUnit(unit);
			/*char save_path[512];
			sprintf_s(save_path, "%s/tile/%d_%d.bmp", outdir, i,j);

			int w = 0, h = 0;
			int n = 0;
			uint8_t* imgBuffer = stbi_load_from_memory(xyqMap->GetUnitBitmap(unit), (int)xyqMap->GetUnitBitmapSize(unit),
				&w, &h, &n, 3);

			uint8_t* newBuffer = new uint8_t[h * w * n]{0};
			for (int imy = 0; imy < h; imy++) {
				for (int imx = 0; imx < w; imx++) {
					uint8_t b = imgBuffer[imy * w * n + imx * n];
					uint8_t g = imgBuffer[imy * w * n + imx * n+1];
					uint8_t r = imgBuffer[imy * w * n + imx * n+2];
					imgBuffer[imy * w * n + imx * n] = r;
					imgBuffer[imy * w * n + imx * n+1] = g;
					imgBuffer[imy * w * n + imx * n+2] = b;
				}
				memcpy(newBuffer + (h - 1 - imy) * w * n  , imgBuffer + imy * w * n, w * n);
			}
			stbi_image_free(imgBuffer);
			Bitmap::CreateBitmap(save_path, w, h, (char*)newBuffer, 24);
			delete[] newBuffer;*/
		}
	}
	int m_CellWidth = col * 16;
	int m_CellHeight = row * 12;
	Json jo = Json::object();
	jo["row"] = row;
	jo["col"] = col;
	jo["image_width"] = xyqMap->Width();
	jo["image_height"] = xyqMap->Height();
	jo["map_width"] = xyqMap->MapWidth();
	jo["map_height"] = xyqMap->MapHeight();
	jo["cell_width"] = m_CellWidth;
	jo["cell_height"] = m_CellHeight;
	jo["masks"] = Json::array();

	std::vector<uint16_t> m_Cell;
	m_Cell.resize(m_CellWidth * m_CellHeight, 0);
	for (int y = 0; y < m_CellHeight; y++) {
		int i_row = y / 12;
		int cy = y % 12;
		for (int x = 0; x < m_CellWidth; x++) {
			int i_col = x / 16;
			int cx = x % 16;
			auto& unit = xyqMap->GetUnit(i_row * col + i_col);
			m_Cell[y * m_CellWidth + x] = unit.Cell[cy * 16 + cx]>0?0xffff:0;
		}
	}

	char cell_save_path[512];
	sprintf_s(cell_save_path, "%s/cell.bmp", outdir);
	Bitmap::CreateBitmap(cell_save_path, m_CellWidth, m_CellHeight, (char*)m_Cell.data(), 16);
	
	for (int i = 0; i < xyqMap->MaskSize(); i++) {
		xyqMap->ReadMask(i);
		auto& mask = xyqMap->GetMask(i);
		char path[512]{0};
		sprintf_s(path, "%s/mask/%d.bmp", outdir,i);
		Bitmap::CreateBitmap(path, mask.Width, mask.Height, (char*)mask.Data.data(), 32);

		Json joMask= Json::object();
		joMask["index"] = i;
		joMask["width"] = mask.Width;
		joMask["height"] = mask.Height;
		joMask["start_x"] = mask.StartX;
		joMask["start_y"] = mask.StartY;
		joMask["size"] = mask.Size;
		jo["masks"].push_back(joMask);
	}

	char json_path[512]{ 0 };
	sprintf_s(json_path, "%s/map.json", outdir);

	std::fstream f(json_path, std::ios::out);
	std::string savejs = jo.dump(4);
	f << savejs << std::endl;
	f.close();
}


void luaopen_resource_manager(lua_State* L)
{
#define REG_ENUM(e)  (lua_pushinteger(L, e),lua_setglobal(L, #e))
	REG_ENUM(ADDONWDF);
	REG_ENUM(ATOMWDF);
	REG_ENUM(CHATWDF);
	REG_ENUM(FIREWORKWDF);
	REG_ENUM(GOODSWDF);
	REG_ENUM(ITEMWDF);
	REG_ENUM(MAGICWDF);
	REG_ENUM(MAPANIWDF);
	REG_ENUM(MHIMAGEWDF);
	REG_ENUM(MISCWDF);
	REG_ENUM(MUSICWDF);
	REG_ENUM(SCENEWDF);
	REG_ENUM(SHAPEWDF);
	REG_ENUM(SHAPEWD1);
	REG_ENUM(SHAPEWD2);
	REG_ENUM(SHAPEWD3);
	REG_ENUM(SHAPEWD4);
	REG_ENUM(SHAPEWD5);
	REG_ENUM(SHAPEWD6);
	REG_ENUM(SHAPEWD7);
	REG_ENUM(SMAPWDF);
	REG_ENUM(SOUNDWDF);
	REG_ENUM(STOCKWDF);
	REG_ENUM(WADDONWDF);
	REG_ENUM(WZIFEWDF);
	REG_ENUM(WZIFEWD1);

	REG_ENUM(AVATAR_TYPE_ROLE);
	REG_ENUM(AVATAR_TYPE_WEAPON);
	REG_ENUM(AVATAR_TYPE_SUMMON);
	REG_ENUM(AVATAR_TYPE_NPC);
	REG_ENUM(AVATAR_TYPE_COUNT);
#undef REG_ENUM


	script_system_register_function(L, resource_manager_init);
	script_system_register_function(L, resource_manager_update);
	script_system_register_function(L, resource_manager_deinit);

	script_system_register_function(L, res_export_map);

	script_system_register_luac_function(L, res_export_bgm);
	script_system_register_luac_function(L, res_get_was);
	script_system_register_luac_function(L, res_export_was);
	script_system_register_luac_function(L, res_export_was_image);
}
