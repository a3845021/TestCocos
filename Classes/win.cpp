#include "win.h"
#include "start.h"
#include "test.h"
#include "SelectScene.h"
#include "MyAction.h"

USING_NS_CC;

#define my_action MyAction::getInstance()
#define database UserDefault::getInstance()
#define TEXT_FONT "fonts/shaonvxin.ttf"

cocos2d::Scene * Win::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Win::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool Win::init()
{
	if (!Layer::init())
	{
		return false;
	}

	isWin = database->getBoolForKey("isWin");

	auto visibleSize = Director::getInstance()->getVisibleSize();

	/*
	Ìí¼Ó±³¾°Í¼
	*/

	auto label = Label::createWithTTF(my_action->getChinese("chineseXML/win.xml", "Win"), TEXT_FONT, 80);
	if (!isWin) label->setString(my_action->getChinese("chineseXML/win.xml", "Lose"));
	label->setPosition(visibleSize.width / 2, visibleSize.height - label->getContentSize().height);
	label->setColor(Color3B::RED);
	this->addChild(label, 1);

	auto item1 = MenuItemLabel::create(Label::createWithTTF(my_action->getChinese("chineseXML/win.xml", "Restart"), TEXT_FONT, 48));
	item1->setCallback([](Ref* ref) {
		my_action->changeScene(Test::createScene());
	});
	item1->setColor(Color3B::RED);
	auto item2 = MenuItemLabel::create(Label::createWithTTF(my_action->getChinese("chineseXML/win.xml", "Main"), TEXT_FONT, 36));
	item2->setCallback([](Ref* ref) {
		my_action->changeScene(Start::createScene());
	});
	auto item3 = MenuItemLabel::create(Label::createWithTTF(my_action->getChinese("chineseXML/win.xml", "Select"), TEXT_FONT, 36));
	item3->setCallback([](Ref* ref) {
		my_action->changeScene(SelectScene::createScene());
	});
	auto menu = Menu::create(item1, item3, item2, NULL);
	menu->setPosition(visibleSize.width / 2, visibleSize.height / 3);
	menu->alignItemsVerticallyWithPadding(20);
	this->addChild(menu, 1);

	return true;
}
