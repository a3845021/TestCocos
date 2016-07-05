#ifndef __START_H__
#define __START_H__

#include "cocos2d.h"

USING_NS_CC;

class Start : public cocos2d::Layer {
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(Start);
	void goTest(Ref* ref);
};

#endif // !__START_H__
