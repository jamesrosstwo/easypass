#ifndef STRING_CONVERTER_H    // To make sure you don't declare the function more than once by including the header multiple times.
#define STRING_CONVERTER_H

#include <string>
#include <windows.h>

std::string WidestringToString(const std::wstring &wstr);

std::wstring StringToWidestring(const std::string &str);

#endif