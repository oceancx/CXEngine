#include "utils.h"
#if defined(_WIN32)
#include <windows.h>

namespace utils
{

	std::string Utf8ToGB2312(const char* utf8)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
		len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* str = new char[len + 1];
		memset(str, 0, len + 1);
		WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
		if (wstr) delete[] wstr;
		std::string res(str);
		delete[] str;
		return res;
	}

	std::string GB2312ToUtf8(const char* gb2312)
	{
		int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
		len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* str = new char[len + 1];
		memset(str, 0, len + 1);
		WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
		if (wstr) delete[] wstr;
		std::string res(str);
		delete[] str;
		return res;
	}

	std::wstring StringToWstring(const std::string str)
	{
		auto len = str.size() * 2;
		setlocale(LC_CTYPE, "");
		wchar_t *p = new wchar_t[len];
		mbstowcs(p, str.c_str(), len);
		std::wstring str1(p);
		delete[] p;
		return str1;
	}

	std::string WstringToString(const std::wstring str)
	{
		size_t len = str.size() * 4;
		setlocale(LC_CTYPE, "");
		char *p = new char[len];
		wcstombs(p, str.c_str(), len);
		std::string str1(p);
		delete[] p;
		return str1;
	}
}
#else
#include <locale>
#include <codecvt>
#include <string>
std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

namespace utils
{

	std::string Utf8ToGB2312(const char* utf8)
	{
		return std::string(utf8);
	}

	std::string GB2312ToUtf8(const char* gb2312)
	{
		return std::string(gb2312);
	}

	std::wstring StringToWstring(const std::string str)
	{
		return converter.from_bytes(str);
	}

	std::string WstringToString(const std::wstring str)
	{
		return converter.to_bytes(str);
	}
}
#endif
