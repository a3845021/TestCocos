#include <ctime>
#include <cstdlib>

class RandomNum {
private:
	RandomNum() {
		srand(time(0));
	}
	//static RandomNum* randomInstance;
public:
	static RandomNum* getInstance() {
		static RandomNum instance;
		return &instance;
	}
	int getRandomNum(int end);
	int getRandomNum(int start, int end);
};
