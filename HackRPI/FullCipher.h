#include "Blowfish.h"
#include "Cipher.h"
#include "StringConverter.h"

class FULLCIPHER {
public:
	BLOWFISH * bf;
	FULLCIPHER(std::wstring name);
	std::vector<Account> dec();
	void enc(std::vector<Account> input);
};

FULLCIPHER::FULLCIPHER(std::wstring name) {
	bf = new BLOWFISH(WidestringToString(name));
}

std::vector<Account> FULLCIPHER::dec() {
	return decrypt(*bf);

}

void FULLCIPHER::enc(std::vector<Account> input) {
	encrypt(*bf, input);
}