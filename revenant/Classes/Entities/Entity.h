#ifndef ENTITY_H
#define ENTITY_H

#include "cocos2d.h"
#include "Box2D/Box2D.h"

#define PTM_RATIO 32.0
#define TAG_ADAMENT 3200
#define TAG_NOUPDATE 9999

class Entity : public cocos2d::Node
{
public:
    static const int TAG_PLAYER = 1;
    static const int TAG_GROUND = 2;
    static const int TAG_JUMPCOUNT = 2;
    static const int TAG_ENEMY = 10;
    static const int TAG_NO = 99;

    Entity();
    virtual ~Entity();
    void setBatchNode(cocos2d::SpriteBatchNode* batchNode);
    void setSprite(cocos2d::Sprite* sprite);
    cocos2d::SpriteBatchNode* getBatchNode();
    cocos2d::Sprite* getSprite();
    b2Body* getBody();
    b2Body* getEnemyBody();
    b2Body* getBullet();
    int getDirection();
    int getState();
    virtual int getTag();
    float getLocationX();
    float getLocationY();
    int getJumpCount();
    int getAttribute();

    virtual void getDamage(float damage, int attribute);
    virtual bool isCounter(int attack, int defense);
    virtual float getHealth();
    virtual float getMaxHealth();
    virtual float getOnesDamage();
    virtual void getCleanUp();


    cocos2d::Vec2 setLocation();

protected:

    cocos2d::Sprite*            sprite;
    cocos2d::RepeatForever*     actionStateDefault;
    cocos2d::RepeatForever*     actionStateMoving;
    cocos2d::RepeatForever*     actionStateJumping;
    cocos2d::RepeatForever*     actionStateAttack;
    cocos2d::RepeatForever*     actionStateDie;
    cocos2d::Animate*           animateDefault;
    cocos2d::Animate*           animateMoving;
    cocos2d::Animate*           animateJumping;
    cocos2d::Animate*           animateAttack;
    cocos2d::Animate*           animateDie;
    cocos2d::Animation*         animationDefault;
    cocos2d::Animation*         animationMoving;
    cocos2d::Animation*         animationJumping;
    cocos2d::Animation*         animationAttack;
    cocos2d::Animation*         animationDie;
    cocos2d::SpriteBatchNode*   batchNode;
    
    cocos2d::Sprite*            _hpBar;
    cocos2d::Sprite*            _hpGauge;      

    cocos2d::RepeatForever* actionBall;
    cocos2d::Animate* animateBall;
    cocos2d::Animation* animationBall;

    float _health;
    float _MaxHealth;
    float _speed;
    float   _damRate;
    int     _attribute;
    float   _attributeRate;
    bool _isATTCounter;
    float _pureDamage;

    int direction;
    b2Body* body;
    b2Body* enemyBody;
    b2Body* bullet;
    b2World* world;
    int state; // 1 = default, 2 = moving, 3 = jumping, 4 = attack
    int nLocation;
    int nJumpCount;


    //¿‹∏”∏Æon



public:
    enum categoryValue
    {
        categoryPlayer = 0x0002,
        categoryPlayerBullet = 0x0003,
        categoryEnemy = 0x0004,
        categoryEnemyBullet = 0x0005,

        categoryMovingBox1 = 0x1001,
        categoryMovingBox2 = 0x1005,
        categoryMovingBox3 = 0x1003,
        categoryMovingBox4 = 0x1004,
        categoryRevJoint1 = 0x2001,
        categoryRevJoint2 = 0x2002,
    };

};

#endif // ENTITY_H
