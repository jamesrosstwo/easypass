#include "PasswordGenerator.h"

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <stdio.h>
using namespace std;


const int FILESIZE = 4000;
wstring genPass(byte length, std::string lan) {
	wstring out = L"";
	while (true) {
		srand(time(NULL));
		int startIndex = rand() % FILESIZE;
		int stepSize = rand() % 5;
		srand(rand());
		cout << startIndex;
		cout << " ";
		cout << stepSize;
		string address = "C:\\Users\\alex.zhang\\Desktop\\HackRPI\\HackRPI\\Words\\" + lan + ".txt";
		wifstream fileIn(address);
		wstring input;


		for (int count = 0; count < startIndex; count++) {
			getline(fileIn, input);
		}
		while (fileIn.is_open()) {
			//take steps;
			if (length > 50) {
				return L"shit";
			}
			for (int count = 0; count < stepSize; count++) {
				getline(fileIn, input);
			}
			//checks that the remainder will still allow a word to be put in

			getline(fileIn, input);
			while ((length - input.length() < 4 || length - input.length() > length) && length - input.length() != 0) {
				getline(fileIn, input);
			}

			//			 getline(fileIn, input);
			length -= input.length();
			out = out + input;
			cout << (int)length;
			cout << " ";
			wcout << input;
			cout << " a" << endl;
			if (length == 0) {
				return out;
			}

		}





	}

	//fileIn.open()
}

