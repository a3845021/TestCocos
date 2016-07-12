#include "HelpScene.h"
#include "start.h"
#include "MyAction.h"

#define my_action MyAction::getInstance()

cocos2d::Scene * HelpScene::createScene()
{
	auto scene = Scene::create();
	auto layer = HelpScene::create();
	scene->addChild(layer);
	return scene;
}

bool HelpScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();

	/*
	增加背景图
	帮助界面的文字
	cocos2dx好像不支持中文的label。
	*/

	auto back = MenuItemLabel::create(Label::createWithTTF("Back", "fonts/Marker Felt.ttf", 36));
	back->setCallback([](Ref* ref) {
		my_action->changeScene(Start::createScene());
	});
	back->setColor(Color3B::WHITE);
	auto backMenu = Menu::create(back, NULL);
	backMenu->setPosition(back->getContentSize().width, visibleSize.height - back->getContentSize().height);
	this->addChild(backMenu, 5);

	/*auto label = Label::createWithTTF("Help Scene", "fonts/arial.ttf", 60);
	// label->setString(my_action->getChinese("chineseXML/HelpScene.xml", "HelpTitle"));
	label->setPosition(visibleSize.width / 2, visibleSize.height * 5 / 6);
	this->addChild(label);*/

	help1 = Sprite::create("help1.png");
	help1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(help1, 2);

	help2 = Sprite::create("help2.png");
	help2->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	help2->setVisible(false);
	this->addChild(help2, 2);

	front = MenuItemLabel::create(Label::createWithTTF("Front", "fonts/Marker Felt.ttf", 28));
	front->setCallback([&](Ref* ref) {
		front->setVisible(false);
		help2->setVisible(false);
		next->setVisible(true);
		help1->setVisible(true);
	});
	next = MenuItemLabel::create(Label::createWithTTF("Next", "fonts/Marker Felt.ttf", 28));
	next->setCallback([&](Ref* ref) {
		front->setVisible(true);
		help2->setVisible(true);
		next->setVisible(false);
		help1->setVisible(false);
	});
	auto menu = Menu::create(front, next, NULL);
	menu->setColor(Color3B::WHITE);
	menu->alignItemsHorizontallyWithPadding(50.0f);
	front->setVisible(false);
	menu->setPosition(visibleSize.width / 2, next->getContentSize().height);
	this->addChild(menu, 5);

	return true;
}
