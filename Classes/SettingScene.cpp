#include "SettingScene.h"
#include "start.h"
#include "MyAction.h"
#include "SimpleAudioEngine.h"
#include "res.h"

using namespace CocosDenshion;

#define audio SimpleAudioEngine::getInstance()
#define my_action MyAction::getInstance()
#define database UserDefault::getInstance() // ±¾µØ´æ´¢ÊµÀı

cocos2d::Scene * SettingScene::createScene()
{
	auto scene = Scene::create();
	auto layer = SettingScene::create();
	scene->addChild(layer);
	return scene;
}

bool SettingScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();

	auto back = MenuItemLabel::create(Label::createWithTTF(my_action->getChinese("chineseXML/start.xml", "Back"), "fonts/shaonvxin.ttf", 36));
	back->setCallback([](Ref* ref) {
		my_action->changeScene(Start::createScene());
	});
	back->setColor(Color3B::WHITE);
	auto backMenu = Menu::create(back, NULL);
	backMenu->setPosition(back->getContentSize().width, visibleSize.height - back->getContentSize().height);
	this->addChild(backMenu, 5);

	auto background = Sprite::create("start_bg.jpg");
	background->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(background, 0);

	auto bg = Sprite::create("setting.png");
	bg->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(bg, 0);

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

	auto menu1 = Menu::create(playBGM, playEffect, NULL);
	menu1->alignItemsHorizontallyWithPadding(30);
	menu1->setPosition(visibleSize.width / 2, visibleSize.height / 4);

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

	auto menu2 = Menu::create(notPlayBGM, notPlayEffect, NULL);
	menu2->alignItemsHorizontallyWithPadding(30);
	menu2->setPosition(visibleSize.width / 2, visibleSize.height / 4);
	
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

	this->addChild(menu1, 1);
	this->addChild(menu2, 1);

	return true;
}
