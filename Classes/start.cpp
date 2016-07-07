#include "start.h"
#include "test.h"
#include "MyAction.h"

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

	auto background = Sprite::create("bg.jpg");
	background->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(background, 0);
	background->setVisible(false);

	auto label = Label::createWithTTF("Start", "fonts/Marker Felt.ttf", 48);
	label->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - label->getContentSize().height));
	this->addChild(label, 1);

	auto item = MenuItemLabel::create(Label::createWithTTF("Test", "fonts/Marker Felt.ttf", 80));
	item->setCallback([](Ref* ref) {
		my_action->changeScene(Test::createScene());
	});
	auto menu = Menu::create(item, NULL);
	menu->setColor(Color3B::RED);
	menu->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	menu->alignItemsVerticallyWithPadding(10);
	this->addChild(menu, 1);

	return true;
}

void Start::goTest(Ref * ref)
{
	auto scene = Test::createScene();
	Director::getInstance()->replaceScene(scene);
}
