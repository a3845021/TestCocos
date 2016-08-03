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
#define INIT_SPEED 250 // ʵ���ٶ�Ϊ��ʼ�ٶ�*���ʱ��
#define MAX_TOUCH_TIME 2.0f // �������ʱ�䣬��������ٶ�
#define DIZZY_TIME 2.0f // �����к�ѣ��ʱ��
#define AI_DEVIATION 0.1f //AI�����Χ
#define AI_SHOOT_PLAYER_PROBABILITY 0.3f // AI�����Ҹ���
#define GIFT_SCORE 25 // ÿ��Ŀ��ķ���
#define G -200.0f // �������ٶ�
#define TARGET_SCORE 100 // Ŀ�����
#define SCORE_FORMAT "Score:%d  AIScore:%d" // �����ַ���
#define TEXT_FONT "fonts/shaonvxin.ttf"
#define BALL_RADIUS 18.0f // �ڵ��뾶

// ����Ͷʯ��������㡢ѣ��ͼ���λ��
const Vec2 playerPosition = Vec2(100, 35);
const Vec2 shootPosition = Vec2(150, 150);
const Vec2 playerDizzyPosition = Vec2(160, 150);
const Vec2 AIPosition = Vec2(700, 35);
const Vec2 AIshootPosition = Vec2(650, 150);
const Vec2 AIdizzyPosition = Vec2(640, 150);

const int ground_height = 40; // ����߶�

float AIshootTime_level2 = 3.0f; // AI������

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

	// ��¼������ؿ���
	game_level = 2;

	// ��ʼ��giftλ���б�
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

	// ��ʼ����Һ�AI��������������
	playScore = AIScore = 0;
	currentTime = startTime = 0;
	tempAIshootTime2 = tempPlayerDizzyTime2 = 0.0f;
	isTouch = isHit = false;

	// cocos2dx��ʱ�� 
	schedule(schedule_selector(Level2::updateTime), 0.05);

	// ��ȡ��ǰ���Ӵ��ڴ�С
	visibleSize = director->getVisibleSize();

	// ��ӱ���ͼƬ
	background = Sprite::create("bg.jpg");
	background->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(background, 0);

	// ��ӵ������
	auto ground = my_action->createNode(0, PhysicsBody::createEdgeSegment(Vec2(0, ground_height), Vec2(visibleSize.width, ground_height)), false);
	my_action->addNode(this, ground, 1);

	// ����м䵲��
	auto brick = my_action->createSprite("brick.png", 0, Vec2(visibleSize.width / 2, 90), PhysicsBody::createBox(Size(20, 100)), false);
	my_action->addNode(this, brick, 1);

	// ������Ͷʯ��  ����ê��ΪSprite�����½�
	shooter = my_action->createSprite("shooter.png", 3, playerPosition, Vec2(0, 0),
		PhysicsBody::createCircle(25.0f, PhysicsMaterial(), Vec2(20, -20)), false);
	my_action->addNode(this, shooter, 2);

	// ���AIͶʯ��  ����ê��ΪSprite�����½�
	AIshooter = my_action->createSprite("AIshooter.png", 6, AIPosition, Vec2(1, 0),
		PhysicsBody::createCircle(25.0f, PhysicsMaterial(), Vec2(-20, -20)), false);
	my_action->addNode(this, AIshooter, 2);

	// ��Ӽ�ͷ������������ʾ
	arrow = my_action->createSprite("arrow.png", 7, shootPosition, Vec2(-0.5, 0.5));
	arrow->setVisible(false); // ��ʼ��ʱ������
	my_action->addNode(this, arrow, 2);

	/*
	// �����ҵ�target
	for (auto pos : allGiftPosition2) {
		auto gift = my_action->createSprite("gift_small.png", 2, pos, PhysicsBody::createCircle(30.0f), false);
		my_action->addNode(this, gift, 1);
	}

	// ���AI��target
	for (auto pos : AIAllGiftPosition2) {
		auto gift = my_action->createSprite("AIgift_small.png", 5, pos, PhysicsBody::createCircle(30.0f), false);
		my_action->addNode(this, gift, 1);
	}*/

	addGift();

	// ��ʾĿ�����
	char c[30];
	sprintf(c, "Target:%d", TARGET_SCORE);
	auto targetLabel = Label::createWithTTF(c, TEXT_FONT, 36);
	targetLabel->setPosition(visibleSize.width / 2, visibleSize.height / 2 + targetLabel->getContentSize().height);
	targetLabel->setColor(Color3B::RED);
	this->addChild(targetLabel, 5);

	// ��ʾ˫������
	scoreLabel = Label::createWithTTF(SCORE_FORMAT, TEXT_FONT, 36);
	scoreLabel->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	scoreLabel->setColor(Color3B::BLACK);
	my_action->updateLabelScore(scoreLabel, playScore, AIScore, SCORE_FORMAT);
	this->addChild(scoreLabel, 5);

	// ��ʾʣ��ʱ��
	timeLabel = Label::createWithTTF("60.0", TEXT_FONT, 36);
	timeLabel->setPosition(visibleSize.width / 2, visibleSize.height - timeLabel->getContentSize().height);
	timeLabel->setColor(Color3B::BLUE);
	this->addChild(timeLabel, 5);

	// Pause��ť
	auto pause = MenuItemImage::create("pause_button.png", "pause_button_click.png");
	pause->setCallback([&](Ref* ref) {
		doPause();
	});
	auto menu = Menu::create(pause, NULL);
	menu->setPosition(visibleSize.width / 2, visibleSize.height - pause->getContentSize().height * 2);
	menu->setColor(Color3B::BLACK);
	this->addChild(menu, 5);

	// �����¼�
	touchEvent();

	// ��ײ�¼�
	contactEvent();

	return true;
}

void Level2::updateTime(float dt)
{
	visibleSize = director->getVisibleSize();
	currentTime += dt;  // ��ʱ��
	tempAIshootTime2 += dt; // ��¼AI������

	// ����ʱ�����
	my_action->updateLabelTime(timeLabel, currentTime, GAME_TIME);
	// ʣ��ʱ��Ϊ0����Ϸ����
	if (currentTime >= GAME_TIME) {
		my_action->judgeWin(playScore, AIScore);
		my_action->changeScene(Win::createScene()); // ��ת����������
	}

	// �жϷ����Ƿ񵽴�Ŀ��
	if (playScore >= TARGET_SCORE || AIScore >= TARGET_SCORE) {
		my_action->judgeWin(playScore, AIScore);
		my_action->changeScene(Win::createScene()); // ��ת����������
	}

	if (isHit) {
		tempPlayerDizzyTime2 += dt;
		if (tempPlayerDizzyTime2 >= DIZZY_TIME) {
			isHit = false;
			touchEvent(); // ���´������ؼ�����
		}
	}
	if (isTouch) {
		my_action->arrowColor(arrow, this->getTouchTime());
	}
	// AI��ʱ����AI��������ִ�����
	if (tempAIshootTime2 >= AIshootTime_level2) {
		tempAIshootTime2 = 0.0f; // AI��ʱ����
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
	// ����������
	touchListener = EventListenerTouchOneByOne::create();
	// ��仰�����ʲô��˼����
	touchListener->setSwallowTouches(true);

	// ���ʱ��ʾ��ͷ����ת��ͷ���򣬼�¼�����ʼʱ��
	touchListener->onTouchBegan = [this](Touch* touch, Event* e) {
		this->setStartTime();
		this->arrow->setColor(Color3B(255, 255, 255));
		this->isTouch = true;
		my_action->arrowRotation(this->arrow, shootPosition, touch->getLocation());
		this->arrow->setVisible(true);
		return true;
	};

	// ����ƶ�ʱ��ת��ͷ����
	touchListener->onTouchMoved = [this](Touch* touch, Event* e) {
		my_action->arrowRotation(this->arrow, shootPosition, touch->getLocation());
	};

	// ����������ؼ�ͷ
	// �����ڵ�ʵ��
	// ���ݵ������ʱ��ͷ��������ڵ��ķ����ٶ�����
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

	// �Ѵ��ؼ�������ӵ������¼�����
	director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, background);
}

void Level2::contactEvent()
{
	// ������ײ������
	auto listener = EventListenerPhysicsContact::create();

	// ��ײ����ʱ�Ļص�����
	listener->onContactBegin = [this](PhysicsContact& contact) {
		// ��ȡ������ײ����������A,B
		auto A = (Sprite*)contact.getShapeA()->getBody()->getNode();
		auto B = (Sprite*)contact.getShapeB()->getBody()->getNode();

		/*
		Tag 0 ���� ����
		Tag 1 ����ڵ�
		Tag 2 ���Ŀ��
		Tag 3 ���Ͷʯ��
		Tag 4 AI�ڵ�
		Tag 5 AIĿ��
		Tag 6 AIͶʯ��
		*/

		// �ڵ���������򵲰� ���ڵ���ʧ
		if ((A->getTag() == 1 && B->getTag() == 0) || (A->getTag() == 0 && B->getTag() == 1) ||
			(A->getTag() == 4 && B->getTag() == 0) || (A->getTag() == 0 && B->getTag() == 4))
		{
			my_action->playExplosionEffect(); // ��ը��Ч
			if (A->getTag() == 0) {
				my_action->showExplosion(B->getPosition(), this, 5); // ��ըЧ��
				my_action->spriteFadeOut(B);
			}
			else {
				my_action->showExplosion(A->getPosition(), this, 5); // ��ըЧ��
				my_action->spriteFadeOut(A);
			}
			return true;
		}

		// �����ڵ���ײ ����ʧ
		// ������������ͬһ�ߵ��ڵ�
		// ������ըЧ��
		if ((A->getTag() == 1 && B->getTag() == 4) || (A->getTag() == 4 && B->getTag() == 1) ||
			(A->getTag() == 1 && B->getTag() == 1) || (A->getTag() == 4 && B->getTag() == 4))
		{
			my_action->playExplosionEffect(); // ��ը��Ч
			Vec2 posA = A->getPosition();
			Vec2 posB = B->getPosition();
			Vec2 tmp = Vec2((posA.x + posB.x) / 2, (posA.y + posB.y) / 2);
			my_action->showExplosion(tmp, this, 5); // ��ըЧ��

			my_action->spriteFadeOut(A);
			my_action->spriteFadeOut(B);
			return true;
		}

		// ����ڵ�����AIĿ��������Ͷʯ�� ���ڵ���ʧ
		if ((A->getTag() == 1 && B->getTag() == 3) || (A->getTag() == 3 && B->getTag() == 1) ||
			(A->getTag() == 1 && B->getTag() == 5) || (A->getTag() == 5 && B->getTag() == 1))
		{
			my_action->playExplosionEffect(); // ��ը��Ч
			if (A->getTag() == 1) {
				my_action->showExplosion(A->getPosition(), this, 5); // ��ըЧ��
				my_action->spriteFadeOut(A);
			}
			else {
				my_action->showExplosion(B->getPosition(), this, 5); // ��ըЧ��
				my_action->spriteFadeOut(B);
			}
			return true;
		}

		// ����ڵ��������Ŀ�� �ڵ���Ŀ�����ʧ ���ӷ���
		if ((A->getTag() == 1 && B->getTag() == 2) || (A->getTag() == 2 && B->getTag() == 1)) {
			my_action->playGetPointEffect(); // �÷���Ч
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
			my_action->showExplosion(tmp, this, 5); // ��ըЧ��

			my_action->spriteFadeOut(A);
			my_action->spriteFadeOut(B);
			my_action->showPerScore(giftPosition, GIFT_SCORE, this);
			my_action->addScore(this->playScore, GIFT_SCORE);
			my_action->updateLabelScore(scoreLabel, playScore, AIScore, SCORE_FORMAT);
			return true;
		}

		// AI�ڵ�����AIĿ�� �ڵ���Ŀ�궼��ʧ AI��������
		if ((A->getTag() == 4 && B->getTag() == 5) || (A->getTag() == 5 && B->getTag() == 4)) {
			my_action->playGetPointEffect(); // �÷���Ч
			Vec2 giftPosition;
			if (A->getTag() == 4) {
				giftPosition = B->getPosition();
			}
			else {
				giftPosition = A->getPosition();
			}

			// �ѱ����е�Ŀ��λ�ô�AIĿ���б���ɾ��
			for (std::vector<Vec2>::iterator it = AIAllGiftPosition2.begin(); it != AIAllGiftPosition2.end(); it++) {
				if ((*it) == judgeTarget(giftPosition)) {
					AIAllGiftPosition2.erase(it);
					break;
				}
			}

			Vec2 posA = A->getPosition();
			Vec2 posB = B->getPosition();
			Vec2 tmp = Vec2((posA.x + posB.x) / 2, (posA.y + posB.y) / 2);
			my_action->showExplosion(tmp, this, 5); // ��ըЧ��

			my_action->spriteFadeOut(A);
			my_action->spriteFadeOut(B);
			my_action->showPerScore(giftPosition, GIFT_SCORE, this);
			my_action->addScore(this->AIScore, GIFT_SCORE);
			my_action->updateLabelScore(scoreLabel, playScore, AIScore, SCORE_FORMAT);
			return true;
		}

		// ����ڵ�����AIͶʯ�� AIͶʯ��ѣ��
		if ((A->getTag() == 1 && B->getTag() == 6) || (A->getTag() == 6 && B->getTag() == 1)) {
			my_action->playExplosionEffect(); // ��ը��Ч
			if (A->getTag() == 1) {
				my_action->showExplosion(A->getPosition(), this, 5); // ��ըЧ��
				my_action->spriteFadeOut(A);
			}
			else {
				my_action->showExplosion(B->getPosition(), this, 5); // ��ըЧ��
				my_action->spriteFadeOut(B);
			}
			// AI ֹͣ����2s
			if (tempAIshootTime2 > AIshootTime_level2 - 1) tempAIshootTime2 = AIshootTime_level2 - 1;
			tempAIshootTime2 -= DIZZY_TIME;
			auto dizzy = my_action->createSprite("dizzy.png", 7, AIdizzyPosition);
			my_action->addNode(this, dizzy, 5);
			my_action->showDizzyPic(dizzy, DIZZY_TIME);

			return true;
		}

		// AI�ڵ��������Ͷʯ�� ���Ͷʯ��ѣ��
		if ((A->getTag() == 3 && B->getTag() == 4) || (A->getTag() == 4 && B->getTag() == 3)) {
			my_action->playExplosionEffect(); // ��ը��Ч
			if (A->getTag() == 4) {
				my_action->showExplosion(A->getPosition(), this, 5); // ��ըЧ��
				my_action->spriteFadeOut(A);
			}
			else {
				my_action->showExplosion(B->getPosition(), this, 5); // ��ըЧ��
				my_action->spriteFadeOut(B);
			}
			/*
			play stop shoot 2 second
			*/
			isHit = true;
			// ��ʼ����ѣ��ʱ��
			tempPlayerDizzyTime2 = 0.0f;
			this->arrow->setVisible(false);
			this->arrow->setColor(Color3B(255, 255, 255));
			// ѣ��Ч��
			auto dizzy = my_action->createSprite("dizzy.png", 7, playerDizzyPosition);
			my_action->addNode(this, dizzy, 5);
			my_action->showDizzyPic(dizzy, DIZZY_TIME);
			// �Ƴ�������
			director->getEventDispatcher()->removeEventListenersForTarget(this->background);

			return true;
		}

		// AI�ڵ��������Ŀ���AIͶʯ�� ���ڵ���ʧ
		if ((A->getTag() == 4 && B->getTag() == 2) || (A->getTag() == 2 && B->getTag() == 4) ||
			(A->getTag() == 4 && B->getTag() == 6) || (A->getTag() == 6 && B->getTag() == 4))
		{
			my_action->playExplosionEffect(); // ��ը��Ч
			if (A->getTag() == 4) {
				my_action->showExplosion(A->getPosition(), this, 5); // ��ըЧ��
				my_action->spriteFadeOut(A);
			}
			else {
				my_action->showExplosion(B->getPosition(), this, 5); // ��ըЧ��
				my_action->spriteFadeOut(B);
			}
			return true;
		}

		return true;
	};

	// ����ײ��������ӵ������¼�����
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
