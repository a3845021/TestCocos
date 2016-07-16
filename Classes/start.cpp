#include "start.h"
#include "SelectScene.h"
#include "test.h"
#include "MyAction.h"
#include "HelpScene.h"
#include "AboutScene.h"
#include "SettingScene.h"
#include "SimpleAudioEngine.h"
#include "res.h"

using namespace CocosDenshion;

#define my_action MyAction::getInstance()
#define TEXT_FONT "fonts/shaonvxin.ttf"

cocos2d::Scene * Start::createScene()
{
	auto scene = Scene::create();
	auto layer = Start::create();
	scene->addChild(layer);
	return scene;
}

bool Start::init()
{
	if (!Layer::init())
	{
		return false;
	}

	// 播放bgm
	/*if (playBGM_extern && (!SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())) {
		SimpleAudioEngine::getInstance()->playBackgroundMusic("bgm/bgm_huanqin.mp3", true);
	}*/

	auto visibleSize = Director::getInstance()->getVisibleSize();
	
	/*
	添加背景图
	auto background = Sprite::create("bg.jpg");
	background->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(background, 0);
	//background->setVisible(false);
	*/

	auto label = Label::createWithTTF("Catapult Game", TEXT_FONT, 48);
	label->setAnchorPoint(Vec2(0.5, 1));
	label->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - label->getContentSize().height / 2));
	this->addChild(label, 1);

	auto sprite = Sprite::create("shooter.png");
	sprite->setScale(1.5f);
	sprite->setPosition(visibleSize.width / 5, visibleSize.height / 2);
	this->addChild(sprite, 1);

	auto AIsprite = Sprite::create("AIshooter.png");
	AIsprite->setScale(1.5f);
	AIsprite->setPosition(visibleSize.width / 5 * 4, visibleSize.height / 2);
	this->addChild(AIsprite, 1);

	auto item = MenuItemLabel::create(Label::createWithTTF(my_action->getChinese("chineseXML/start.xml", "Play"), TEXT_FONT, 60));
	item->setCallback([](Ref* ref) {
		// 选关页面
		my_action->changeScene(SelectScene::createScene());
	});
	item->setColor(Color3B::RED);

	auto settingItem = MenuItemLabel::create(Label::createWithTTF(my_action->getChinese("chineseXML/start.xml", "Setting"), TEXT_FONT, 40));
	settingItem->setCallback([](Ref* ref) {
		// 设置页面
		my_action->changeScene(SettingScene::createScene());
	});

	auto helpItem = MenuItemLabel::create(Label::createWithTTF(my_action->getChinese("chineseXML/start.xml", "Help"), TEXT_FONT, 40));
	helpItem->setCallback([](Ref* ref) {
		// 帮助页面
		my_action->changeScene(HelpScene::createScene());
	});

	auto aboutItem = MenuItemLabel::create(Label::createWithTTF(my_action->getChinese("chineseXML/start.xml", "About"), TEXT_FONT, 40));
	aboutItem->setCallback([](Ref* ref) {
		// 关于页面
		my_action->changeScene(AboutScene::createScene());
	});

	auto quitItem = MenuItemLabel::create(Label::createWithTTF(my_action->getChinese("chineseXML/start.xml", "Quit"), TEXT_FONT, 40));
	quitItem->setCallback([](Ref* ref) {
		// 退出游戏
		Director::getInstance()->end();
	});

	auto menu = Menu::create(item, settingItem, helpItem, aboutItem, quitItem, NULL);
	menu->setPosition(visibleSize.width / 2, visibleSize.height / 3);
	menu->alignItemsVerticallyWithPadding(10);
	this->addChild(menu, 1);

	return true;
}
