#include "test.h"
#include "start.h"
#include "RandomNum.h"
#include "win.h"
#include <vector>
#include <cmath>

#define database UserDefault::getInstance() // 本地存储实例
#define INIT_SPEED 250 // 实际速度为初始速度*点击时间
#define MAX_TOUCH_TIME 2.0f // 触控最大时间，控制最大速度
#define PI 3.14159265 // 圆周率
#define AI_SHOOT_TIME 5.0f // AI射击间隔
#define GIFT_SCORE 25 // 每个目标的分数
#define G -200.0f // 重力加速度
#define TARGET_SCORE 100 // 目标分数

// 两个投石车的位置
const Vec2 shootPosition = Vec2(120, 120);
const Vec2 AIshootPosition = Vec2(680, 120);

float tmpAIshootTime;
std::vector<Vec2> allGiftPos, AIAllGiftPos;
RandomNum random_num;

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
	tmpAIshootTime = 0.0f;

	// cocos2dx计时器 
	schedule(schedule_selector(Test::updateTime), 0.1);

	// 获取当前可视窗口大小
	visibleSize = Director::getInstance()->getVisibleSize();

	// 添加背景图片
	auto background = Sprite::create("bg.jpg");
	background->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(background, 0);

	// 添加地面刚体
	Node* ground = Node::create();
	ground->setPhysicsBody(PhysicsBody::createEdgeSegment(Vec2(0, 40), Vec2(800, 40))); // 地面用一条线作为边界
	ground->getPhysicsBody()->setDynamic(false); // 设置静态使其位置不变
	ground->setTag(0); // 设置标号，用于碰撞判断
	ground->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF); // 设置碰撞不过滤，全0为忽略碰撞检测
	this->addChild(ground, 1);
	
	// 添加中间挡板
	auto brick = Sprite::create("brick.png");
	brick->setPosition(Vec2(visibleSize.width / 2, 90));
	brick->setPhysicsBody(PhysicsBody::createBox(Size(20, 100))); // 添加一个长方形的刚体
	brick->getPhysicsBody()->setDynamic(false);
	brick->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	brick->setTag(0);
	this->addChild(brick, 1);

	// Back按钮
	auto item = MenuItemLabel::create(Label::createWithTTF("Back", "fonts/Marker Felt.ttf", 36), CC_CALLBACK_1(Test::goBack, this));
	auto menu = Menu::create(item, NULL);
	menu->setPosition(item->getContentSize().width / 2, item->getContentSize().height / 2);
	menu->setColor(Color3B::BLACK);
	this->addChild(menu, 5);

	// 添加玩家投石机
	shooter = Sprite::create("shooter.png");
	shooter->setAnchorPoint(Vec2(0, 0)); // 设置锚点为Sprite的左下角
	shooter->setPosition(100, 35);
	shooter->setPhysicsBody(PhysicsBody::createCircle(25.0f, PhysicsMaterial(), Vec2(20, -20))); // 设置刚体（有偏移）
	shooter->getPhysicsBody()->setDynamic(false);
	shooter->setTag(3);
	shooter->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	this->addChild(shooter, 2);

	// 添加AI投石机
	AIshooter = Sprite::create("AIshooter.png");
	AIshooter->setAnchorPoint(Vec2(1, 0)); // 设置锚点为Sprite的右下角
	AIshooter->setPosition(700, 35);
	AIshooter->setPhysicsBody(PhysicsBody::createCircle(25.0f, PhysicsMaterial(), Vec2(-20, -20)));
	AIshooter->getPhysicsBody()->setDynamic(false);
	AIshooter->setTag(6);
	AIshooter->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	this->addChild(AIshooter, 2);

	// 添加箭头，用于蓄力显示
	arrow = Sprite::create("arrow.png");
	arrow->setAnchorPoint(Vec2(-0.5, 0.5)); // 设置锚点，可以绕锚点转
	arrow->setPosition(shootPosition);
	arrow->setVisible(false); // 初始化时不可视
	this->addChild(arrow, 2);

	// 添加玩家的target
	for (auto pos : allGiftPos) {
		auto gift = Sprite::create("gift.png");
		gift->setPosition(pos);
		gift->setPhysicsBody(PhysicsBody::createCircle(50.0f));
		gift->getPhysicsBody()->setDynamic(false);
		gift->setTag(2);
		gift->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
		this->addChild(gift, 1);
	}

	// 添加AI的target
	for (auto pos : AIAllGiftPos) {
		auto gift = Sprite::create("AIgift.png");
		gift->setPosition(pos);
		gift->setPhysicsBody(PhysicsBody::createCircle(50.0f));
		gift->getPhysicsBody()->setDynamic(false);
		gift->setTag(5);
		gift->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
		this->addChild(gift, 1);
	}

	// 显示目标分数
	char c[30];
	sprintf(c, "Target:%d", TARGET_SCORE);
	auto targetLabel = Label::createWithTTF(c, "fonts/Marker Felt.ttf", 36);
	targetLabel->setPosition(visibleSize.width / 2, visibleSize.height - targetLabel->getContentSize().height / 2);
	targetLabel->setColor(Color3B::RED);
	this->addChild(targetLabel, 5);

	// 显示双方分数
	char t[30];
	sprintf(t, "Score:%d  AIScore:%d", playScore, AIScore);
	scoreLabel = Label::createWithTTF(t, "fonts/Marker Felt.ttf", 36);
	scoreLabel->setPosition(visibleSize.width / 2, visibleSize.height - scoreLabel->getContentSize().height * 3 / 2);
	scoreLabel->setColor(Color3B::BLACK);
	this->addChild(scoreLabel, 5);

	// 触控事件
	touchEvent();

	// 碰撞事件
	contactEvent();

	return true;
}

/*
返回按钮回调函数
点击则切换回start界面
*/
void Test::goBack(Ref * ref)
{
	auto scene = Start::createScene();
	Director::getInstance()->replaceScene(scene);
}

/*
update函数
每隔一个dt执行一次
dt单位是秒
*/
void Test::updateTime(float dt)
{
	visibleSize = Director::getInstance()->getVisibleSize();
	currentTime += dt;
	tmpAIshootTime += dt; // 记录AI射击间隔
	// AI计时超过AI射击间隔则执行射击
	if (tmpAIshootTime >= AI_SHOOT_TIME) {
		tmpAIshootTime = 0.0f; // AI计时重置
		AIshoot(AIselectTarget()); // AI射击
	}
}

/*
获取停留在当前页面总时间
*/
float Test::getTime()
{
	return currentTime;
}

/*
记录点击开始时的时间
*/
void Test::setStartTime()
{
	startTime = getTime();
}

/*
计算点击持续时间=当前时间-开始时间
*/
float Test::getTouchTime()
{
	return currentTime - startTime;
}

/*
计算玩家发射炮弹的方向向量
*/
Vec2 Test::getShootVelocity()
{
	float touchTime = this->getTouchTime();
	if (touchTime > MAX_TOUCH_TIME) touchTime = MAX_TOUCH_TIME;
	float a = mousePosition.x - shootPosition.x;
	float b = mousePosition.y - shootPosition.y;
	float c = sqrt(a * a + b * b);
	return Vec2(INIT_SPEED * touchTime / c * a, INIT_SPEED * touchTime / c * b);
}

/*
计算箭头的旋转角度
*/
float Test::getArrowRotation()
{
	float a = mousePosition.x - shootPosition.x;
	float b = mousePosition.y - shootPosition.y;
	if (a < 0) return -180 / PI * atan(b / a) + 180;
	return -180 / PI * atan(b / a);
}

/*
记录当前的鼠标位置
*/
void Test::setMousePosition(Vec2 pos)
{
	mousePosition = pos;
}

/*
获取箭头实例
*/
Sprite * Test::getArrow()
{
	return arrow;
}

/*
更新分数面板
如果任意一方达到目标分数，则游戏结束
跳转到结束界面
*/
void Test::updateScore()
{
	char t[30];
	sprintf(t, "Score:%d  AIScore:%d", playScore, AIScore);
	scoreLabel->setString(t);
	if (playScore >= TARGET_SCORE || AIScore >= TARGET_SCORE) {
		if (playScore >= TARGET_SCORE) {
			recordUserDefault(true); // 使用本地记录本次游戏结果
		}
		else {
			recordUserDefault(false); // 使用本地记录本次游戏结果
		}
		gameOver(); // 跳转到结束界面
	}
}

/*
触控事件
包括 触控开始，触控移动，触控结束 三个回调函数
*/
void Test::touchEvent()
{
	// 创建监听器
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	// 这句话不清楚什么意思。。
	listener->setSwallowTouches(true);
	
	// 点击时显示箭头，记录点击开始时间
	listener->onTouchBegan = [this](Touch* touch, Event* e) {
		this->setStartTime();
		this->setMousePosition(touch->getLocation());
		this->getArrow()->setRotation(this->getArrowRotation());
		this->getArrow()->setColor(Color3B(255, 255, 255));
		this->getArrow()->setVisible(true);
		return true;
	};

	// 点击移动时旋转箭头，并改变箭头颜色
	// 点击不移动不会触发该回调函数。= =
	listener->onTouchMoved = [this](Touch* touch, Event* e) {
		this->setMousePosition(touch->getLocation());
		this->getArrow()->setRotation(this->getArrowRotation());
		float touchTime = this->getTouchTime();
		if (touchTime > 2) touchTime = 2;
		int temp = (int)(touchTime * 100);
		this->getArrow()->setColor(Color3B(255 - temp, 255 - temp, 255 - temp));

	};

	// 点击结束隐藏箭头
	// 创建炮弹实例
	// 根据点击持续时间和方向设置炮弹的发射速度向量
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

	// 把触控监听器添加到导演事件调度
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
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
				this->spriteFadeOut(B);
			}
			else {
				this->spriteFadeOut(A);
			}
			return true;
		}

		// 两发炮弹碰撞 则都消失
		// 包括两发来自同一边的炮弹
		if ((A->getTag() == 1 && B->getTag() == 4) || (A->getTag() == 4 && B->getTag() == 1) ||
			(A->getTag() == 1 && B->getTag() == 1) || (A->getTag() == 4 && B->getTag() == 4))
		{
			this->spriteFadeOut(A);
			this->spriteFadeOut(B);
			return true;
		}

		// 玩家炮弹碰到AI目标或者玩家投石机 则炮弹消失
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

		// 玩家炮弹碰到玩家目标 炮弹和目标均消失 增加分数
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

			this->spriteFadeOut(A);
			this->spriteFadeOut(B);
			this->showPerScore(giftPosition, GIFT_SCORE);
			this->addAIScore(GIFT_SCORE);
			return true;
		}

		// 玩家炮弹命中AI投石机 AI投石机眩晕（.未实现.）
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

		// AI炮弹命中玩家投石机 玩家投石机眩晕（.未实现.）
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

		// AI炮弹命中玩家目标或AI投石机 则炮弹消失
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

	// 把碰撞监听器添加到导演事件调度
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

/*
移除某个精灵
*/
void Test::spriteFadeOut(Sprite * sprite)
{
	// 设置碰撞不检测  否则在淡出时间内也能触发碰撞事件
	sprite->getPhysicsBody()->setContactTestBitmask(0x00000000);
	// 设置淡出时间
	auto fadeOut = FadeOut::create(0.05f);
	// 让精灵执行一串事件（淡出，移除） 不移除只是变透明。。
	sprite->runAction(Sequence::create(fadeOut,
		CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sprite)), NULL));
}

/*
增加玩家分数
*/
void Test::addPlayScore(int addScore)
{
	playScore += addScore;
	updateScore();
}

/*
增加AI分数
*/
void Test::addAIScore(int addScore)
{
	AIScore += addScore;
	updateScore();
}

/*
游戏结束跳转到结束界面
*/
void Test::gameOver()
{
	auto scene = Win::createScene();
	Director::getInstance()->replaceScene(scene);
}

/*
显示命中分数
先淡入后淡出
*/
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

/*
AI选择攻击目标，有20%概率攻击玩家投石机
*/
Vec2 Test::AIselectTarget()
{
	int random = random_num.getRandomNum(10);
	if (random < 2) return shooter->getPosition();
	return AIAllGiftPos[random_num.getRandomNum(AIAllGiftPos.size())];
}

/*
创建AI炮弹，攻击目标
没有误差机制，必命中
*/
void Test::AIshoot(Vec2 targetPos)
{

	float t = sqrt(2 * (AIshootPosition.x + AIshootPosition.y - targetPos.x - targetPos.y) / -G);
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

/*
本地保存游戏数据
*/
void Test::recordUserDefault(bool isWin)
{
	database->setBoolForKey("isWin", isWin);
}
