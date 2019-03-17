
#include <string>
#include <Windows.h>
#include <vector>
#include "Blowfish.h"
#include "Account.h"
std::vector<Account> decrypt( BLOWFISH enc);

void encrypt(BLOWFISH dec, std::vector<Account> input);
