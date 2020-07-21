#ifndef ENEMY_H
#define ENEMY_H

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "Entity.h"
#include "SneakyInput/SneakyJoystick.h"
#include "HudLayer.h"
#include "StartScene.h"
#include "Player.h"
#include "C_STATE.h"

#define PTM_RATIO 32.0

class C_STATE;

class C_ENEMY : public Entity
{
public:

	C_ENEMY();
	virtual ~C_ENEMY();
	virtual bool init();

	bool initEnemy(b2World* world, cocos2d::TMXTiledMap* map);

	void update(float dt);
	void stopMoving();
	void attackButtonPressed(int button);

	CREATE_FUNC(C_ENEMY);

	void setStateDefault();
	void setStateMoving();
	void setStateJumping();
	void setStateAttack();
	void changeDirection(int direction);
	
	void addBodyToWorld(C_HUDLAYER* layer, b2World* world, cocos2d::TMXTiledMap* map);
	void setTMXMap(cocos2d::TMXTiledMap* map, cocos2d::ValueMap& pos);

	int getTag();

	float						fPosY;
	cocos2d::Vec2               pBackupPos;
	cocos2d::Point				pVelocity;
	cocos2d::TMXTiledMap*		_map;
	cocos2d::Vec2				_pos;
	cocos2d::Size visibleSize;
	cocos2d::Point origin;
	b2World* _world;

	int nActionToken; // 0 = not available , 1 = available
	int nAttackToken; // 6frame

	cocos2d::Vec2 getBackupPos();

	//damage
	virtual void getDamage(float damage, int attribute)override;
	virtual bool isCounter(int attack, int defense)override;
	virtual float getHealth()override;
	virtual float getMaxHealth()override;
	virtual void getCleanUp()override;
	virtual float getOnesDamage()override;

	float _enemyDamage;
	//

	std::map<C_ENEMY*, int> m_pEnemy;
	Player* m_pPlayer;

	//¿‹∏”∏Æ ¡∏
	C_STATE* m_pCurrentState;
	C_STATE* m_pNewState;
	void doUpdate();
	bool IsState(C_STATE& state);
	void ChangeState(C_STATE* pNewState);
	void StartMoving();
	void startAttack();
	bool detectTarget();
	float fTimerForStay;
	float fMovingTimer;
	float wakeDistance;
	bool IsWall;

	//projectile
	void makeBullet();
	void createBullet();

	void setNumTag(int _Tag);
	int Tag;
	void setData(std::map<C_ENEMY*, int> _Data);
	std::map<C_ENEMY*, int> getData();
	void setPlayer(Player* player);
	int playerDirection;
	//


	void createTest();
	b2Body* localBody;
	b2Body* addNewSpriteWithBody(cocos2d::Vec2 pos, cocos2d::Size size, b2BodyType bodyType, const char* spriteName, int shape, Entity::categoryValue categoryBits);
	cocos2d::Sprite* ball;
};

#endif