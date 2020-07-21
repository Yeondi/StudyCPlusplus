//
//  ScrollMap.h
//  CocosProject1
//
//  Created by choikyuhwan on 2015. 7. 25..
//
//

#ifndef CocosProject1_ScrollMap_h
#define CocosProject1_ScrollMap_h

#include "AppManager.h"



#define CREATE_SCROLL_BACKGROUND(_BGNAME_, _WIDTH_, _SPEED_)\
private:\
cocos2d::Sprite * _scr_bg_std = cocos2d::Sprite::create(_BGNAME_);\
cocos2d::Sprite * _scr_bg_1 = cocos2d::Sprite::createWithTexture(_scr_bg_std->getTexture());\
cocos2d::Sprite * _scr_bg_2 = cocos2d::Sprite::createWithTexture(_scr_bg_std->getTexture());\
void moveLeftScrBg(float f){\
float speed = _SPEED_;\
if(_scr_bg_1->getPosition().x <= -(_WIDTH_)/2){\
_scr_bg_1->setPosition(cocos2d::Point(REAL_FRAME_CENTER_X+(_WIDTH_), REAL_FRAME_CENTER_Y));\
}\
if(_scr_bg_2->getPosition().x <= -(_WIDTH_)/2){\
_scr_bg_2->setPosition(cocos2d::Point(REAL_FRAME_CENTER_X+(_WIDTH_), REAL_FRAME_CENTER_Y));\
}\
_scr_bg_1->setPosition(cocos2d::Point(_scr_bg_1->getPosition().x-speed,REAL_FRAME_CENTER_Y));\
_scr_bg_2->setPosition(cocos2d::Point(_scr_bg_2->getPosition().x-speed,REAL_FRAME_CENTER_Y));\
}

#define INIT_SCROLL_BACKGROUND(_CLASS_)\
_scr_bg_1->setPosition(cocos2d::Point(STD_FRAME_CENTER_X, STD_FRAME_CENTER_Y));\
this->addChild(_scr_bg_1);\
_scr_bg_2->setPosition(cocos2d::Point(STD_FRAME_CENTER_X+_scr_bg_1->getContentSize().width, STD_FRAME_CENTER_Y));\
this->addChild(_scr_bg_2);\
this->schedule(schedule_selector(_CLASS_::moveLeftScrBg));





#endif
