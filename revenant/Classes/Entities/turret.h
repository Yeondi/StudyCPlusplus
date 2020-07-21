#pragma once

#include "cocos2d.h"
#include "Enemy.h"
#include "Box2D/Box2D.h"

USING_NS_CC;

class C_TURRET : public Node
{
private:

public:
	C_TURRET();

	virtual bool init();
	bool initEnemy(b2World* world, cocos2d::TMXTiledMap* map);
	void setTMXMap(cocos2d::TMXTiledMap* map, cocos2d::ValueMap& pos);
	void addBodyToWorld(C_HUDLAYER* layer, b2World* world, cocos2d::TMXTiledMap* map);
};