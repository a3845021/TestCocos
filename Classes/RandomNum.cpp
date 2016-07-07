#include "RandomNum.h"

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
