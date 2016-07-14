#include "GamePause.h"
#include "MyAction.h"
#include "test.h"
#include "SelectScene.h"
#include "start.h"

#define director Director::getInstance()
#define my_action MyAction::getInstance()
#define TEXT_FONT "fonts/shaonvxin.ttf"

cocos2d::Scene * GamePause::createScene()
{
	auto scene = Scene::create();
	auto layer = GamePause::create();
	scene->addChild(layer);
	return scene;
}

cocos2d::Scene * GamePause::createScene(RenderTexture * sqr)
{
	auto scene = Scene::create();
	auto layer = GamePause::create();
	scene->addChild(layer, 0);

	auto visibleSize = director->getVisibleSize();

	auto sprite = Sprite::createWithTexture(sqr->getSprite()->getTexture());
	sprite->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	sprite->setFlipY(true);
	sprite->setColor(Color3B::GRAY);
	scene->addChild(sprite, 0);

	auto bg = Sprite::create("pause.png");
	bg->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	scene->addChild(bg, 1);

	auto pause = Label::createWithTTF(my_action->getChinese("chineseXML/pause.xml", "Pause"), TEXT_FONT, 40);
	pause->setPosition(visibleSize.width / 2, visibleSize.height / 2 + pause->getContentSize().height * 1.5);
	scene->addChild(pause, 1);

	auto continue_game = MenuItemLabel::create(Label::createWithTTF(my_action->getChinese("chineseXML/pause.xml", "Continue"), TEXT_FONT, 28));
	continue_game->setCallback([](Ref* ref) {
		// 继续游戏
		director->popScene();
	});
	continue_game->setColor(Color3B::RED);

	auto select = MenuItemLabel::create(Label::createWithTTF(my_action->getChinese("chineseXML/pause.xml", "Reselect"), TEXT_FONT, 28));
	select->setCallback([](Ref* ref) {
		// 选关页面
		my_action->changeScene(SelectScene::createScene());
	});

	auto restart_game = MenuItemLabel::create(Label::createWithTTF(my_action->getChinese("chineseXML/pause.xml", "Restart"), TEXT_FONT, 28));
	restart_game->setCallback([](Ref* ref) {
		// 重新开始游戏
		my_action->changeScene(Test::createScene());
	});
	restart_game->setColor(Color3B::RED);

	auto backmenu = MenuItemLabel::create(Label::createWithTTF(my_action->getChinese("chineseXML/pause.xml", "BackToMenu"), TEXT_FONT, 28));
	backmenu->setCallback([](Ref* ref) {
		// 开始页面
		my_action->changeScene(Start::createScene());
	});

	auto menu = Menu::create(continue_game, select, NULL);
	menu->setPosition(visibleSize.width * 0.4, visibleSize.height / 2 - 40);
	menu->alignItemsVerticallyWithPadding(20);
	scene->addChild(menu, 1);

	auto menu2 = Menu::create(restart_game, backmenu, NULL);
	menu2->setPosition(visibleSize.width * 0.6, visibleSize.height / 2 - 40);
	menu2->alignItemsVerticallyWithPadding(20);
	scene->addChild(menu2, 1);

	return scene;
}

bool GamePause::init()
{
	if (!Layer::init())
	{
		return false;
	}
	return true;
}
