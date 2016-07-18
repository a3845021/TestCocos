#include "SelectScene.h"
#include "test.h"
#include "MyAction.h"
#include "start.h"

#define my_action MyAction::getInstance()

cocos2d::Scene * SelectScene::createScene()
{
	auto scene = Scene::create();
	auto layer = SelectScene::create();
	scene->addChild(layer);
	return scene;
}

bool SelectScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();

	/*
	Ìí¼Ó±³¾°Í¼¡£
	*/
	auto background = Sprite::create("other_bg.jpg");
	background->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(background, 0);

	auto bg = Sprite::create("select.png");
	bg->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(bg, 1);

	auto image1 = MenuItemImage::create("simple.png", "simple_selected.png");
	image1->setCallback([](Ref* ref) {
		my_action->changeScene(Test::createScene());
	});
	auto image2 = MenuItemImage::create("medium.png", "medium_selected.png");
	image2->setCallback([](Ref* ref) {
		// my_action->changeScene(::createScene());
	});
	auto image3 = MenuItemImage::create("hard.png", "hard_selected.png");
	image3->setCallback([](Ref* ref) {
		// my_action->changeScene(::createScene());
	});

	auto menu = Menu::create(image1, image2, image3, NULL);
	menu->setOpacity(200);
	menu->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	menu->alignItemsHorizontallyWithPadding(30);
	this->addChild(menu, 1);

	auto back = MenuItemLabel::create(Label::createWithTTF(my_action->getChinese("chineseXML/start.xml", "Back"), "fonts/shaonvxin.ttf", 36));
	back->setCallback([](Ref* ref) {
		my_action->changeScene(Start::createScene());
	});
	back->setColor(Color3B::WHITE);
	auto backMenu = Menu::create(back, NULL);
	backMenu->setPosition(back->getContentSize().width, visibleSize.height - back->getContentSize().height);
	this->addChild(backMenu, 1);

	return true;
}
