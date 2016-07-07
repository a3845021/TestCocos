#ifndef __TEST_H__
#define __TEST_H__

#include "cocos2d.h"

USING_NS_CC;

class Test : public cocos2d::Layer {
public:
	static cocos2d::Scene* createScene();
	static Test* create(PhysicsWorld* pw);
	virtual bool init(PhysicsWorld* pw);
	void updateTime(float dt);
	void setStartTime();
	float getTouchTime();
	void updateScore();
	void touchEvent();
	void contactEvent();
	void gameOver();
	Vec2 AIselectTarget();
	void AIshoot(Vec2 targetPos);
	void recordUserDefault(bool isWin);

	bool isTouch, isHit;
	cocos2d::Label *scoreLabel, *testLabel;
	int playScore, AIScore;
	Sprite *arrow, *background;
private:
	Size visibleSize;
	Sprite *gift, *AIgift, *shooter, *AIshooter;
	float currentTime, startTime;
	float AIshootTime;
	EventListenerTouchOneByOne* touchListener;
};

#endif // !__TEST_H__

