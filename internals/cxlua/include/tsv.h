#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "lua_bind.h"

namespace utils
{
	template<typename Out>
	inline void split(const std::string &s, char delim, Out result) {
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			*(result++) = item;
		}
	}

	inline std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, std::back_inserter(elems));
		return elems;
	}

	inline std::vector<std::string> split_by_cuts(const std::string &s, char delim) {
		std::vector<std::string> elems;
		std::vector<int> cuts;
		int i = 0;
		for (auto c : s)
		{
			if (c == delim)
			{
				cuts.push_back(i);
			}
			i++;
		}
		cuts.push_back((int)s.size());
		int start = 0;
		int end = 0;
		for (int j=0; j <(int)cuts.size(); j++)
		{
			end = cuts[j];
			if (start != end)
			{
				elems.push_back(s.substr(start, end - start));
			}
			else
			{
				elems.push_back("");
			}
			start = end + 1;
		}
		return elems;
	}

	inline std::vector<std::string> split_by_cnt(const std::string &s, char delim, int fixcnt) {
		auto elems = split_by_cuts(s, delim);
		
		while ((int)elems.size() < fixcnt)
			elems.push_back("");
		while ((int)elems.size() > fixcnt)
			elems.pop_back();
		return elems;
	}

	uint32_t tsv_parse_wasstr(std::string);

	class tsv
	{
	public:
		tsv(std::string path);

		~tsv();

		std::vector<std::map<std::string, std::string>> Rows;

		std::map<std::string, std::map<std::string, std::string>> MapRows;

		std::vector<std::string> Keys;

		//std::string jsonstr;
	};
};

uint64_t res_encode_was(uint32_t pack, uint32_t wasID);
void res_decode_was(uint64_t resID, uint32_t& pack, uint32_t& wasID);


int utils_str_is_number(const std::string& str);
void luaopen_tsv(lua_State* L);