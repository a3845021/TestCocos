#include "test.h"
#include "MyAction.h"
#include "start.h"
#include "RandomNum.h"
#include "win.h"
#include <vector>
#include <cmath>

#define director Director::getInstance() //导演类单例
#define random_num RandomNum::getInstance() // 随机数单例
#define my_action MyAction::getInstance() // MyAction存放可重用代码

#define GAME_TIME 60.0f
#define INIT_SPEED 300 // 实际速度为初始速度*点击时间
#define MAX_TOUCH_TIME 2.0f // 触控最大时间，控制最大速度
#define DIZZY_TIME 2.0f // 被命中后眩晕时间
#define AI_DEVIATION 0.2f //AI射击误差范围
#define AI_SHOOT_PLAYER_PROBABILITY 0.2f // AI射击玩家概率
#define GIFT_SCORE 25 // 每个目标的分数
#define G -200.0f // 重力加速度
#define TARGET_SCORE 100 // 目标分数
#define SCORE_FORMAT "Score:%d  AIScore:%d" // 分数字符串
#define MARKER_FELT_TTF "fonts/Marker Felt.ttf" // 字体路径

// 两个投石车、发射点、眩晕图标的位置
const Vec2 playerPosition = Vec2(100, 35);
const Vec2 shootPosition = Vec2(120, 120);
const Vec2 playerDizzyPosition = Vec2(160, 150);
const Vec2 AIPosition = Vec2(700, 35);
const Vec2 AIshootPosition = Vec2(680, 120);
const Vec2 AIdizzyPosition = Vec2(640, 150);

const int ground_height = 40; // 地面高度

float AIshootTime = 4.0f; // AI射击间隔

float tmpAIshootTime, playerDizzyTime;
std::vector<Vec2> allGiftPos, AIAllGiftPos;
cocos2d::Vector<Sprite*> allPlayerBullet, allAIBullet;

/*
创建带物理引擎的场景
设置重力加速度大小方向
*/
cocos2d::Scene * Test::createScene()
{
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setGravity(Vec2(0, G));
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

/*
初始化场景
*/
bool Test::init(PhysicsWorld* pw)
{
	if (!Layer::init())
	{
		return false;
	}
	
	// 初始化gift位置列表
	allGiftPos.clear();
	AIAllGiftPos.clear();

	allGiftPos.push_back(Vec2(750, 250));
	allGiftPos.push_back(Vec2(750, 350));
	allGiftPos.push_back(Vec2(750, 450));
	allGiftPos.push_back(Vec2(600, 450));

	AIAllGiftPos.push_back(Vec2(50, 250));
	AIAllGiftPos.push_back(Vec2(50, 350));
	AIAllGiftPos.push_back(Vec2(50, 450));
	AIAllGiftPos.push_back(Vec2(200, 450));

	// 初始化玩家和AI分数及其他数据
	playScore = AIScore = 0;
	currentTime = startTime = 0;
	tmpAIshootTime = playerDizzyTime = 0.0f;
	isTouch = isHit = false;

	// cocos2dx计时器 
	schedule(schedule_selector(Test::updateTime), 0.1);

	// 获取当前可视窗口大小
	visibleSize = director->getVisibleSize();

	// 添加背景图片
	background = Sprite::create("bg.jpg");
	background->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(background, 0);

	// 添加地面刚体
	auto ground = my_action->createNode(0, PhysicsBody::createEdgeSegment(Vec2(0, ground_height), Vec2(visibleSize.width, ground_height)), false);
	my_action->addNode(this, ground, 1);
	
	// 添加中间挡板
	auto brick = my_action->createSprite("brick.png", 0, Vec2(visibleSize.width / 2, 90), PhysicsBody::createBox(Size(20, 100)), false);
	my_action->addNode(this, brick, 1);

	// Back按钮
	auto item = MenuItemLabel::create(Label::createWithTTF("Back", MARKER_FELT_TTF, 36));
	item->setCallback([](Ref* ref) {
		my_action->changeScene(Start::createScene());
	});
	auto menu = Menu::create(item, NULL);
	menu->setPosition(visibleSize.width - menu->getContentSize().width / 2, item->getContentSize().height);
	menu->setColor(Color3B::BLACK);
	this->addChild(menu, 5);

	// 添加玩家投石机  设置锚点为Sprite的左下角
	shooter = my_action->createSprite("shooter.png", 3, playerPosition, Vec2(0, 0), 
		PhysicsBody::createCircle(25.0f, PhysicsMaterial(), Vec2(20, -20)), false);
	my_action->addNode(this, shooter, 2);

	// 添加AI投石机  设置锚点为Sprite的右下角
	AIshooter = my_action->createSprite("AIshooter.png", 6, AIPosition, Vec2(1, 0), 
		PhysicsBody::createCircle(25.0f, PhysicsMaterial(), Vec2(-20, -20)), false);
	my_action->addNode(this, AIshooter, 2);

	// 添加箭头，用于蓄力显示
	arrow = my_action->createSprite("arrow.png", 7, shootPosition, Vec2(-0.5, 0.5));
	arrow->setVisible(false); // 初始化时不可视
	my_action->addNode(this, arrow, 2);

	// 添加玩家的target
	for (auto pos : allGiftPos) {
		auto gift = my_action->createSprite("gift.png", 2, pos, PhysicsBody::createCircle(50.0f), false);
		my_action->addNode(this, gift, 1);
	}

	// 添加AI的target
	for (auto pos : AIAllGiftPos) {
		auto gift = my_action->createSprite("AIgift.png", 5, pos, PhysicsBody::createCircle(50.0f), false);
		my_action->addNode(this, gift, 1);
	}

	// 显示目标分数
	char c[30];
	sprintf(c, "Target:%d", TARGET_SCORE);
	auto targetLabel = Label::createWithTTF(c, MARKER_FELT_TTF, 36);
	targetLabel->setPosition(visibleSize.width / 2, visibleSize.height / 2 + targetLabel->getContentSize().height);
	targetLabel->setColor(Color3B::RED);
	this->addChild(targetLabel, 5);

	// test label
	/*testLabel = Label::createWithTTF("", MARKER_FELT_TTF, 36);
	testLabel->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	testLabel->setColor(Color3B::RED);
	this->addChild(testLabel, 5);*/

	// 显示双方分数
	scoreLabel = Label::createWithTTF(SCORE_FORMAT, MARKER_FELT_TTF, 36);
	scoreLabel->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	scoreLabel->setColor(Color3B::BLACK);
	my_action->updateLabelScore(scoreLabel, playScore, AIScore, SCORE_FORMAT);
	this->addChild(scoreLabel, 5);

	// 显示剩余时间
	timeLabel = Label::createWithTTF("60.0", MARKER_FELT_TTF, 36);
	timeLabel->setPosition(visibleSize.width / 2, visibleSize.height - timeLabel->getContentSize().height);
	timeLabel->setColor(Color3B::BLUE);
	this->addChild(timeLabel, 5);

	// 触控事件
	touchEvent();

	// 碰撞事件
	contactEvent();

	return true;
}

/*
update函数
每隔一个dt执行一次
dt单位是秒
鼠标点击时改变箭头颜色
*/
void Test::updateTime(float dt)
{
	visibleSize = director->getVisibleSize();
	currentTime += dt;  // 计时器
	tmpAIshootTime += dt; // 记录AI射击间隔

	// 更新时间面板
	my_action->updateLabelTime(timeLabel, currentTime, GAME_TIME);
	// 剩余时间为0则游戏结束
	if (currentTime >= GAME_TIME) {
		my_action->judgeWin(playScore, AIScore);
		my_action->changeScene(Win::createScene()); // 跳转到结束界面
	}

	// 判断分数是否到达目标
	if (playScore >= TARGET_SCORE || AIScore >= TARGET_SCORE) {
		my_action->judgeWin(playScore, AIScore);
		my_action->changeScene(Win::createScene()); // 跳转到结束界面
	}

	// test
	/*char t[30];
	sprintf(t, "%.1f", tmpAIshootTime);
	testLabel->setString(t);
	*/
	
	if (isHit) {
		playerDizzyTime += dt;
		if (playerDizzyTime >= DIZZY_TIME) {
			isHit = false;
			touchEvent(); // 重新创建触控监听器
		}
	}
	if (isTouch) {
		my_action->arrowColor(arrow, this->getTouchTime());
	}
	// AI计时超过AI射击间隔则执行射击
	if (tmpAIshootTime >= AIshootTime) {
		tmpAIshootTime = 0.0f; // AI计时重置
		AIshoot(AIselectTarget()); // AI射击
		AIshootTime = random_num->getRandomNum(300, 500) / 100.0f;
	}
}

/*
记录点击开始时的时间
*/
void Test::setStartTime()
{
	startTime = currentTime;
}

/*
计算点击持续时间=当前时间-开始时间
*/
float Test::getTouchTime()
{
	return currentTime - startTime;
}

/*
触控事件
包括 触控开始，触控移动，触控结束 三个回调函数
*/
void Test::touchEvent()
{
	// 创建监听器
	touchListener = EventListenerTouchOneByOne::create();
	// 这句话不清楚什么意思。。
	touchListener->setSwallowTouches(true);
	
	// 点击时显示箭头并旋转箭头方向，记录点击开始时间
	touchListener->onTouchBegan = [this](Touch* touch, Event* e) {
		this->setStartTime();
		this->isTouch = true;
		my_action->arrowRotation(this->arrow, shootPosition, touch->getLocation());
		this->arrow->setVisible(true);
		return true;
	};

	// 点击移动时旋转箭头方向
	touchListener->onTouchMoved = [this](Touch* touch, Event* e) {
		my_action->arrowRotation(this->arrow, shootPosition, touch->getLocation());
	};

	// 点击结束隐藏箭头
	// 创建炮弹实例
	// 根据点击持续时间和方向设置炮弹的发射速度向量
	touchListener->onTouchEnded = [this](Touch* touch, Event* e) {
		this->isTouch = false;

		this->arrow->setVisible(false);
		this->arrow->setColor(Color3B(255, 255, 255));

		float touchTime = this->getTouchTime();

		//this->setMousePosition(touch->getLocation());

		auto new_ball = my_action->createSprite("bullet.png", 1, shootPosition, PhysicsBody::createCircle(20.0f));
		Vec2 v = my_action->calPlayerShootVelocity(shootPosition, touch->getLocation(), INIT_SPEED, this->getTouchTime());
		my_action->shootAction(this, v, new_ball, 1);
		//my_action->shootAction(this, this->getShootVelocity(), new_ball, 1);
		return true;
	};

	// 把触控监听器添加到导演事件调度
	director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, background);
}

/*
碰撞事件
*/
void Test::contactEvent()
{
	// 创建碰撞监听器
	auto listener = EventListenerPhysicsContact::create();

	// 碰撞发生时的回调函数
	listener->onContactBegin = [this](PhysicsContact& contact) {
		// 获取发生碰撞的两个物体A,B
		auto A = (Sprite*)contact.getShapeA()->getBody()->getNode();
		auto B = (Sprite*)contact.getShapeB()->getBody()->getNode();

		/*
		Tag 0 地面 挡板
		Tag 1 玩家炮弹
		Tag 2 玩家目标
		Tag 3 玩家投石机
		Tag 4 AI炮弹
		Tag 5 AI目标
		Tag 6 AI投石机
		*/

		// 炮弹碰到地面或挡板 则炮弹消失
		if ((A->getTag() == 1 && B->getTag() == 0) || (A->getTag() == 0 && B->getTag() == 1) ||
			(A->getTag() == 4 && B->getTag() == 0) || (A->getTag() == 0 && B->getTag() == 4))
		{
			if (A->getTag() == 0) {
				my_action->spriteFadeOut(B);
			}
			else {
				my_action->spriteFadeOut(A);
			}
			return true;
		}

		// 两发炮弹碰撞 则都消失
		// 包括两发来自同一边的炮弹
		if ((A->getTag() == 1 && B->getTag() == 4) || (A->getTag() == 4 && B->getTag() == 1) ||
			(A->getTag() == 1 && B->getTag() == 1) || (A->getTag() == 4 && B->getTag() == 4))
		{
			my_action->spriteFadeOut(A);
			my_action->spriteFadeOut(B);
			return true;
		}

		// 玩家炮弹碰到AI目标或者玩家投石机 则炮弹消失
		if ((A->getTag() == 1 && B->getTag() == 3) || (A->getTag() == 3 && B->getTag() == 1) ||
			(A->getTag() == 1 && B->getTag() == 5) || (A->getTag() == 5 && B->getTag() == 1))
		{
			if (A->getTag() == 1) {
				my_action->spriteFadeOut(A);
			}
			else {
				my_action->spriteFadeOut(B);
			}
			return true;
		}

		// 玩家炮弹碰到玩家目标 炮弹和目标均消失 增加分数
		if ((A->getTag() == 1 && B->getTag() == 2) || (A->getTag() == 2 && B->getTag() == 1)) {
			Vec2 giftPosition;
			if (A->getTag() == 1) {
				giftPosition = B->getPosition();
			}
			else {
				giftPosition = A->getPosition();
			}
			
			my_action->spriteFadeOut(A);
			my_action->spriteFadeOut(B);
			my_action->showPerScore(giftPosition, GIFT_SCORE, this);
			my_action->addScore(this->playScore, GIFT_SCORE);
			my_action->updateLabelScore(scoreLabel, playScore, AIScore, SCORE_FORMAT);
			return true;
		}

		// AI炮弹碰到AI目标 炮弹和目标都消失 AI分数增加
		if ((A->getTag() == 4 && B->getTag() == 5) || (A->getTag() == 5 && B->getTag() == 4)) {
			Vec2 giftPosition;
			if (A->getTag() == 4) {
				giftPosition = B->getPosition();
			}
			else {
				giftPosition = A->getPosition();
			}
			
			// 把被命中的目标位置从AI目标列表里删除
			for (std::vector<Vec2>::iterator it = AIAllGiftPos.begin(); it != AIAllGiftPos.end(); it++) {
				if ((*it) == giftPosition) {
					AIAllGiftPos.erase(it);
					break;
				}
			}

			my_action->spriteFadeOut(A);
			my_action->spriteFadeOut(B);
			my_action->showPerScore(giftPosition, GIFT_SCORE, this);
			my_action->addScore(this->AIScore, GIFT_SCORE);
			my_action->updateLabelScore(scoreLabel, playScore, AIScore, SCORE_FORMAT);
			return true;
		}

		// 玩家炮弹命中AI投石机 AI投石机眩晕
		if ((A->getTag() == 1 && B->getTag() == 6) || (A->getTag() == 6 && B->getTag() == 1)) {
			if (A->getTag() == 1) {
				my_action->spriteFadeOut(A);
			}
			else {
				my_action->spriteFadeOut(B);
			}
			// AI 停止攻击2s
			tmpAIshootTime -= DIZZY_TIME;
			auto dizzy = my_action->createSprite("dizzy.png", 7, AIdizzyPosition);
			my_action->addNode(this, dizzy, 5);
			my_action->showDizzyPic(dizzy, DIZZY_TIME);

			return true;
		}

		// AI炮弹命中玩家投石机 玩家投石机眩晕（.未实现.）
		if ((A->getTag() == 3 && B->getTag() == 4) || (A->getTag() == 4 && B->getTag() == 3)) {
			if (A->getTag() == 4) {
				my_action->spriteFadeOut(A);
			}
			else {
				my_action->spriteFadeOut(B);
			}
			/*
			play stop shoot 2 second
			*/
			isHit = true;
			// 开始计算眩晕时间
			playerDizzyTime = 0.0f;
			this->arrow->setVisible(false);
			// 眩晕效果
			auto dizzy = my_action->createSprite("dizzy.png", 7, playerDizzyPosition);
			my_action->addNode(this, dizzy, 5);
			my_action->showDizzyPic(dizzy, DIZZY_TIME);
			// 移除监听器
			director->getEventDispatcher()->removeEventListenersForTarget(this->background);

			return true;
		}

		// AI炮弹命中玩家目标或AI投石机 则炮弹消失
		if ((A->getTag() == 4 && B->getTag() == 2) || (A->getTag() == 2 && B->getTag() == 4) ||
			(A->getTag() == 4 && B->getTag() == 6) || (A->getTag() == 6 && B->getTag() == 4))
		{
			if (A->getTag() == 4) {
				my_action->spriteFadeOut(A);
			}
			else {
				my_action->spriteFadeOut(B);
			}
			return true;
		}

		return true;
	};

	// 把碰撞监听器添加到导演事件调度
	director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

/*
游戏结束跳转到结束界面
*/
void Test::gameOver()
{
	my_action->changeScene(Win::createScene());
}

/*
AI选择攻击目标，有20%概率攻击玩家投石机
*/
Vec2 Test::AIselectTarget()
{
	int random = random_num->getRandomNum(1000);
	if (random < (int)(1000 * AI_SHOOT_PLAYER_PROBABILITY)) return shooter->getPosition();
	return AIAllGiftPos[random_num->getRandomNum(AIAllGiftPos.size())];
}

/*
创建AI炮弹，攻击目标
没有误差机制，必命中
*/
void Test::AIshoot(Vec2 targetPos)
{
	auto v = my_action->calAIShootVelocity(AIshootPosition, targetPos, -G, AI_DEVIATION);
	auto new_ball = my_action->createSprite("AIbullet.png", 4, AIshootPosition, PhysicsBody::createCircle(20.0f));
	my_action->shootAction(this, v, new_ball, 1);
}
