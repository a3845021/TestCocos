#include "RandomNum.h"
/*#include <cstdlib>

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

RandomNum * RandomNum::getInstance()
{
	if (randomInstance == NULL)
		randomInstance = new RandomNum();
	return randomInstance;
}*/

int RandomNum::getRandomNum(int end)
{
	if (end <= 0) return 0;
	return rand() % end;
}

int RandomNum::getRandomNum(int start, int end)
{
	int i = end - start;
	if (i <= 0) return 0;
	return rand() % i + start;
}
