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

	auto back = MenuItemLabel::create(Label::createWithTTF(my_action->getChinese("chineseXML/start.xml", "Back"), "fonts/shaonvxin.ttf", 36));
	back->setCallback([](Ref* ref) {
		my_action->changeScene(Start::createScene());
	});
	back->setColor(Color3B::WHITE);
	auto backMenu = Menu::create(back, NULL);
	backMenu->setPosition(back->getContentSize().width, visibleSize.height - back->getContentSize().height);
	this->addChild(backMenu, 5);

	auto about = Sprite::create("about.png");
	about->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(about, 2);

	auto version = Label::createWithTTF(my_action->getChinese("chineseXML/about.xml", "Version"), "fonts/shaonvxin.ttf", 36);
	version->setPosition(visibleSize.width / 2, visibleSize.height / 3);
	this->addChild(version, 2);

	auto updateTime = Label::createWithTTF(my_action->getChinese("chineseXML/about.xml", "Update"), "fonts/shaonvxin.ttf", 36);
	updateTime->setPosition(visibleSize.width / 2, visibleSize.height / 3 - version->getContentSize().height);
	this->addChild(updateTime, 2);

	/*auto copyright = Label::createWithTTF(my_action->getChinese("chineseXML/about.xml", "Copyright"), "fonts/shaonvxin.ttf", 18);
	copyright->setPosition(visibleSize.width / 2, copyright->getContentSize().height);
	this->addChild(copyright, 2);*/

	return true;
}
