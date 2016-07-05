#ifndef __WIN_H__
#define __WIN_H__

#include "cocos2d.h"

USING_NS_CC;

class Win : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	// a selector callback
	void backToMenu(Ref* ref);
	void restart(Ref* ref);

	// implement the "static create()" method manually
	CREATE_FUNC(Win);
private:
	bool isWin;
};

#endif // __HELLOWORLD_SCENE_H__
