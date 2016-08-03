#include "GamePause.h"
#include "MyAction.h"
#include "test.h"
#include "SelectScene.h"
#include "start.h"
#include "SimpleAudioEngine.h"
#include "res.h"
#include "Level0.h"
#include "Level2.h"

using namespace CocosDenshion;

#define audio SimpleAudioEngine::getInstance()
#define director Director::getInstance()
#define my_action MyAction::getInstance()
#define TEXT_FONT "fonts/shaonvxin.ttf"

MenuItemImage *playBGM, *playEffect, *notPlayBGM, *notPlayEffect;

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

	// 保留原画面变灰
	auto sprite = Sprite::createWithTexture(sqr->getSprite()->getTexture());
	sprite->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	sprite->setFlipY(true);
	sprite->setColor(Color3B::GRAY);
	scene->addChild(sprite, 0);

	auto bg = Sprite::create("pause.png");
	bg->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	scene->addChild(bg, 1);

	auto pause = Label::createWithTTF(my_action->getChinese("chineseXML/pause.xml", "Pause"), TEXT_FONT, 56);
	pause->setPosition(visibleSize.width / 2, visibleSize.height / 2 + pause->getContentSize().height * 1.5);
	scene->addChild(pause, 1);

	auto continue_game = MenuItemLabel::create(Label::createWithTTF(my_action->getChinese("chineseXML/pause.xml", "Continue"), TEXT_FONT, 36));
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

	auto restart_game = MenuItemLabel::create(Label::createWithTTF(my_action->getChinese("chineseXML/pause.xml", "Restart"), TEXT_FONT, 36));
	restart_game->setCallback([&](Ref* ref) {
		// 重新开始游戏
		if (game_level == 0) {
			my_action->changeScene(Level0::createScene());
		}
		if (game_level == 1) {
			// 确定目前在玩的关卡号
			my_action->changeScene(Test::createScene());
		}
		if (game_level == 2) {
			my_action->changeScene(Level2::createScene());
		}
	});
	restart_game->setColor(Color3B::RED);

	auto backmenu = MenuItemLabel::create(Label::createWithTTF(my_action->getChinese("chineseXML/pause.xml", "BackToMenu"), TEXT_FONT, 28));
	backmenu->setCallback([](Ref* ref) {
		// 开始页面
		my_action->changeScene(Start::createScene());
	});

	auto menu = Menu::create(continue_game, select, NULL);
	menu->setPosition(visibleSize.width * 0.4, visibleSize.height / 2 - 80);
	menu->alignItemsVerticallyWithPadding(20);
	scene->addChild(menu, 1);

	auto menu2 = Menu::create(restart_game, backmenu, NULL);
	menu2->setPosition(visibleSize.width * 0.6, visibleSize.height / 2 - 80);
	menu2->alignItemsVerticallyWithPadding(20);
	scene->addChild(menu2, 1);

	// 打开关闭背景音乐或音效
	playBGM = MenuItemImage::create("BGMplay.png", "BGMplay.png");
	playBGM->setCallback([&](Ref* ref) {
		playBGM->setVisible(false);
		notPlayBGM->setVisible(true);
		playBGM_extern = false;
		audio->pauseBackgroundMusic();
	});

	playEffect = MenuItemImage::create("EffectPlay.png", "EffectPlay.png");
	playEffect->setCallback([&](Ref* ref) {
		playEffect->setVisible(false);
		notPlayEffect->setVisible(true);
		playEffect_extern = false;
	});

	auto menu11 = Menu::create(playBGM, playEffect, NULL);
	menu11->alignItemsHorizontallyWithPadding(30);
	menu11->setPosition(visibleSize.width / 2, visibleSize.height / 2);

	notPlayBGM = MenuItemImage::create("BGMnotplay.png", "BGMnotplay.png");
	notPlayBGM->setCallback([&](Ref* ref) {
		playBGM->setVisible(true);
		notPlayBGM->setVisible(false);
		playBGM_extern = true;
		audio->resumeBackgroundMusic();
	});

	notPlayEffect = MenuItemImage::create("EffectNotPlay.png", "EffectNotPlay.png");
	notPlayEffect->setCallback([&](Ref* ref) {
		playEffect->setVisible(true);
		notPlayEffect->setVisible(false);
		playEffect_extern = true;
	});

	auto menu22 = Menu::create(notPlayBGM, notPlayEffect, NULL);
	menu22->alignItemsHorizontallyWithPadding(30);
	menu22->setPosition(visibleSize.width / 2, visibleSize.height / 2);

	if (playBGM_extern) {
		playBGM->setVisible(true);
		notPlayBGM->setVisible(false);
	}
	else {
		playBGM->setVisible(false);
		notPlayBGM->setVisible(true);
	}

	if (playEffect_extern) {
		playEffect->setVisible(true);
		notPlayEffect->setVisible(false);
	}
	else {
		playEffect->setVisible(false);
		notPlayEffect->setVisible(true);
	}

	scene->addChild(menu11, 1);
    scene->addChild(menu22, 1);

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
