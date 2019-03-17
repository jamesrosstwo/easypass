#include "Cipher.h"
#include "Blowfish.h"
#include "StringConverter.h"
#include <fstream>
const std::string address = "";
std::vector<Account> decrypt( BLOWFISH  enc) {
	std::wifstream fileIn(address);
	std::vector<Account> vec;
	while (fileIn.peek() != std::wifstream::traits_type::eof()) {
		std::wstring next;
		std::getline(fileIn, next);
		//		std::wcout<<next;
		//		std::cout<<"________________________"<<std::endl;
		std::string s = WidestringToString(next);
		std::string delimiter = " ";
		Account nextAcc;
		size_t pos = 0;
		pos = s.find(delimiter);
		std::string token;
		token = s.substr(0, pos);
		//		std::cout<<bf.Decrypt_CBC(token)<<std::endl;

		nextAcc.title = StringToWidestring(enc.Decrypt_CBC(token));
		//		std::cout<<"here";
		s.erase(0, pos + delimiter.length());
		pos = s.find(delimiter);
		token = s.substr(0, pos);
		nextAcc.username = StringToWidestring(enc.Decrypt_CBC(token));
		s.erase(0, pos + delimiter.length());
		pos = s.find(delimiter);
		nextAcc.password = StringToWidestring(enc.Decrypt_CBC(s));

		vec.push_back(nextAcc);


	}
	return vec;
}

void encrypt( BLOWFISH dec, std::vector<Account> input) {

	std::ofstream fileOut(address);
	for (Account next : input) {
		fileOut << dec.Encrypt_CBC(WidestringToString(next.title));
		fileOut << " ";
		fileOut << dec.Encrypt_CBC(WidestringToString(next.username));
		fileOut << " ";
		fileOut << dec.Encrypt_CBC(WidestringToString(next.password)) << std::endl;
	}

}

