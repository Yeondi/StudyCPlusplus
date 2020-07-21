#pragma once

#include "cocos2d.h"

class C_STARTSCENE : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	void menuCloseCallback(cocos2d::Ref* pSender);
	CREATE_FUNC(C_STARTSCENE);

	void initBG();
	void initTitle();
	void initButton();

	void startGame(Ref* object);

	cocos2d::Sprite* back;
	cocos2d::Size winSize;

};