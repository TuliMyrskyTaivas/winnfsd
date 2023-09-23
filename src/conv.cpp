/////////////////////////////////////////////////////////////////////
/// file: conv.cpp
///
/// summary: character conversion routines
/////////////////////////////////////////////////////////////////////

#include "conv.h"
#include <cstring>
#include <windows.h>

/////////////////////////////////////////////////////////////////////
wchar_t* ConvFromUtf8(const char* s)
{
	auto count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s, static_cast<int>(strlen(s)), nullptr, 0);
	if (count == 0)
	{
		return nullptr;
	}
	auto dest = new wchar_t[count + 1];
	auto err = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s, static_cast<int>(strlen(s)), dest, count);
	if (err == 0)
	{
		return nullptr;
	}
	dest[count] = 0;
	return dest;
}

/////////////////////////////////////////////////////////////////////
wchar_t* ConvFromCp932(const char* s)
{
	auto count = MultiByteToWideChar(932, MB_ERR_INVALID_CHARS, s, static_cast<int>(strlen(s)), nullptr, 0);
	if (count == 0)
	{
		return nullptr;
	}
	auto dest = new wchar_t[count + 1];
	auto err = MultiByteToWideChar(932, MB_ERR_INVALID_CHARS, s, static_cast<int>(strlen(s)), dest, count);
	if (err == 0)
	{
		return nullptr;
	}
	dest[count] = 0;
	return dest;
}

/////////////////////////////////////////////////////////////////////
char* ConvToCp932(const wchar_t* s)
{
	auto count = WideCharToMultiByte(932, 0, s, static_cast<int>(wcslen(s)), nullptr, 0, nullptr, nullptr);
	if (count == 0)
	{
		return nullptr;
	}
	auto dest = new char[count + 1];
	auto err = WideCharToMultiByte(932, 0, s, static_cast<int>(wcslen(s)), dest, count, nullptr, nullptr);
	if (err == 0)
	{
		return nullptr;
	}
	dest[count] = 0;
	return dest;
}

/////////////////////////////////////////////////////////////////////
char* Utf8ToCp932(const char* s)
{
    auto utf8 = ConvFromUtf8(s);
    auto sjis = ConvToCp932(utf8);
    free(utf8);
    return sjis;
}

