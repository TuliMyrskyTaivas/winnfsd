/////////////////////////////////////////////////////////////////////
/// file: conv.cpp
///
/// summary: character conversion routines
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_CONV_H
#define ICENFSD_CONV_H

wchar_t* ConvFromUtf8(const char* s);
wchar_t* ConvFromCp932(const char* s);
char* ConvToCp932(const wchar_t* s);
char* Utf8ToCp932(const char* s);

#endif // ICENFSD_CONV_H