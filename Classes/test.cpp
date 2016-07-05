#include "test.h"
#include "start.h"
#include "RandomNum.h"
#include "win.h"
#include <vector>
#include <cmath>

#define database UserDefault::getInstance()
#define INIT_SPEED 250
#define MAX_TOUCH_TIME 2.0f
#define PI 3.14159265
#define AI_SHOOT_TIME 2.0f
#define GIFT_SCORE 25

const Vec2 shootPosition = Vec2(120, 120);
const Vec2 AIshootPosition = Vec2(680, 120);
const int targetScore = 100;
const float g = -200.0f;

float tmpAIshootTime;
std::vector<Vec2> allGiftPos, AIAllGiftPos;
RandomNum random_num;

cocos2d::Scene * Test::createScene()
{
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setGravity(Vec2(0, g));
	auto layer = Test::create(scene->getPhysicsWorld());
	scene->addChild(layer);
	return scene;
}

Test* Test::create(PhysicsWorld* pw)
{
	Test* pRet = new Test();
	if (pRet && pRet->init(pw)) {
		return pRet;
	}
	pRet = NULL;
	return NULL;
}

bool Test::init(PhysicsWorld* pw)
{
	if (!Layer::init())
	{
		return false;
	}

	allGiftPos.push_back(Vec2(750, 250));
	allGiftPos.push_back(Vec2(750, 350));
	allGiftPos.push_back(Vec2(750, 450));
	allGiftPos.push_back(Vec2(600, 450));

	AIAllGiftPos.push_back(Vec2(50, 250));
	AIAllGiftPos.push_back(Vec2(50, 350));
	AIAllGiftPos.push_back(Vec2(50, 450));
	AIAllGiftPos.push_back(Vec2(200, 450));

	playScore = AIScore = 0;
	currentTime = startTime = 0;
	p_world = pw;
	AIshootTime = AI_SHOOT_TIME;
	tmpAIshootTime = 0.0f;

	schedule(schedule_selector(Test::updateTime), 0.02);

	visibleSize = Director::getInstance()->getVisibleSize();

	auto background = Sprite::create("bg.jpg");
	background->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(background, 0);

	Node* ground = Node::create();
	ground->setPhysicsBody(PhysicsBody::createEdgeSegment(Vec2(0, 40), Vec2(800, 40)));
	ground->getPhysicsBody()->setDynamic(false);
	ground->setTag(0);
	ground->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	this->addChild(ground, 1);
	
	auto brick = Sprite::create("brick.png");
	brick->setPosition(Vec2(visibleSize.width / 2, 90));
	brick->setPhysicsBody(PhysicsBody::createBox(Size(20, 100)));
	brick->getPhysicsBody()->setDynamic(false);
	brick->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	brick->setTag(0);
	this->addChild(brick, 1);

	auto item = MenuItemLabel::create(Label::createWithTTF("Back", "fonts/Marker Felt.ttf", 36), CC_CALLBACK_1(Test::goBack, this));
	auto menu = Menu::create(item, NULL);
	menu->setPosition(item->getContentSize().width / 2, item->getContentSize().height / 2);
	menu->setColor(Color3B::BLACK);
	this->addChild(menu, 5);

	shooter = Sprite::create("shooter.png");
	shooter->setAnchorPoint(Vec2(0, 0));
	shooter->setPosition(100, 35);
	shooter->setPhysicsBody(PhysicsBody::createCircle(25.0f, PhysicsMaterial(), Vec2(20, -20)));
	shooter->getPhysicsBody()->setDynamic(false);
	shooter->setTag(3);
	shooter->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	this->addChild(shooter, 2);

	AIshooter = Sprite::create("AIshooter.png");
	AIshooter->setAnchorPoint(Vec2(1, 0));
	AIshooter->setPosition(700, 35);
	AIshooter->setPhysicsBody(PhysicsBody::createCircle(25.0f, PhysicsMaterial(), Vec2(-20, -20)));
	AIshooter->getPhysicsBody()->setDynamic(false);
	AIshooter->setTag(6);
	AIshooter->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	this->addChild(AIshooter, 2);

	arrow = Sprite::create("arrow.png");
	arrow->setAnchorPoint(Vec2(-0.5, 0.5));
	arrow->setPosition(shootPosition);
	arrow->setVisible(false);
	this->addChild(arrow, 2);

	for (auto pos : allGiftPos) {
		auto gift = Sprite::create("gift.png");
		gift->setPosition(pos);
		gift->setPhysicsBody(PhysicsBody::createCircle(50.0f));
		gift->getPhysicsBody()->setDynamic(false);
		gift->setTag(2);
		gift->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
		this->addChild(gift, 1);
	}

	for (auto pos : AIAllGiftPos) {
		auto gift = Sprite::create("AIgift.png");
		gift->setPosition(pos);
		gift->setPhysicsBody(PhysicsBody::createCircle(50.0f));
		gift->getPhysicsBody()->setDynamic(false);
		gift->setTag(5);
		gift->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
		this->addChild(gift, 1);
	}

	char c[30];
	sprintf(c, "Target:%d", targetScore);
	auto targetLabel = Label::createWithTTF(c, "fonts/Marker Felt.ttf", 36);
	targetLabel->setPosition(visibleSize.width / 2, visibleSize.height - targetLabel->getContentSize().height / 2);
	targetLabel->setColor(Color3B::RED);
	this->addChild(targetLabel, 5);

	char t[30];
	sprintf(t, "Score:%d  AIScore:%d", playScore, AIScore);
	scoreLabel = Label::createWithTTF(t, "fonts/Marker Felt.ttf", 36);
	scoreLabel->setPosition(visibleSize.width / 2, visibleSize.height - scoreLabel->getContentSize().height * 3 / 2);
	scoreLabel->setColor(Color3B::BLACK);
	this->addChild(scoreLabel, 5);

	touchEvent();
	contactEvent();

	return true;
}

void Test::goBack(Ref * ref)
{
	auto scene = Win::createScene();
	Director::getInstance()->replaceScene(scene);
}

void Test::updateTime(float dt)
{
	currentTime += dt;
	tmpAIshootTime += dt;
	if (tmpAIshootTime >= AIshootTime) {
		tmpAIshootTime = 0.0f;
		
		/*
		AI select shoot target

		AIshoot(Vec2)

		*/
		AIshoot(AIselectTarget());
	}
}

float Test::getTime()
{
	return currentTime;
}

void Test::setStartTime()
{
	startTime = getTime();
}

float Test::getTouchTime()
{
	return currentTime - startTime;
}

Vec2 Test::getShootVelocity()
{
	float touchTime = this->getTouchTime();
	if (touchTime > MAX_TOUCH_TIME) touchTime = MAX_TOUCH_TIME;
	float a = mousePosition.x - shootPosition.x;
	float b = mousePosition.y - shootPosition.y;
	float c = sqrt(a * a + b * b);
	return Vec2(INIT_SPEED * touchTime / c * a, INIT_SPEED * touchTime / c * b);
}

float Test::getArrowRotation()
{
	float a = mousePosition.x - shootPosition.x;
	float b = mousePosition.y - shootPosition.y;
	if (a < 0) return -180 / PI * atan(b / a) + 180;
	return -180 / PI * atan(b / a);
}

void Test::setMousePosition(Vec2 pos)
{
	mousePosition = pos;
}

Sprite * Test::getArrow()
{
	return arrow;
}

void Test::updateScore()
{
	char t[30];
	sprintf(t, "Score:%d  AIScore:%d", playScore, AIScore);
	scoreLabel->setString(t);
	if (playScore >= targetScore || AIScore >= targetScore) {
		if (playScore >= targetScore) {
			recordUserDefault(true);
		}
		else {
			recordUserDefault(false);
		}
		gameOver();
	}
}

void Test::touchEvent()
{
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	//listener->onTouchBegan = CC_CALLBACK_2(Test::onTouchBegan, this);
	listener->onTouchBegan = [this](Touch* touch, Event* e) {
		this->setStartTime();
		this->setMousePosition(touch->getLocation());
		this->getArrow()->setRotation(this->getArrowRotation());
		this->getArrow()->setColor(Color3B(255, 255, 255));
		this->getArrow()->setVisible(true);
		return true;
	};

	listener->onTouchMoved = [this](Touch* touch, Event* e) {
		this->setMousePosition(touch->getLocation());
		this->getArrow()->setRotation(this->getArrowRotation());
		float touchTime = this->getTouchTime();
		if (touchTime > 2) touchTime = 2;
		int temp = (int)(touchTime * 100);
		this->getArrow()->setColor(Color3B(255 - temp, 255 - temp, 255 - temp));

	};

	listener->onTouchEnded = [this](Touch* touch, Event* e) {

		this->getArrow()->setVisible(false);
		float touchTime = this->getTouchTime();

		this->setMousePosition(touch->getLocation());
		auto new_ball = Sprite::create("bullet.png");
		new_ball->setPhysicsBody(PhysicsBody::createCircle(20.0f));
		new_ball->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
		new_ball->setPosition(shootPosition);
		new_ball->getPhysicsBody()->setVelocity(this->getShootVelocity());
		new_ball->setTag(1);
		this->addChild(new_ball, 1);

		return true;
	};

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

void Test::contactEvent()
{
	auto listener = EventListenerPhysicsContact::create();

	listener->onContactBegin = [this](PhysicsContact& contact) {
		auto A = (Sprite*)contact.getShapeA()->getBody()->getNode();
		auto B = (Sprite*)contact.getShapeB()->getBody()->getNode();

		if ((A->getTag() == 1 && B->getTag() == 0) || (A->getTag() == 0 && B->getTag() == 1) ||
			(A->getTag() == 4 && B->getTag() == 0) || (A->getTag() == 0 && B->getTag() == 4))
		{
			if (A->getTag() == 0) {
				this->spriteFadeOut(B);
			}
			else {
				this->spriteFadeOut(A);
			}
			return true;
		}

		if ((A->getTag() == 1 && B->getTag() == 4) || (A->getTag() == 4 && B->getTag() == 1) ||
			(A->getTag() == 1 && B->getTag() == 1) || (A->getTag() == 4 && B->getTag() == 4))
		{
			this->spriteFadeOut(A);
			this->spriteFadeOut(B);
			return true;
		}

		if ((A->getTag() == 1 && B->getTag() == 3) || (A->getTag() == 3 && B->getTag() == 1) ||
			(A->getTag() == 1 && B->getTag() == 5) || (A->getTag() == 5 && B->getTag() == 1))
		{
			if (A->getTag() == 1) {
				this->spriteFadeOut(A);
			}
			else {
				this->spriteFadeOut(B);
			}
			return true;
		}

		if ((A->getTag() == 1 && B->getTag() == 2) || (A->getTag() == 2 && B->getTag() == 1)) {
			Vec2 giftPosition;
			if (A->getTag() == 1) {
				giftPosition = B->getPosition();
			}
			else {
				giftPosition = A->getPosition();
			}
			
			this->spriteFadeOut(A);
			this->spriteFadeOut(B);
			this->showPerScore(giftPosition, GIFT_SCORE);
			this->addPlayScore(GIFT_SCORE);
			return true;
		}

		if ((A->getTag() == 4 && B->getTag() == 5) || (A->getTag() == 5 && B->getTag() == 4)) {
			Vec2 giftPosition;
			if (A->getTag() == 4) {
				giftPosition = B->getPosition();
			}
			else {
				giftPosition = A->getPosition();
			}

			for (std::vector<Vec2>::iterator it = AIAllGiftPos.begin(); it != AIAllGiftPos.end(); it++) {
				if ((*it) == giftPosition) {
					AIAllGiftPos.erase(it);
					break;
				}
			}

			this->spriteFadeOut(A);
			this->spriteFadeOut(B);
			this->showPerScore(giftPosition, GIFT_SCORE);
			this->addAIScore(GIFT_SCORE);
			return true;
		}

		if ((A->getTag() == 1 && B->getTag() == 6) || (A->getTag() == 6 && B->getTag() == 1)) {
			if (A->getTag() == 1) {
				this->spriteFadeOut(A);
			}
			else {
				this->spriteFadeOut(B);
			}
			/*
			AI stop shoot 2 second
			*/
			return true;
		}

		if ((A->getTag() == 3 && B->getTag() == 4) || (A->getTag() == 4 && B->getTag() == 3)) {
			if (A->getTag() == 4) {
				this->spriteFadeOut(A);
			}
			else {
				this->spriteFadeOut(B);
			}
			/*
			play stop shoot 2 second
			*/
			return true;
		}

		if ((A->getTag() == 4 && B->getTag() == 2) || (A->getTag() == 2 && B->getTag() == 4) ||
			(A->getTag() == 4 && B->getTag() == 6) || (A->getTag() == 6 && B->getTag() == 4))
		{
			if (A->getTag() == 4) {
				this->spriteFadeOut(A);
			}
			else {
				this->spriteFadeOut(B);
			}
			return true;
		}

		return true;
	};

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

void Test::spriteFadeOut(Sprite * sprite)
{
	sprite->getPhysicsBody()->setContactTestBitmask(0x00000000);
	auto fadeOut = FadeOut::create(0.1f);
	sprite->runAction(Sequence::create(fadeOut,
		CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sprite)), NULL));
}

void Test::addPlayScore(int addScore)
{
	playScore += addScore;
	updateScore();
}

void Test::addAIScore(int addScore)
{
	AIScore += addScore;
	updateScore();
}

void Test::gameOver()
{
	auto scene = Win::createScene();
	Director::getInstance()->replaceScene(scene);
}

void Test::showPerScore(Vec2 pos, int score)
{
	char t[30];
	sprintf(t, "%d", score);
	auto label = Label::createWithTTF(t, "fonts/Marker Felt.ttf", 48);
	label->setColor(Color3B::RED);
	label->setPosition(pos);
	this->addChild(label, 2);
	auto fadeIn = FadeIn::create(0.2f);
	auto fadeOut = FadeOut::create(0.8f);
	label->runAction(Sequence::create(fadeIn, fadeOut,
		CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, label)), NULL));
}

Vec2 Test::AIselectTarget()
{
	int random = random_num.getRandomNum(10);
	if (random < 2) return shooter->getPosition();
	return AIAllGiftPos[random_num.getRandomNum(AIAllGiftPos.size())];
}

void Test::AIshoot(Vec2 targetPos)
{
	/*float t = sqrt(2 * (targetPos.y - AIshootPosition.y) / -g);
	float vx = (AIshootPosition.x - targetPos.x) / t;
	float vy = -g * t;*/

	float t = sqrt(2 * (AIshootPosition.x + AIshootPosition.y - targetPos.x - targetPos.y) / -g);
	float vx = (AIshootPosition.x - targetPos.x) / t;
	float vy = vx;

	auto new_ball = Sprite::create("AIbullet.png");
	new_ball->setPhysicsBody(PhysicsBody::createCircle(20.0f));
	new_ball->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	new_ball->setPosition(AIshootPosition);
	new_ball->getPhysicsBody()->setVelocity(Vec2(-vx, vy));
	new_ball->setTag(4);
	this->addChild(new_ball, 1);
}

void Test::recordUserDefault(bool isWin)
{
	database->setBoolForKey("isWin", isWin);
}
