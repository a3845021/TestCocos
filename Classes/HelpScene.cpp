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

	auto back = MenuItemLabel::create(Label::createWithTTF("Back", "fonts/Marker Felt.ttf", 36));
	back->setCallback([](Ref* ref) {
		my_action->changeScene(Start::createScene());
	});
	back->setColor(Color3B::WHITE);
	auto backMenu = Menu::create(back, NULL);
	backMenu->setPosition(back->getContentSize().width, visibleSize.height - back->getContentSize().height);
	this->addChild(backMenu, 1);

	auto label = Label::createWithTTF("Help Scene", "fonts/Marker Felt.ttf", 60);
	label->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(label);

	return true;
}
