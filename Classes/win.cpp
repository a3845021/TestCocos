#include "win.h"
#include "start.h"
#include "test.h"

USING_NS_CC;

#define database UserDefault::getInstance()

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

	auto label = Label::createWithTTF("You Win!!", "fonts/Marker Felt.ttf", 60);
	if (!isWin) label->setString("You Lose..");
	label->setPosition(visibleSize.width / 2, visibleSize.height - label->getContentSize().height);
	label->setColor(Color3B::RED);
	this->addChild(label, 1);

	auto item1 = MenuItemLabel::create(Label::createWithTTF("Restart", "fonts/Marker Felt.ttf", 48), CC_CALLBACK_1(Win::restart, this));
	item1->setColor(Color3B::RED);
	auto item2 = MenuItemLabel::create(Label::createWithTTF("Back To Menu", "fonts/Marker Felt.ttf", 36), CC_CALLBACK_1(Win::backToMenu, this));
	auto menu = Menu::create(item1, item2, NULL);
	menu->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	menu->alignItemsVerticallyWithPadding(20);
	this->addChild(menu, 1);

	return true;
}

void Win::backToMenu(Ref * ref)
{
	auto scene = Start::createScene();
	Director::getInstance()->replaceScene(scene);
}

void Win::restart(Ref * ref)
{
	auto scene = Test::createScene();
	Director::getInstance()->replaceScene(scene);
}
