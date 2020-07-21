#include "turret.h"

C_TURRET::C_TURRET()
{
}

bool C_TURRET::init()
{
	return false;
}

bool C_TURRET::initEnemy(b2World* world, cocos2d::TMXTiledMap* map)
{
	return false;
}

void C_TURRET::setTMXMap(cocos2d::TMXTiledMap* map, cocos2d::ValueMap& pos)
{
}

void C_TURRET::addBodyToWorld(C_HUDLAYER* layer, b2World* world, cocos2d::TMXTiledMap* map)
{
}
