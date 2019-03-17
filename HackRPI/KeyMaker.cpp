#include "KeyMaker.h"
#include <string>
#include <locale>
#include <codecvt>

const int PRIMESIZE = 3000;

int * genPrimes() {
	int list[PRIMESIZE];
	list[0] = 2;
	int primeNum = 1;
	for (int count = 3; true; count++) {
		bool nextPrime = true;

		//a prime number is only valid it is not divisible by the prime numbers before it
		for (int count2 = 0; count2 < primeNum; count2++) {
			if (count%list[count2] == 0) {
				nextPrime = false;
				break;
			}
		}
		if (nextPrime) {

			//sets the next prime number in the array when found
			list[primeNum] = count;
			primeNum++;

			//returns the completed list once enough prime numbers has been found
			if (primeNum >= PRIMESIZE) {
				return list;
			}
		}
	}
}
int hashCode(std::string in)
{
	std::hash<std::string> string_hash;

	return string_hash(in);
}
const int MODULUS = 715827882;
int * makeKeys(LPCWSTR pass) {
	//hash the pass
	std::wstring in(pass);
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	std::string in2 = converter.to_bytes(in);
	int sum = hashCode(in2);
	int *primes;
	primes = genPrimes();




	return 0;
}
