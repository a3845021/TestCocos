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

	auto continue_game = MenuItemLabel::create(Label::createWithTTF("Continue", TEXT_FONT, 40));
	continue_game->setCallback([](Ref* ref) {
		// 继续游戏
		director->popScene();
	});
	continue_game->setColor(Color3B::RED);

	auto select = MenuItemLabel::create(Label::createWithTTF("Reselect", TEXT_FONT, 40));
	select->setCallback([](Ref* ref) {
		// 选关页面
		my_action->changeScene(SelectScene::createScene());
	});

	auto restart_game = MenuItemLabel::create(Label::createWithTTF("Restart", TEXT_FONT, 40));
	restart_game->setCallback([](Ref* ref) {
		// 继续游戏
		director->popScene();
	});
	restart_game->setColor(Color3B::RED);

	auto backmenu = MenuItemLabel::create(Label::createWithTTF("Back To Menu", TEXT_FONT, 40));
	backmenu->setCallback([](Ref* ref) {
		// 选关页面
		my_action->changeScene(SelectScene::createScene());
	});

	auto menu = Menu::create(continue_game, select, NULL);
	menu->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	menu->alignItemsVerticallyWithPadding(10);
	scene->addChild(menu, 1);

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
