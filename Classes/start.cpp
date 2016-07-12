#include "start.h"
#include "SelectScene.h"
#include "test.h"
#include "MyAction.h"
#include "HelpScene.h"
#include "AboutScene.h"
#include "SettingScene.h"

#define my_action MyAction::getInstance()

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
	auto visibleSize = Director::getInstance()->getVisibleSize();
	
	/*
	Ìí¼Ó±³¾°Í¼
	auto background = Sprite::create("bg.jpg");
	background->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(background, 0);
	//background->setVisible(false);
	*/

	auto label = Label::createWithTTF("Catapult Game", "fonts/Marker Felt.ttf", 48);
	label->setAnchorPoint(Vec2(0.5, 1));
	label->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - label->getContentSize().height / 2));
	this->addChild(label, 1);

	auto sprite = Sprite::create("shooter.png");
	sprite->setScale(1.5f);
	sprite->setPosition(visibleSize.width / 4, visibleSize.height / 2);
	this->addChild(sprite, 1);

	auto AIsprite = Sprite::create("AIshooter.png");
	AIsprite->setScale(1.5f);
	AIsprite->setPosition(visibleSize.width / 4 * 3, visibleSize.height / 2);
	this->addChild(AIsprite, 1);

	auto item = MenuItemLabel::create(Label::createWithTTF("Play", "fonts/Marker Felt.ttf", 80));
	item->setCallback([](Ref* ref) {
		my_action->changeScene(SelectScene::createScene());
	});
	item->setColor(Color3B::RED);

	auto settingItem = MenuItemLabel::create(Label::createWithTTF("Setting", "fonts/Marker Felt.ttf", 40));
	settingItem->setCallback([](Ref* ref) {
		// ÉèÖÃÒ³Ãæ
		my_action->changeScene(SettingScene::createScene());
	});

	auto helpItem = MenuItemLabel::create(Label::createWithTTF("Help", "fonts/Marker Felt.ttf", 40));
	helpItem->setCallback([](Ref* ref) {
		// °ïÖúÒ³Ãæ
		my_action->changeScene(HelpScene::createScene());
	});

	auto aboutItem = MenuItemLabel::create(Label::createWithTTF("About", "fonts/Marker Felt.ttf", 40));
	aboutItem->setCallback([](Ref* ref) {
		// ¹ØÓÚÒ³Ãæ
		my_action->changeScene(AboutScene::createScene());
	});

	auto quitItem = MenuItemLabel::create(Label::createWithTTF("Quit", "fonts/Marker Felt.ttf", 40));
	quitItem->setCallback([](Ref* ref) {
		Director::getInstance()->end();
	});

	auto menu = Menu::create(item, settingItem, helpItem, aboutItem, quitItem, NULL);
	menu->setPosition(visibleSize.width / 2, visibleSize.height / 2.5);
	menu->alignItemsVerticallyWithPadding(10);
	this->addChild(menu, 1);

	return true;
}
