#ifndef __MY_ACTION_H__
#define __MY_ACTION_H__

#include "cocos2d.h"

USING_NS_CC;

class MyAction {
private:
	MyAction() {}
public:
	static MyAction* getInstance() {
		static MyAction instance;
		return &instance;
	}
	void preLoad();
	void changeScene(cocos2d::Scene* scene);
	cocos2d::Node* createNode(int tag, cocos2d::PhysicsBody* physics_body = NULL, bool dynamic = true);
	cocos2d::Sprite* createSprite(std::string file_name, int tag, Vec2 position, cocos2d::PhysicsBody* physics_body = NULL, bool dynamic = true);
	cocos2d::Sprite* createSprite(std::string file_name, int tag, Vec2 position, Vec2 anchorPoint, cocos2d::PhysicsBody* physics_body = NULL, bool dynamic = true);
	void addNode(cocos2d::Layer* layer, cocos2d::Node* node, int localZOrder);
	void shootAction(cocos2d::Layer* layer, Vec2 shootVelocity, cocos2d::Sprite* bullet, int localZOrder);
	cocos2d::Vec2 calAIShootVelocity(cocos2d::Vec2 shootPos, cocos2d::Vec2 targetPos, float g, float random_range = 0.0f);
	cocos2d::Vec2 calPlayerShootVelocity(cocos2d::Vec2 shootPos, cocos2d::Vec2 targetPos, int init_speed, float touch_time);
	void spriteFadeOut(cocos2d::Sprite* sprite);
	void addScore(int& score, const int add_score);
	void distroyAllBullet(cocos2d::Vector<Sprite*>* playerBullet, cocos2d::Vector<Sprite*>* AIBullet);
	void updateLabelScore(cocos2d::Label* score_label, int player_score, int AI_score, char* label_format);
	void updateLabelTime(cocos2d::Label* time_label, float currentTime, float totalTime);
	void showPerScore(cocos2d::Vec2 pos, int score, cocos2d::Layer* layer);
	void arrowRotation(cocos2d::Sprite* arrow, Vec2 shootPos, Vec2 mousePos);
	void arrowColor(cocos2d::Sprite* arrow, float touchTime);
	void showDizzyPic(cocos2d::Sprite* sprite, float dizzy_time);
	void judgeWin(int playerScore, int AIScore);
	const char* getChinese(char* file_name, char* key_name);
};

#endif // !__MY_ACTION_H__
