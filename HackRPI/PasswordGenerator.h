#pragma once
#ifndef PASSWORDGENERATOR_H


#include <windows.h>

#include <string>


std::wstring genPass(int length, std::string lan);

#endif // !PASSWORDGENERATOR_H
