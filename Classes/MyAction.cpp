#include "MyAction.h"

#define director Director::getInstance()
#define PI 3.14159265 // 圆周率
#define MAX_TOUCH_TIME 2.0f // 触控最大时间，控制最大速度
#define MAX_UINT 0xFFFFFFFF

/*
更换场景
*/
void MyAction::changeScene(cocos2d::Scene * scene)
{
	director->replaceScene(scene);
}

/*
创建一个Node
*/
cocos2d::Node * MyAction::createNode(int tag, cocos2d::PhysicsBody * physics_body, bool dynamic)
{
	auto node = Node::create();
	node->setTag(tag);
	if (physics_body != NULL) {
		node->setPhysicsBody(physics_body);
		node->getPhysicsBody()->setContactTestBitmask(MAX_UINT);
		node->getPhysicsBody()->setDynamic(dynamic);
	}
	return node;
}

/*
创建一个Sprite(不可设置AnchorPoint)
*/
cocos2d::Sprite * MyAction::createSprite(std::string file_name, int tag, Vec2 position, cocos2d::PhysicsBody * physics_body, bool dynamic)
{
	auto sprite = Sprite::create(file_name);
	sprite->setTag(tag);
	sprite->setPosition(position);
	if (physics_body != NULL) {
		sprite->setPhysicsBody(physics_body);
		sprite->getPhysicsBody()->setContactTestBitmask(MAX_UINT);
		sprite->getPhysicsBody()->setDynamic(dynamic);
	}
	return sprite;
}

/*
创建一个Sprite(可设置AnchorPoint)
*/
cocos2d::Sprite * MyAction::createSprite(std::string file_name, int tag, Vec2 position, Vec2 anchorPoint, cocos2d::PhysicsBody * physics_body, bool dynamic)
{
	auto sprite = Sprite::create(file_name);
	sprite->setTag(tag);
	sprite->setAnchorPoint(anchorPoint);
	sprite->setPosition(position);
	if (physics_body != NULL) {
		sprite->setPhysicsBody(physics_body);
		sprite->getPhysicsBody()->setContactTestBitmask(MAX_UINT);
		sprite->getPhysicsBody()->setDynamic(dynamic);
	}
	return sprite;
}

/*
添加一个Node(Sprite, Label, Menu, MenuItemLabel...)到Layer里
*/
void MyAction::addNode(cocos2d::Layer * layer, cocos2d::Node * node, int localZOrder)
{
	layer->addChild(node, localZOrder);
}

/*
player发射以shootVelocity的速度发射bullet
*/
void MyAction::shootAction(cocos2d::Layer * layer, Vec2 shootVelocity, cocos2d::Sprite * bullet, int localZOrder)
{
	bullet->getPhysicsBody()->setVelocity(shootVelocity);
	layer->addChild(bullet, localZOrder);
}

/*
计算AI发射Velocity
*/
cocos2d::Vec2 MyAction::calAIShootVelocity(cocos2d::Vec2 shootPos, cocos2d::Vec2 targetPos, float g)
{
	float t = sqrt(2 * (shootPos.x + shootPos.y - targetPos.x - targetPos.y) / g);
	float vx = (shootPos.x - targetPos.x) / t;
	float vy = vx;
	return Vec2(-vx, vy);
}

/*
计算player发射Velocity
*/
cocos2d::Vec2 MyAction::calPlayerShootVelocity(cocos2d::Vec2 shootPos, cocos2d::Vec2 targetPos, int init_speed, float touch_time)
{
	if (touch_time > MAX_TOUCH_TIME) touch_time = MAX_TOUCH_TIME;
	float a = targetPos.x - shootPos.x;
	float b = targetPos.y - shootPos.y;
	float c = sqrt(a * a + b * b);
	return Vec2(init_speed * touch_time / c * a, init_speed * touch_time / c * b);
}

/*
移除一个Sprite(包含0.1s淡出效果)
*/
void MyAction::spriteFadeOut(cocos2d::Sprite * sprite)
{
	// 设置碰撞不检测  否则在淡出时间内也能触发碰撞事件
	sprite->getPhysicsBody()->setContactTestBitmask(0x00000000);
	// 设置淡出时间
	auto fadeOut = FadeOut::create(0.1f);
	// 让精灵执行一串事件（淡出，移除） 不移除只是变透明。。
	sprite->runAction(Sequence::create(fadeOut,
		CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sprite)), NULL));
}

/*
增加分数
score += add_score
*/
void MyAction::addScore(int & score, const int add_score)
{
	score += add_score;
}

/*
更新分数面板
*/
void MyAction::updateLabelScore(cocos2d::Label * score_label, int player_score, int AI_score, char * label_format)
{
	char t[30];
	sprintf(t, label_format, player_score, AI_score);
	score_label->setString(t);
}

/*
击中目标后显示得分，1s后消失
*/
void MyAction::showPerScore(cocos2d::Vec2 pos, int score, cocos2d::Layer* layer)
{
	char t[30];
	sprintf(t, "%d", score);
	auto label = Label::createWithTTF(t, "fonts/Marker Felt.ttf", 48);
	label->setColor(Color3B::RED);
	label->setPosition(pos);
	layer->addChild(label, 2);
	auto fadeIn = FadeIn::create(0.2f);
	auto fadeOut = FadeOut::create(0.8f);
	label->runAction(Sequence::create(fadeIn, fadeOut,
		CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, label)), NULL));
}

/*
箭头旋转(初始方向为箭头向右)
*/
void MyAction::arrowRotation(cocos2d::Sprite * arrow, Vec2 shootPos, Vec2 mousePos)
{
	float a = mousePos.x - shootPos.x;
	float b = mousePos.y - shootPos.y;
	float rotation;
	if (a < 0) rotation = -180 / PI * atan(b / a) + 180;
	else rotation = -180 / PI * atan(b / a);
	arrow->setRotation(rotation);
}

/*
改变箭头颜色
*/
void MyAction::arrowColor(cocos2d::Sprite * arrow, float touchTime)
{
	if (touchTime > 2) touchTime = 2;
	int temp = (int)(touchTime * 100);
	arrow->setColor(Color3B(255 - temp, 255 - temp, 255 - temp));
}

/*
眩晕效果
*/
void MyAction::showDizzyPic(cocos2d::Sprite * sprite, float dizzy_time)
{
	auto rotateBy = RotateBy::create(dizzy_time, 1440);
	sprite->runAction(Sequence::create(rotateBy,
		CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sprite)), NULL));
}
