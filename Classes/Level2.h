#pragma once

#include "cocos2d.h"

USING_NS_CC;

class Level2 : public cocos2d::Layer {
public:
	static cocos2d::Scene* createScene();
	static Level2* create(PhysicsWorld* pw);
	virtual bool init(PhysicsWorld* pw);
	void updateTime(float dt);
	void setStartTime();
	float getTouchTime();
	void touchEvent();
	void contactEvent();
	Vec2 AIselectTarget();
	void AIshoot();
	void playShoot();
	void doPause();
	void addGift();
	void addPlayerGift(Vec2 Pos1, Vec2 Pos2, float d);
	void addAIGift(Vec2 Pos1, Vec2 Pos2, float d);
	Vec2 judgeTarget(Vec2 pos);

	Vec2 touchLocation, AItarget;
	bool isTouch, isHit;
	cocos2d::Label *scoreLabel, *timeLabel, *testLabel;
	int playScore, AIScore;
	Sprite *arrow, *background, *shooter, *AIshooter;

private:
	Size visibleSize;
	Sprite *gift, *AIgift;
	float currentTime, startTime;
	float AIshootTime;
	EventListenerTouchOneByOne* touchListener;
};