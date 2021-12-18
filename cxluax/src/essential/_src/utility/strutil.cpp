#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <iomanip>

#include "../../check_function.h"
#include "../../utility/strutil.h"

BEGIN_ES_NAMESPACE


std::string GetFileSuffix( const std::string& fileName )
{
	size_t dotIndex = fileName.rfind('.');

	// û���ҵ�.�ŵ����
	if (dotIndex == std::string::npos)
		return "";

	// .�������һ���ַ������
	if (dotIndex == fileName.size() - 1)
		return "";

	size_t suffixBeginPos = dotIndex + 1;
	assert_check(suffixBeginPos < fileName.size(), "GetFileSuffix");
	size_t suffixLen = fileName.size() - suffixBeginPos;
	assert_check(suffixLen > 0, "GetFileSuffix");
	assert_check(suffixLen < fileName.size() - 1, "GetFileSuffix");
	return fileName.substr(suffixBeginPos, suffixLen);
}

std::string GetFileWithoutSuffix(const std::string& fileName)
{
	size_t dotIndex = fileName.rfind('.');

	// û���ҵ�.�ŵ����
	if (dotIndex == std::string::npos)
		return "";

	// .�������һ���ַ������
	if (dotIndex == fileName.size() - 1)
		return "";

	std::string rst = fileName;
	rst.resize(dotIndex);
	return rst;
}

// ��ȡ�ļ���·����. ����"c:\34.txt",���"c:\"
std::string GetFillPath_ByFullPathName(const std::string& fullPathName)
{
	assert_check(fullPathName.size() > 0, "GetFillPath_ByFullPathName");

	size_t pos = fullPathName.rfind('\\');
	assert_check(pos != std::string::npos, "GetFillPath_ByFullPathName");
	
	std::string rst = fullPathName;
	rst.resize(pos + 1);
	return rst;
}

std::string GetFileNameWithoutPath(const std::string& fullPathName)
{
	assert_check(fullPathName.size() > 0, "GetFileNameWithoutPath");

	size_t pos = fullPathName.rfind('\\');
	assert_check(pos != std::string::npos, "GetFileNameWithoutPath");

	std::string rst = &fullPathName[pos + 1];
	return rst;
}


///////////////////////////////////////////////////////////////////////
// StrToData��ʵ�ִ���
///////////////////////////////////////////////////////////////////////
//
static long pow_i(int d,int n)
{
	int i;
	long m=1;
	for(i=0;i<n;i++)
		m*=d;
	return m;
}

static bool isRightData(char c,int jz)
{
	if((jz<2)||(jz>16))
		return false;
	char ch[]="00112233445566778899aAbBcCdDeEfF";
	int i=0;
	for(i=0;i<jz;i++)
	{
		if(c==ch[i*2])return true;
		if(c==ch[i*2+1])return true;
	}
	return false;
}

long StrToData(const ::std::string& strData, int jz)
{
	int l = (int)strData.length();
	long IntValue=0;
	int bitValue = 0;
	int i;
	for(i=0;i<l;i++)
	{ 
		if(isRightData(strData[i],jz))
		{
			bitValue = ToHexDigit(strData[i]);
			IntValue=bitValue*pow_i(jz,l-i-1)+IntValue;
		}
		else
			return -1;
	}
	return IntValue;
}

int ToHexDigit( char c )
{
	assert(isRightData(c, 16));

	int bitValue = 0;
	if(isdigit(c))
		bitValue=c-0x30;
	else
	{
		switch(c)
		{
		case 'a':
		case 'A':
			bitValue=0x0a;
			break;

		case 'b':
		case 'B':
			bitValue=0x0b;
			break;

		case 'c':
		case 'C':
			bitValue=0x0c;
			break;

		case 'd':
		case 'D':
			bitValue=0x0d;
			break;

		case 'e':
		case 'E':
			bitValue=0x0e;
			break;

		case 'f':
		case 'F':
			bitValue=0x0f;
			break;
		}
	}	
	return bitValue;
}

///////////////////////////////////////////////////////////////////////
// ConvertToCStyleStr��ʵ�ִ���
///////////////////////////////////////////////////////////////////////
//

#define __ENABLE_ASSERT_IN_STYLESTR_CVT__ // ����ʱ�����ر� "C���ת������" �����assert�� �� 

//00 01 02 03 04 05 06 07  ........
//08 09 0a 0b 0c 0d 0e 0f  ........
//10 11 12 13 14 15 16 17  ........
//18 19 1a 1b 1c 1d 1e 1f  ........
//20 21 22 23 24 25 26 27   !"#$%&'
//28 29 2a 2b 2c 2d 2e 2f  ()*+,-./
//30 31 32 33 34 35 36 37  01234567
//38 39 3a 3b 3c 3d 3e 3f  89:;<=>?
//40 41 42 43 44 45 46 47  @ABCDEFG
//48 49 4a 4b 4c 4d 4e 4f  HIJKLMNO
//50 51 52 53 54 55 56 57  PQRSTUVW
//58 59 5a 5b 5c 5d 5e 5f  XYZ[\]^_
//60 61 62 63 64 65 66 67  `abcdefg
//68 69 6a 6b 6c 6d 6e 6f  hijklmno
//70 71 72 73 74 75 76 77  pqrstuvw
//78 79 7a 7b 7c 7d 7e 7f  xyz{|}~.
::std::string ConvertToCStyleStr( const ::std::string& _Str )
{
	static const char* shiftMap[] = {
		"\\(00)", "\\(01)", "\\(02)", "\\(03)", "\\(04)", "\\(05)", "\\(06)", "\\(07)",
		"\\(08)", "\\t"   , "\\n"   , "\\(0B)", "\\(0C)", "\\r"   , "\\(0E)", "\\(0F)",
		"\\(10)", "\\(11)", "\\(12)", "\\(13)", "\\(14)", "\\(15)", "\\(16)", "\\(17)",
		"\\(18)", "\\(19)", "\\(1A)", "\\(1B)", "\\(1C)", "\\(1D)", "\\(1E)", "\\(1F)",
		" "     , "!"     , "\""    , "#"     , "$"     , "%"     , "&"     , "\'"    , 
		"("     , ")"     , "*"     , "+"     , ","     , "-"     , "."     , "/"     , 
		"0"     , "1"     , "2"     , "3"     , "4"     , "5"     , "6"     , "7"     , 
		"8"     , "9"     , ":"     , ";"     , "<"     , "="     , ">"     , "?"     , 
		"@"     , "A"     , "B"     , "C"     , "D"     , "E"     , "F"     , "G"     , 
		"H"     , "I"     , "J"     , "K"     , "L"     , "M"     , "N"     , "O"     , 
		"P"     , "Q"     , "R"     , "S"     , "T"     , "U"     , "V"     , "W"     , 
		"X"     , "Y"     , "Z"     , "["     , "\\\\"  , "]"     , "^"     , "_"     , 
		"`"     , "a"     , "b"     , "c"     , "d"     , "e"     , "f"     , "g"     , 
		"h"     , "i"     , "j"     , "k"     , "l"     , "m"     , "n"     , "o"     , 
		"p"     , "q"     , "r"     , "s"     , "t"     , "u"     , "v"     , "w"     , 
		"x"     , "y"     , "z"     , "{"     , "|"     , "}"     , "~"     , "\\(7F)"
	};

	::std::string rstString;
	rstString.reserve(_Str.size() * 2); // Ԥ�����һ��ռ䣬��ֹstring�ڲ�Ƶ�����·����ڴ�

	for (size_t i = 0; i < _Str.size(); i++)
	{
		bool isAscIIChar =  _Str[i] >= 0 && _Str[i] <= 0x7f;
		if (isAscIIChar)
			rstString.append(shiftMap[size_t(_Str[i])]); // Ӣ���ַ�
		else
			rstString.append(1, _Str[i]); // �����ַ�
	}
	return rstString;
}

std::string hexdump_oneline(const std::string& prefix, const std::string& line, size_t width)
{
    std::ostringstream ostrm;
    ostrm << prefix;

    // hex
    //
    for (size_t i=0;i< line.size(); i++)
    {
        char hex_text[16] = "";
        snprintf(hex_text, sizeof(hex_text), "%02X ", (unsigned char)(line[i]));
        ostrm << hex_text;
        if (i == 7 || i == 15 || i == 23)
            ostrm << ' ';
    }

    // the space between hex and text
    //
    for (unsigned long spacer = line.size(); spacer < width ; spacer++)
        ostrm << "   ";
    if (line.size() <= 8)
        ostrm << "   ";
    else if (line.size() <= 16)
        ostrm << "  ";
    else if (line.size() <= 24)
        ostrm << " ";
    ostrm << ": ";


    // text
    //
    for (size_t i = 0; i < line.size(); i++)
    {
        if (line[i] < 32)
            ostrm << '.';
        else
            ostrm << char(line[i]);
    }
    ostrm << std::endl;

    return ostrm.str();
}

::std::string ToHexDumpText(const std::string& _Str, size_t width, const std::string& prefix)
{
    std::ostringstream ostrm;

    ::std::string str = _Str;
    while (str.size() > width)
    {
        ::std::string line(str.c_str(), width);
        ostrm << hexdump_oneline(prefix, line, width);
        str = str.substr(width);
    }
    if (str.size() > 0)
        ostrm << hexdump_oneline(prefix, str, width);
    return ostrm.str();
}

///////////////////////////////////////////////////////////////////////
// ConvertFromCStyleStr��ʵ�ִ���
///////////////////////////////////////////////////////////////////////
//

static bool IsHexNumber(const char& c)
{
	const static char hexNumber[] = "0123456789abcdefABCDEF";
	return (strchr(hexNumber, c) != NULL);
}

// pSrc���������� "\(3F)" ��ʽ���ַ���
static int ProcShiftCharByNum(char& dst, const char* const pSrc, int srcStrLen)
{
	assert(pSrc != NULL);

	// ����Ƿ��׼��ʽ : "\(3F)"
	//
	bool isAllowdFormat = false;
	if (srcStrLen < 5)
		isAllowdFormat = false;
	else if (pSrc[0] != '\\' || pSrc[1] != '(' || pSrc[4] != ')')
		isAllowdFormat = false;
	else if ( (!IsHexNumber(pSrc[2])) || (!IsHexNumber(pSrc[3])) )
		isAllowdFormat = false;
	else
		isAllowdFormat = true;


	// ����Ƿ����
	//
	if (isAllowdFormat == false)
	{
#ifdef __ENABLE_ASSERT_IN_STYLESTR_CVT__
		assert(false); // �Ƿ��ĸ�ʽ. ��������ֻ��debug�汾��assert
#endif

		dst = pSrc[0]; // ���ڲ���ʶ���ת�壬��'\'���Ű��շ�ת��ָʾ���������������α�ָ��'\'���ź�һ���ַ�
		return 1;
	}

	// ������ȷ���
	//
	::std::string hexNumString(&pSrc[2], 2);
	int hexNum = StrToData(hexNumString, 16);
	dst = (static_cast<unsigned char>(hexNum));
	return 5; // ��׼��ʽ:"\(3F)" �պ���ռ��5���ֽ�
}

// pSrc���������� "\r" "\n" "\t" "\\" ��ʽ���ַ���
static int ProcShiftCharByChar(char& dst, const char* const pSrc, int srcStrLen)
{
	// ����ʽ
	if (srcStrLen < 2)
		goto ERR_END;

	switch (pSrc[1])
	{
	case 'r' : 
		dst = '\r'; 
		return 2;

	case 'n' : 
		dst = '\n'; 
		return 2;

	case 't' : 
		dst = '	'; 
		return 2;

	case '\\': 
		dst = '\\'; 
		return 2;

	default:
		goto ERR_END;
	}

ERR_END:
	{
#ifdef __ENABLE_ASSERT_IN_STYLESTR_CVT__
		assert(false); // �Ƿ��ĸ�ʽ. ��������ֻ��debug�汾��assert
#endif

		dst = pSrc[0]; // ���ڲ���ʶ���ת�壬��'\'���Ű��շ�ת��ָʾ���������������α�ָ��'\'���ź�һ���ַ�
		return 1;
	}
}

// �����α�Ӧ���ƶ�����λ��.
static int ProcShiftChar(char& dst, const char* pSrc, int srcStrLen)
{
	switch (pSrc[1])
	{
	case 'r': 
	case 'n': 
	case 't': 
	case '\\': 
		return ProcShiftCharByChar(dst, pSrc, srcStrLen);


	case '(': 
		return ProcShiftCharByNum(dst, pSrc, srcStrLen);

	default:

#ifdef __ENABLE_ASSERT_IN_STYLESTR_CVT__
		assert(false); // �Ƿ��ĸ�ʽ. ��������ֻ��debug�汾��assert
#endif

		// ������ʶ���ת��
		dst = pSrc[0];// ���� \s ��������ʶ���ת�壬��'\'���Ű��շ�ת��ָʾ���������������α�ָ��'s'�� �������������ʾ����"\s"
		return 1;
	}
}


::std::string ConvertFromCStyleStr( const ::std::string& _CStyleStr )
{
	::std::string rstString;
	rstString.reserve(_CStyleStr.size()); // Ԥ����ռ䣬��ֹstring�ڲ�Ƶ�����·����ڴ�

	size_t cStyleStrIndex = 0;
	while (cStyleStrIndex < _CStyleStr.size())
	{
		if (_CStyleStr[cStyleStrIndex] != '\\')
		{
			rstString.append(1, _CStyleStr[cStyleStrIndex]);
			cStyleStrIndex++;
		}
		else
		{
			char shiftChar = 0;
			int cStyleStrLeftLen = _CStyleStr.size() - cStyleStrIndex;
			cStyleStrIndex += ProcShiftChar(shiftChar, &_CStyleStr[cStyleStrIndex], cStyleStrLeftLen);
			rstString.append(1, shiftChar);
		}
	}

	return rstString;
}

int CompairNoCase(const ::std::string& lhs, const ::std::string& rhs)
{
	size_t minSize = std::min(lhs.size(), rhs.size());

	int v = 0;
	for(size_t i = 0; i < minSize; ++i)
	{
		v = tolower(lhs[i]) - tolower(rhs[i]);

		if(v != 0)
			return v;
	}

	v = lhs.size() - rhs.size();
	return v;
}

::std::string  ToLower(const ::std::string& str)
{
	::std::string ret;
	for(size_t i = 0; i < str.size(); ++i)
	{
		ret += tolower(str[i]);
	}

	return ret;
}

std::wstring& CutSpace(std::wstring& ret)
{
	while(ret.length() > 0 && ret[0] == 0X20)
		ret = ret.substr(1, ret.length() - 1);

	while(ret.length() > 0 && ret[ret.length() - 1] == 0X20)
		ret = ret.substr(0, ret.length() - 1);

	return ret;
}

std::string& CutSpace(std::string& ret)
{
	while ( ret.length() > 0 && (ret[0] == 0X20 || ret[0] == '\t') )
		ret = ret.substr(1, ret.length() - 1);

	while ( ret.length() > 0 && (ret[ret.length() - 1] == 0X20 || ret[ret.length() - 1] == '\t') )
		ret = ret.substr(0, ret.length() - 1);

	return ret;
}

std::string CutAllSpace( const std::string& srcStr )
{
	std::string noSpaceStr = "";
	for( size_t i = 0; i < srcStr.size(); ++i )
	{
		if( srcStr[i] == ' ' )
			continue;

		noSpaceStr.push_back(srcStr[i]);
	}

	return noSpaceStr;
}

END_ES_NAMESPACE
