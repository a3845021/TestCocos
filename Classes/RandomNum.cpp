#include "RandomNum.h"
#include <cstdlib>

RandomNum::RandomNum() {
	srand(time(0));
}

int RandomNum::getRandomNum()
{
	return rand() % 1000 - 500;
}

int RandomNum::getRandomNum(int end)
{
	if (end <= 0) return 0;
	return rand() % end;
}
