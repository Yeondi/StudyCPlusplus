#ifndef PLAYER_H
#define PLAYER_H

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "Entity.h"
#include "SneakyInput/SneakyJoystick.h"
#include "HudLayer.h"
#include "StartScene.h"

#define PTM_RATIO 32.0

class Player : public Entity
{
private:    
    cocos2d::Vec2               vBackupPos;
    cocos2d::Point              pVelocity;
    cocos2d::TMXTiledMap*       _map;
    cocos2d::Vec2               _pos;

    C_HUDLAYER* hudLayer;

    float   _health;
    float   _maxHealth;

    float   _mana;
    float   _MaxMana;

    float   _playerDamage;
    float   _damRate;

    int     _attribute;
    float   _attributeRate;     // 0 = none , 1 = water , 2 = fire , 3 = earth , 4 = Wind   / 물>땅>불>바람>물

    bool    _isATTCounter;


public:
    Player();
    virtual ~Player();
    virtual bool init(C_HUDLAYER *layer, b2World *world, cocos2d::TMXTiledMap* map);
    void update(float dt);
    void updateVelocity(cocos2d::Point velocity);
    void move(cocos2d::Point velocity);
    void stopMoving();
    void actionButtonPressed(int button);
    void attackButtonPressed(int button);
    
    void setStateDefault();
    void setStateMoving();
    void setStateJumping();
    void setStateAttack();
    void changeDirection(int direction);

    void setDataFromPreviousStage(float health,int jump);

    void setTMXMap(cocos2d::TMXTiledMap* map);

    int getTag();
    Player* getPlayer();
    void addJumpCount(int _add);


    int nActionToken; // 0 = not available , 1 = available
    int nAttackToken; // 6frame

    cocos2d::Vec2 getBackupPos();
    void setBackupPos(cocos2d::Vec2 pos);

    //damage
    virtual void getDamage(float damage, int attribute)override;
    virtual bool isCounter(int attack, int defense)override;
    virtual float getOnesDamage()override;
    virtual float getHealth()override;
    virtual float getMaxHealth()override;
    //
};

#endif
