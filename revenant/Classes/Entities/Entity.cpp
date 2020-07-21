#include "Entity.h"

USING_NS_CC;

Entity::Entity()
{
    nJumpCount = TAG_JUMPCOUNT;
}

Entity::~Entity()
{
}

void Entity::setBatchNode(SpriteBatchNode* batchNode)
{
    this->batchNode = batchNode;
}

void Entity::setSprite(cocos2d::Sprite* sprite)
{
    this->sprite = sprite;
}

SpriteBatchNode* Entity::getBatchNode()
{
    return batchNode;
}

cocos2d::Sprite* Entity::getSprite()
{
    return sprite;
}

b2Body* Entity::getBody()
{
	return body;
}

b2Body* Entity::getEnemyBody()
{
    return enemyBody;
}

b2Body* Entity::getBullet()
{
    return bullet;
}

int Entity::getDirection()
{
    return direction;
}

int Entity::getState()
{
    return state;
}

int Entity::getTag()
{
    return TAG_PLAYER;
}

float Entity::getLocationX()
{
    return sprite->getPositionX();
}

float Entity::getLocationY()
{
    return sprite->getPositionY();
}

int Entity::getJumpCount()
{
    return nJumpCount;
}

int Entity::getAttribute()
{
    return _attribute;
}

void Entity::getDamage(float damage, int attribute)
{
}

bool Entity::isCounter(int attack, int defense)
{
    return false;
}

float Entity::getHealth()
{
    return _health;
}

float Entity::getMaxHealth()
{
    return _MaxHealth;
}

float Entity::getOnesDamage()
{
    return _pureDamage;
}

void Entity::getCleanUp()
{
}

cocos2d::Vec2 Entity::setLocation()
{
	return cocos2d::Vec2();
}
