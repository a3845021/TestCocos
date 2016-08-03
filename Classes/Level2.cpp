#include "Level2.h"
#include "MyAction.h"
#include "RandomNum.h"
#include "GamePause.h"
#include "win.h"
#include "res.h"

#define director Director::getInstance()
#define my_action MyAction::getInstance()
#define random_num RandomNum::getInstance()

#define GAME_TIME 60.0f
#define INIT_SPEED 250 // 实际速度为初始速度*点击时间
#define MAX_TOUCH_TIME 2.0f // 触控最大时间，控制最大速度
#define DIZZY_TIME 2.0f // 被命中后眩晕时间
#define AI_DEVIATION 0.1f //AI射击误差范围
#define AI_SHOOT_PLAYER_PROBABILITY 0.3f // AI射击玩家概率
#define GIFT_SCORE 25 // 每个目标的分数
#define G -200.0f // 重力加速度
#define TARGET_SCORE 100 // 目标分数
#define SCORE_FORMAT "Score:%d  AIScore:%d" // 分数字符串
#define TEXT_FONT "fonts/shaonvxin.ttf"
#define BALL_RADIUS 18.0f // 炮弹半径

// 两个投石车、发射点、眩晕图标的位置
const Vec2 playerPosition = Vec2(100, 35);
const Vec2 shootPosition = Vec2(150, 150);
const Vec2 playerDizzyPosition = Vec2(160, 150);
const Vec2 AIPosition = Vec2(700, 35);
const Vec2 AIshootPosition = Vec2(650, 150);
const Vec2 AIdizzyPosition = Vec2(640, 150);

const int ground_height = 40; // 地面高度

float AIshootTime_level2 = 3.0f; // AI射击间隔

float tempAIshootTime2, tempPlayerDizzyTime2;
std::vector<Vec2> allGiftPosition2, AIAllGiftPosition2;

cocos2d::Scene * Level2::createScene()
{
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setGravity(Vec2(0, G));
	auto layer = Level2::create(scene->getPhysicsWorld());
	scene->addChild(layer);
	return scene;
}

Level2 * Level2::create(PhysicsWorld * pw)
{
	Level2* pRet = new Level2();
	if (pRet && pRet->init(pw)) {
		return pRet;
	}
	pRet = NULL;
	return NULL;
}

bool Level2::init(PhysicsWorld * pw)
{
	if (!Layer::init())
	{
		return false;
	}

	// 记录所在玩关卡号
	game_level = 2;

	// 初始化gift位置列表
	allGiftPosition2.clear();
	AIAllGiftPosition2.clear();

	allGiftPosition2.push_back(Vec2(700, 250));
	allGiftPosition2.push_back(Vec2(650, 350));
	allGiftPosition2.push_back(Vec2(650, 450));
	allGiftPosition2.push_back(Vec2(470, 400));

	AIAllGiftPosition2.push_back(Vec2(100, 250));
	AIAllGiftPosition2.push_back(Vec2(150, 350));
	AIAllGiftPosition2.push_back(Vec2(150, 450));
	AIAllGiftPosition2.push_back(Vec2(330, 400));

	// 初始化玩家和AI分数及其他数据
	playScore = AIScore = 0;
	currentTime = startTime = 0;
	tempAIshootTime2 = tempPlayerDizzyTime2 = 0.0f;
	isTouch = isHit = false;

	// cocos2dx计时器 
	schedule(schedule_selector(Level2::updateTime), 0.05);

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

	/*
	// 添加玩家的target
	for (auto pos : allGiftPosition2) {
		auto gift = my_action->createSprite("gift_small.png", 2, pos, PhysicsBody::createCircle(30.0f), false);
		my_action->addNode(this, gift, 1);
	}

	// 添加AI的target
	for (auto pos : AIAllGiftPosition2) {
		auto gift = my_action->createSprite("AIgift_small.png", 5, pos, PhysicsBody::createCircle(30.0f), false);
		my_action->addNode(this, gift, 1);
	}*/

	addGift();

	// 显示目标分数
	char c[30];
	sprintf(c, "Target:%d", TARGET_SCORE);
	auto targetLabel = Label::createWithTTF(c, TEXT_FONT, 36);
	targetLabel->setPosition(visibleSize.width / 2, visibleSize.height / 2 + targetLabel->getContentSize().height);
	targetLabel->setColor(Color3B::RED);
	this->addChild(targetLabel, 5);

	// 显示双方分数
	scoreLabel = Label::createWithTTF(SCORE_FORMAT, TEXT_FONT, 36);
	scoreLabel->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	scoreLabel->setColor(Color3B::BLACK);
	my_action->updateLabelScore(scoreLabel, playScore, AIScore, SCORE_FORMAT);
	this->addChild(scoreLabel, 5);

	// 显示剩余时间
	timeLabel = Label::createWithTTF("60.0", TEXT_FONT, 36);
	timeLabel->setPosition(visibleSize.width / 2, visibleSize.height - timeLabel->getContentSize().height);
	timeLabel->setColor(Color3B::BLUE);
	this->addChild(timeLabel, 5);

	// Pause按钮
	auto pause = MenuItemImage::create("pause_button.png", "pause_button_click.png");
	pause->setCallback([&](Ref* ref) {
		doPause();
	});
	auto menu = Menu::create(pause, NULL);
	menu->setPosition(visibleSize.width / 2, visibleSize.height - pause->getContentSize().height * 2);
	menu->setColor(Color3B::BLACK);
	this->addChild(menu, 5);

	// 触控事件
	touchEvent();

	// 碰撞事件
	contactEvent();

	return true;
}

void Level2::updateTime(float dt)
{
	visibleSize = director->getVisibleSize();
	currentTime += dt;  // 计时器
	tempAIshootTime2 += dt; // 记录AI射击间隔

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

	if (isHit) {
		tempPlayerDizzyTime2 += dt;
		if (tempPlayerDizzyTime2 >= DIZZY_TIME) {
			isHit = false;
			touchEvent(); // 重新创建触控监听器
		}
	}
	if (isTouch) {
		my_action->arrowColor(arrow, this->getTouchTime());
	}
	// AI计时超过AI射击间隔则执行射击
	if (tempAIshootTime2 >= AIshootTime_level2) {
		tempAIshootTime2 = 0.0f; // AI计时重置
		AItarget = AIselectTarget();

		this->AIshooter->runAction(Sequence::create(Animate::create(AnimationCache::getInstance()->getAnimation("AIAnimation")),
			CCCallFunc::create(CC_CALLBACK_0(Level2::AIshoot, this)),
			Animate::create(AnimationCache::getInstance()->getAnimation("AIAfterShoot")), NULL));
	}
}

void Level2::setStartTime()
{
	startTime = currentTime;
}

float Level2::getTouchTime()
{
	return currentTime - startTime;
}

void Level2::touchEvent()
{
	// 创建监听器
	touchListener = EventListenerTouchOneByOne::create();
	// 这句话不清楚什么意思。。
	touchListener->setSwallowTouches(true);

	// 点击时显示箭头并旋转箭头方向，记录点击开始时间
	touchListener->onTouchBegan = [this](Touch* touch, Event* e) {
		this->setStartTime();
		this->arrow->setColor(Color3B(255, 255, 255));
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
		this->touchLocation = touch->getLocation();
		//this->setMousePosition(touch->getLocation());

		//shooter animation
		//Animate* shooterAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("playerAnimation"));
		this->shooter->runAction(Sequence::create(Animate::create(AnimationCache::getInstance()->getAnimation("playerAnimation")),
			CCCallFunc::create(CC_CALLBACK_0(Level2::playShoot, this)),
			Animate::create(AnimationCache::getInstance()->getAnimation("playerAfterShoot")), NULL));

		/*auto new_ball = my_action->createSprite("bullet.png", 1, shootPosition, PhysicsBody::createCircle(20.0f));
		Vec2 v = my_action->calPlayerShootVelocity(shootPosition, touch->getLocation(), INIT_SPEED, this->getTouchTime());
		my_action->shootAction(this, v, new_ball, 1);*/
	};

	// 把触控监听器添加到导演事件调度
	director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, background);
}

void Level2::contactEvent()
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
			my_action->playExplosionEffect(); // 爆炸音效
			if (A->getTag() == 0) {
				my_action->showExplosion(B->getPosition(), this, 5); // 爆炸效果
				my_action->spriteFadeOut(B);
			}
			else {
				my_action->showExplosion(A->getPosition(), this, 5); // 爆炸效果
				my_action->spriteFadeOut(A);
			}
			return true;
		}

		// 两发炮弹碰撞 则都消失
		// 包括两发来自同一边的炮弹
		// 触发爆炸效果
		if ((A->getTag() == 1 && B->getTag() == 4) || (A->getTag() == 4 && B->getTag() == 1) ||
			(A->getTag() == 1 && B->getTag() == 1) || (A->getTag() == 4 && B->getTag() == 4))
		{
			my_action->playExplosionEffect(); // 爆炸音效
			Vec2 posA = A->getPosition();
			Vec2 posB = B->getPosition();
			Vec2 tmp = Vec2((posA.x + posB.x) / 2, (posA.y + posB.y) / 2);
			my_action->showExplosion(tmp, this, 5); // 爆炸效果

			my_action->spriteFadeOut(A);
			my_action->spriteFadeOut(B);
			return true;
		}

		// 玩家炮弹碰到AI目标或者玩家投石机 则炮弹消失
		if ((A->getTag() == 1 && B->getTag() == 3) || (A->getTag() == 3 && B->getTag() == 1) ||
			(A->getTag() == 1 && B->getTag() == 5) || (A->getTag() == 5 && B->getTag() == 1))
		{
			my_action->playExplosionEffect(); // 爆炸音效
			if (A->getTag() == 1) {
				my_action->showExplosion(A->getPosition(), this, 5); // 爆炸效果
				my_action->spriteFadeOut(A);
			}
			else {
				my_action->showExplosion(B->getPosition(), this, 5); // 爆炸效果
				my_action->spriteFadeOut(B);
			}
			return true;
		}

		// 玩家炮弹碰到玩家目标 炮弹和目标均消失 增加分数
		if ((A->getTag() == 1 && B->getTag() == 2) || (A->getTag() == 2 && B->getTag() == 1)) {
			my_action->playGetPointEffect(); // 得分音效
			Vec2 giftPosition;
			if (A->getTag() == 1) {
				giftPosition = B->getPosition();
			}
			else {
				giftPosition = A->getPosition();
			}

			Vec2 posA = A->getPosition();
			Vec2 posB = B->getPosition();
			Vec2 tmp = Vec2((posA.x + posB.x) / 2, (posA.y + posB.y) / 2);
			my_action->showExplosion(tmp, this, 5); // 爆炸效果

			my_action->spriteFadeOut(A);
			my_action->spriteFadeOut(B);
			my_action->showPerScore(giftPosition, GIFT_SCORE, this);
			my_action->addScore(this->playScore, GIFT_SCORE);
			my_action->updateLabelScore(scoreLabel, playScore, AIScore, SCORE_FORMAT);
			return true;
		}

		// AI炮弹碰到AI目标 炮弹和目标都消失 AI分数增加
		if ((A->getTag() == 4 && B->getTag() == 5) || (A->getTag() == 5 && B->getTag() == 4)) {
			my_action->playGetPointEffect(); // 得分音效
			Vec2 giftPosition;
			if (A->getTag() == 4) {
				giftPosition = B->getPosition();
			}
			else {
				giftPosition = A->getPosition();
			}

			// 把被命中的目标位置从AI目标列表里删除
			for (std::vector<Vec2>::iterator it = AIAllGiftPosition2.begin(); it != AIAllGiftPosition2.end(); it++) {
				if ((*it) == judgeTarget(giftPosition)) {
					AIAllGiftPosition2.erase(it);
					break;
				}
			}

			Vec2 posA = A->getPosition();
			Vec2 posB = B->getPosition();
			Vec2 tmp = Vec2((posA.x + posB.x) / 2, (posA.y + posB.y) / 2);
			my_action->showExplosion(tmp, this, 5); // 爆炸效果

			my_action->spriteFadeOut(A);
			my_action->spriteFadeOut(B);
			my_action->showPerScore(giftPosition, GIFT_SCORE, this);
			my_action->addScore(this->AIScore, GIFT_SCORE);
			my_action->updateLabelScore(scoreLabel, playScore, AIScore, SCORE_FORMAT);
			return true;
		}

		// 玩家炮弹命中AI投石机 AI投石机眩晕
		if ((A->getTag() == 1 && B->getTag() == 6) || (A->getTag() == 6 && B->getTag() == 1)) {
			my_action->playExplosionEffect(); // 爆炸音效
			if (A->getTag() == 1) {
				my_action->showExplosion(A->getPosition(), this, 5); // 爆炸效果
				my_action->spriteFadeOut(A);
			}
			else {
				my_action->showExplosion(B->getPosition(), this, 5); // 爆炸效果
				my_action->spriteFadeOut(B);
			}
			// AI 停止攻击2s
			if (tempAIshootTime2 > AIshootTime_level2 - 1) tempAIshootTime2 = AIshootTime_level2 - 1;
			tempAIshootTime2 -= DIZZY_TIME;
			auto dizzy = my_action->createSprite("dizzy.png", 7, AIdizzyPosition);
			my_action->addNode(this, dizzy, 5);
			my_action->showDizzyPic(dizzy, DIZZY_TIME);

			return true;
		}

		// AI炮弹命中玩家投石机 玩家投石机眩晕
		if ((A->getTag() == 3 && B->getTag() == 4) || (A->getTag() == 4 && B->getTag() == 3)) {
			my_action->playExplosionEffect(); // 爆炸音效
			if (A->getTag() == 4) {
				my_action->showExplosion(A->getPosition(), this, 5); // 爆炸效果
				my_action->spriteFadeOut(A);
			}
			else {
				my_action->showExplosion(B->getPosition(), this, 5); // 爆炸效果
				my_action->spriteFadeOut(B);
			}
			/*
			play stop shoot 2 second
			*/
			isHit = true;
			// 开始计算眩晕时间
			tempPlayerDizzyTime2 = 0.0f;
			this->arrow->setVisible(false);
			this->arrow->setColor(Color3B(255, 255, 255));
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
			my_action->playExplosionEffect(); // 爆炸音效
			if (A->getTag() == 4) {
				my_action->showExplosion(A->getPosition(), this, 5); // 爆炸效果
				my_action->spriteFadeOut(A);
			}
			else {
				my_action->showExplosion(B->getPosition(), this, 5); // 爆炸效果
				my_action->spriteFadeOut(B);
			}
			return true;
		}

		return true;
	};

	// 把碰撞监听器添加到导演事件调度
	director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

Vec2 Level2::AIselectTarget()
{
	int random = random_num->getRandomNum(1000);
	if (random < (int)(1000 * AI_SHOOT_PLAYER_PROBABILITY)) return shooter->getPosition();
	return AIAllGiftPosition2[random_num->getRandomNum(AIAllGiftPosition2.size())];
}

void Level2::AIshoot()
{
	auto v = my_action->calAIShootVelocity(AIshootPosition, AItarget, -G, AI_DEVIATION);
	auto new_ball = my_action->createSprite("AIbullet.png", 4, AIshootPosition, PhysicsBody::createCircle(BALL_RADIUS));
	new_ball->runAction(RepeatForever::create(RotateBy::create(0.5f, -360)));
	my_action->shootAction(this, v, new_ball, 1);
}

void Level2::playShoot()
{
	auto new_ball = my_action->createSprite("bullet.png", 1, shootPosition, PhysicsBody::createCircle(BALL_RADIUS));
	new_ball->runAction(RepeatForever::create(RotateBy::create(0.5f, 360)));
	Vec2 v = my_action->calPlayerShootVelocity(shootPosition, touchLocation, INIT_SPEED, this->getTouchTime());
	my_action->shootAction(this, v, new_ball, 1);
}

void Level2::doPause()
{
	auto renderTexture = RenderTexture::create(800, 480);
	renderTexture->begin();
	this->getParent()->visit();
	renderTexture->end();
	director->pushScene(GamePause::createScene(renderTexture));
}

void Level2::addGift()
{
	addPlayerGift(Vec2(750, 250), Vec2(650, 250), 2.0f);
	addPlayerGift(Vec2(750, 350), Vec2(550, 350), 1.5f);
	addPlayerGift(Vec2(750, 450), Vec2(550, 450), 2.5f);
	addPlayerGift(Vec2(470, 450), Vec2(470, 350), 1.8f);

	addAIGift(Vec2(50, 250), Vec2(150, 250), 2.0f);
	addAIGift(Vec2(50, 350), Vec2(250, 350), 1.5f);
	addAIGift(Vec2(50, 450), Vec2(250, 450), 2.5f);
	addAIGift(Vec2(330, 450), Vec2(330, 350), 1.8f);
}

void Level2::addPlayerGift(Vec2 Pos1, Vec2 Pos2, float d)
{
	auto gift = my_action->createSprite("gift_small.png", 2, Pos1, PhysicsBody::createCircle(30.0f), false);
	auto a1 = Sequence::create(MoveTo::create(d, Pos2), MoveTo::create(d, Pos1), NULL);
	gift->runAction(RepeatForever::create(a1));
	my_action->addNode(this, gift, 1);
}

void Level2::addAIGift(Vec2 Pos1, Vec2 Pos2, float d)
{
	auto gift = my_action->createSprite("AIgift_small.png", 5, Pos1, PhysicsBody::createCircle(30.0f), false);
	auto a1 = Sequence::create(MoveTo::create(d, Pos2), MoveTo::create(d, Pos1), NULL);
	gift->runAction(RepeatForever::create(a1));
	my_action->addNode(this, gift, 1);
}

Vec2 Level2::judgeTarget(Vec2 pos)
{
	if (pos.x == 330) return Vec2(330, 400);
	if (pos.y == 250) return Vec2(100, 250);
	if (pos.y == 350) return Vec2(150, 350);
	if (pos.y == 450) return Vec2(150, 450);
	return Vec2();
}
