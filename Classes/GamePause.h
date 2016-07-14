#pragma once
#include "cocos2d.h"

USING_NS_CC;

class GamePause : public cocos2d::Layer {
public:
	static cocos2d::Scene* createScene();
	static cocos2d::Scene* createScene(RenderTexture* sqr);
	virtual bool init();
	CREATE_FUNC(GamePause);
};