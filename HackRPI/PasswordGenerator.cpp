#include "PasswordGenerator.h"

#include <fstream>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <tchar.h>
#include <filesystem>

const int FILESIZE = 4000;

std::string workingDir()
{
	/*char buf[256];
	GetCurrentDirectoryA(256, buf);
	return std::string(buf) + '\\';*/
	return "C:\\Users\\james.ross\\Desktop\\HackRPI\\HackRPI";
}

std::wstring genPass(int length, std::string lan) {
	std::wstring out = L"";
	while (true) {
		srand(time(NULL));
		int startIndex = rand() % FILESIZE;
		int stepSize = rand() % 5;
		srand(rand());
		std::string address = workingDir() + "\\Words\\" + lan + ".txt";
		std::wifstream fileIn(address);
		std::wstring input;


		for (int count = 0; count < startIndex; count++) {
			std::getline(fileIn, input);
		}
		while (fileIn.peek() != std::wifstream::traits_type::eof()) {
			for (int count = 0; count < stepSize; count++) {
				std::getline(fileIn, input);
			}
			//checks that the remainder will still allow a word to be put in

			getline(fileIn, input);
			while ((length - input.length() < 4 || length - input.length() > length) && length - input.length() != 0) {
				getline(fileIn, input);
			}

			//			 getline(fileIn, input);
			length -= input.length();
			out = out + input;
			if (length == 0) {
				return out;
			}
		}
	}
}