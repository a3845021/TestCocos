#include "AboutScene.h"
#include "start.h"
#include "MyAction.h"

#define my_action MyAction::getInstance()

cocos2d::Scene * AboutScene::createScene()
{
	auto scene = Scene::create();
	auto layer = AboutScene::create();
	scene->addChild(layer);
	return scene;
}

bool AboutScene::init()
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

	auto label = Label::createWithTTF("About Scene", "fonts/Marker Felt.ttf", 60);
	label->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(label);

	/*auto shooter = Sprite::create("shooter.png");
	Animate* shooterAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("playerAnimation"));
	shooter->runAction(RepeatForever::create(shooterAnimate));
	shooter->setAnchorPoint(Vec2(0, 0));
	shooter->setPosition(Vec2(100, 35));
	this->addChild(shooter, 2);

	auto ball = Sprite::create("bullet.png");
	ball->setPosition(Vec2(150, 150));
	this->addChild(ball, 2);*/

	return true;
}
