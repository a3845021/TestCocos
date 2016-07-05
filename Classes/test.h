#ifndef __TEST_H__
#define __TEST_H__

#include "cocos2d.h"

USING_NS_CC;

class Test : public cocos2d::Layer {
public:
	static cocos2d::Scene* createScene();
	static Test* create(PhysicsWorld* pw);
	virtual bool init(PhysicsWorld* pw);
	//CREATE_FUNC(Test);
	void goBack(Ref* ref);
	void updateTime(float dt);
	float getTime();
	void setStartTime();
	float getTouchTime();
	Vec2 getShootVelocity();
	float getArrowRotation();
	void setMousePosition(Vec2 pos);
	Sprite* getArrow();
	void updateScore();
	void touchEvent();
	void contactEvent();
	void spriteFadeOut(Sprite* sprite);
	void addPlayScore(int addScore);
	void addAIScore(int addScore);
	void gameOver();
	void showPerScore(Vec2 pos, int score);
	Vec2 AIselectTarget();
	void AIshoot(Vec2 targetPos);
	void recordUserDefault(bool isWin);
private:
	Size visibleSize;
	Sprite *arrow;
	Sprite *gift, *AIgift, *shooter, *AIshooter;
	float currentTime, startTime;
	Vec2 mousePosition;
	Label *scoreLabel;
	int playScore, AIScore;
	float AIshootTime;
};

#endif // !__TEST_H__

