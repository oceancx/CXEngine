#include "tsv.h"
#include <fstream>
#include <sstream>
#include "utils.h"
using String = std::string;
using std::string;
using std::map;
using std::vector;
using Json = nlohmann::json;
namespace utils
{
	tsv::tsv(String path) 
	{
		std::ifstream fs(path);
		if (!fs)
		{
			//cxlog_err("tsv(String path) error!");
			return;
		};

		fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		std::stringstream  ss;
		ss << fs.rdbuf();
		fs.close();
		
		std::map<String, String> defaultRow;
		std::string file_content = ss.str();
		std::vector<String> rows = utils::split(file_content, '\n');
		Rows.clear();
		if (rows.size() > 0)
		{
			Keys = utils::split_by_cuts(rows[0], '\t');
			for (size_t i = 1; i < rows.size(); i++)
			{
				if (rows[i][0] == '*')
				{
					rows[i] = rows[i].substr(1);
					std::vector<std::string> vals = split_by_cnt(rows[i], '\t',(int) Keys.size());
					for (size_t ki = 0; ki < Keys.size(); ki++)
					{
						defaultRow[Keys[ki]] = vals[ki];
					}
					continue;
				}
				
				std::vector<std::string> vals = split_by_cnt(rows[i], '\t',(int)Keys.size());
				std::map<String, String> tsvrow;
				for (size_t ki = 0; ki < Keys.size(); ki++)
				{
					String key = Keys[ki];
					tsvrow[key] = vals[ki] != "" ? vals[ki] : defaultRow[key];
				}
				Rows.push_back(tsvrow);
			}
		}
		MapRows.clear();
		for (auto row: Rows)
		{
			MapRows.insert({ row["ID"], row });
		}
		
		//nlohmann::json json = nlohmann::json::array();
		//json = tabRows;
		//jsonstr = json.dump();

	}

	tsv::~tsv()
	{

	}

	uint32_t tsv_parse_wasstr(std::string s)
	{
		if (s == "")return 0;
		return std::stoul(s, 0, 16);
	}
}

void utils_resave_tsv_file(const char* path)
{	
	std::ifstream fs(path);
	if (!fs)
	{
		printf("read file error!");
		return ;
	};

	fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	std::stringstream  ss;
	ss << fs.rdbuf();
	fs.close();

	std::string file_content = ss.str();
	std::vector<std::string> rows = utils::split(file_content, '\n');
	if (rows.size() > 0)
	{
		std::vector<std::string> keys = utils::split(rows[0], '\t');
		if (keys.size() > 0)
		{
			std::vector<std::map<std::string, std::string>> parsedrows;
			for (size_t i = 1; i < rows.size(); i++)
			{
				if (rows[i][0] == '*')continue;
				std::map<std::string, std::string> parsedrow;		
				std::vector<std::string> vals = utils::split(rows[i], '\t');
				assert(keys.size() == vals.size());

				for (size_t j = 0; j < keys.size(); j++)
				{
					parsedrow[keys[j]] = vals[j];
				}
				parsedrows.push_back(parsedrow);
			}
			std::sort(parsedrows.begin(), parsedrows.end(), [](std::map<std::string, std::string>& lhs, std::map<std::string, std::string>& rhs) {
				return lhs["name"] < rhs["name"];
			});
			std::ofstream newfs(path);
			newfs << rows[0] << '\n';		//write keys
			for (size_t i = 0; i < parsedrows.size(); i++)
			{
				bool first = true;
				for (auto& it : parsedrows[i])
				{
					if (first)
					{
						first = false;
						newfs << it.second;
					}
					else
					{
						newfs << '\t' << it.second;
					}
				}
				newfs << '\n';
			}
			newfs.close();
		}
	}
}

//0不是number 1是整数 2是浮点数
int utils_str_is_number(const std::string& str)
{
	std::stringstream ss(str);
	double d;
	char c;
	if (!(ss >> d))
		return 0;
	if (ss >> c)
		return 0;
	int intd = (int)d;
	if (d == intd)
	{
		return 1;
	}
	else
	{
		return 2;
	}
}


int utils_parse_tsv_file(lua_State*L)
{
	const char* path = luaL_checkstring(L, 1);

	
	std::ifstream fs(path);
	if (!fs){
		printf("utils_parse_tsv_file read file error!");
		return 0;
	};

	/*fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	std::stringstream  ss;
	ss << fs.rdbuf();
	fs.close();*/
	
	std::string line;
	vector<string> col_names;
	map<string, int> col_indices;
	vector<map<string,string>> read_cols;
	bool parse_columns = !lua_isnil(L, 2);
	if (parse_columns) {
		lua_pushnil(L);
		while (lua_next(L, 2) != 0) {
			map<string, string> fmt;
			lua_pushnil(L);
			while (lua_next(L, -2) != 0) {
				string key = lua_tostring(L, -2);
				string val = lua_tostring(L, -1);
				fmt[key] = val;
				lua_pop(L, 1);
			}
			read_cols.push_back(fmt);
			lua_pop(L, 1);
		}
	}
	
	lua_newtable(L);
	int table_index = 0;
	while (std::getline(fs, line)) {
		if (table_index == 0) {
			table_index = 1;
			col_names = utils::split_by_cuts(line, '\t');

			int temp_i = 0;
			for (auto& name : col_names) {
				col_indices[name] = temp_i++;
				if (!parse_columns) {
					read_cols.push_back({ {"name",name} });
				}
			}
			continue;
		}
		if (line[0] == '*') continue;
		
		vector<string> vals = utils::split_by_cuts(line, '\t');
		assert(vals.size() == col_names.size());

		lua_newtable(L);
		
		for (auto& col : read_cols) {
			string key = col["name"];

			string val = vals[col_indices[col["name"]]];
			auto def_it = col.find("def");
			if (def_it != col.end()) {
				if(val == ""){
					val = def_it->second;
				}
			}
			auto fmt_it = col.find("fmt");
			if (fmt_it != col.end()) {
				if (fmt_it->second == "i") {
					int64_t i = std::stoi(val);
					lua_pushinteger(L, i);
					lua_setfield(L, -2, key.c_str());
				}
				else if (fmt_it->second == "n") {
					double n = std::stod(val);
					lua_pushnumber(L, n);
					lua_setfield(L, -2, key.c_str());
				}
				else if (fmt_it->second == "pos") {
					auto v2 = utils::split_by_cuts(val, ',');
					lua_newtable(L);
					if (v2.size() == 2) {
						lua_pushnumber(L, std::stod(v2[0]));
						lua_setfield(L, -2, "x");
						lua_pushnumber(L, std::stod(v2[1]));
						lua_setfield(L, -2, "y");
					}
					else {
						lua_pushnumber(L, 0);
						lua_setfield(L, -2, "x");
						lua_pushnumber(L, 0);
						lua_setfield(L, -2, "y");
					}
					lua_setfield(L, -2, key.c_str());
				}
				else if (fmt_it->second == "res") {
					auto strs = utils::split_by_cuts(val, '-');
					if (strs.size() == 2) {
						uint32_t pack = std::stoul(strs[0], 0);
						uint32_t wasID = std::stoul(strs[1], 0, 16);
						lua_pushinteger(L, res_encode_was(pack, wasID));
					}
					else {
						lua_pushinteger(L, 0);
					}
					lua_setfield(L, -2, key.c_str());
				}
			}else{
				lua_pushstring(L, val.c_str());
				lua_setfield(L, -2, key.c_str());
			}
		}
		lua_seti(L, -2, table_index++);
	}
	fs.close();

	lua_newtable(L);
	table_index = 1;
	for(auto& cname : col_names){
		lua_pushstring(L, cname.c_str());
		lua_seti(L, -2, table_index++);
	}
	return 2;
}

int utils_str_split(lua_State*L)
{
	const char* str = luaL_checkstring(L, 1);
	const char* delim = luaL_checkstring(L, 2);
	if (str == nullptr || delim == nullptr)return 0;
	std::string _str(str);
	char _delim1(delim[0]);
	char _delim2 = 0; 
	if (strlen(delim) > 1)
	{
		_delim2 = (delim[1]);
	}

	auto splitstrs = utils::split(_str, _delim1);
	if (splitstrs.size() > 0)
	{
		lua_newtable(L);
		int table_index = 1;
		for (auto& s : splitstrs)
		{
			if (_delim2 != 0)
			{
				auto splitstrs2 = utils::split(s, _delim2);
				for (auto& s2 : splitstrs2)
				{
					lua_pushinteger(L, table_index++);
					lua_pushargs(L, s2.c_str());
					lua_settable(L, -3);
				}
			}
			else
			{
				lua_pushinteger(L, table_index++);
				lua_pushargs(L, s.c_str());
				lua_settable(L, -3);
			}
		}
		return 1;
	}
	else
	{
		lua_newtable(L);
		lua_pushinteger(L, 1);
		lua_pushargs(L, str);
		lua_settable(L, -2);
		return 1;
	}
}

int utils_file_open(lua_State*L)
{
	const char* path = luaL_checkstring(L, 1);
	int mode =(int) luaL_optinteger(L, 2 , std::fstream::in|std::fstream::out);
	static std::fstream fs;
	if (fs&&fs.is_open())
	{
		fs.close();
	}

	fs.open(path, mode);
	if (!fs)
		return 0;
	lua_pushlightuserdata(L,(void*)&fs);
	return 1;
}

int utils_file_close(lua_State*L)
{
	void* udata = lua_touserdata(L, 1);
	if (!udata)return 0;
	std::fstream* fsptr = (std::fstream*)udata;
	std::fstream& fs = *fsptr;
	if (fs&&fs.is_open())
	{
		fs.close();
	}
	return 0;
}

int utils_file_write(lua_State*L)
{
	void* udata = lua_touserdata(L, 1);
	if (!udata)return 0;
	std::fstream* fsptr = (std::fstream*)udata;
	std::fstream& fs = *fsptr;
	if (fs&&fs.is_open())
	{
		const char* data = luaL_checkstring(L, 2);
		fs << data;
	}
	return 0;
}


uint64_t res_encode_was(uint32_t pack, uint32_t wasID) {
	uint64_t resID = pack; return (resID << 32) | wasID;
}

void res_decode_was(uint64_t resID, uint32_t& pack, uint32_t& wasID) {
	pack = resID >> 32; wasID = (resID & 4294967295);
}

uint64_t EncodeWAS(uint32_t pack, uint32_t wasID) { uint64_t resID = pack; return (resID << 32) | wasID; }

void DecodeWAS(uint64_t resID, uint32_t& pack, uint32_t& wasID) { pack = resID >> 32; wasID = (resID & 4294967295); };


int res_encode(lua_State* L) {
	uint32_t pack = (uint32_t)lua_tointeger(L, 1);
	uint32_t wasid = (uint32_t)lua_tointeger(L, 2);
	lua_pushinteger(L,EncodeWAS(pack, wasid));
	return 1;
}

int res_decode(lua_State* L) {
	uint64_t res = (uint64_t)lua_tointeger(L, 1);
	uint32_t pack = 0;
	uint32_t wasID = 0;
	DecodeWAS(res, pack, wasID);
	lua_pushinteger(L, pack);
	lua_pushinteger(L, wasID);
	return 2;
}

int res_parse_resid(lua_State* L) {
	const char* str = lua_tostring(L, 1);
	auto strs = utils::split(str, '-');
	if (strs.size() != 2) {
		lua_pushinteger(L, 0);
		return 1;
	}
	uint32_t pack = std::stoul(strs[0], 0);
	uint32_t wasID = std::stoul(strs[1], 0, 16);
	lua_pushinteger(L, res_encode_was(pack, wasID));
	return 1;
}

int util_gb2312_to_utf8(lua_State* L) {
	const char* str = lua_tostring(L, 1);
	std::string s = utils::GB2312ToUtf8(str);
	lua_pushstring(L, s.c_str());
	return 1;
}

int util_utf8_to_gb2312(lua_State* L) {
	const char* str = lua_tostring(L, 1);
	std::string s = utils::Utf8ToGB2312(str);
	lua_pushstring(L, s.c_str());
	return 1;
}


void luaopen_tsv(lua_State* L)
{
	script_system_register_luac_function(L, utils_parse_tsv_file);
	
	script_system_register_luac_function(L, utils_str_split);
	script_system_register_luac_function(L, utils_file_open);
	script_system_register_luac_function(L, utils_file_close);
	script_system_register_luac_function(L, utils_file_write);
	script_system_register_function(L, utils_resave_tsv_file);

	script_system_register_luac_function(L, res_encode);
	script_system_register_luac_function(L, res_decode);

	script_system_register_luac_function(L, res_parse_resid);

	script_system_register_luac_function(L, util_gb2312_to_utf8);
	script_system_register_luac_function(L, util_utf8_to_gb2312);
	
}

