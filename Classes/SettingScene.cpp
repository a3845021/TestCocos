#include "SettingScene.h"
#include "start.h"
#include "MyAction.h"

#define my_action MyAction::getInstance()

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

	/*auto label = Label::createWithTTF("Setting Scene", "fonts/Marker Felt.ttf", 60);
	label->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(label);*/

	//auto text = my_action->getChinese("chineseXML/setting.xml", "BGM");
	auto label = Label::create(my_action->getChinese("chineseXML/setting.xml", "BGM"), "fonts/shaonvxin.ttf", 60);
	label->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(label);

	/*auto pe = ParticleExplosion::createWithTotalParticles(100);
	pe->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	pe->setLife(0.3f);
	pe->setLifeVar(0.1f);
	//auto emitter = ParticleFireworks::create();

	this->addChild(pe, 2);*/

	return true;
}
