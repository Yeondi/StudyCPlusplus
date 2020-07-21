#ifndef STAGE3_H
#define STAGE3_H

#include "cocos2d.h"
#include "AppManager.h"
#include "Box2D/Box2D.h"
#include "GLES-Render.h"
#include "Entities/Entity.h"
#include "Entities/Player.h"
#include "Entities/Enemy.h"
#include "Extras/ContactListener.h"
#include "Inventory.h"
#include "HudLayer.h"
#include "Boss.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#pragma execution_character_set("utf-8")
#endif

class C_STAGE3 : public cocos2d::Layer
{
private:
	cocos2d::Size		winSize;
	cocos2d::Size		visibleSize;
	cocos2d::Point		origin;
	cocos2d::Size		worldSize;

	b2World*			_world;
	ContactListener*	contactListener;
	Player*				player;
	C_ENEMY*			enemy;
	C_HUDLAYER*			hudLayer;

	Label* labelJumpCount;

	cocos2d::TMXTiledMap*		tMap;
	cocos2d::TMXLayer*			tGround;
	cocos2d::TMXLayer*			tProps;
	cocos2d::TMXLayer*			tWall;
	cocos2d::TMXLayer*			tBG1;
	cocos2d::TMXLayer*			tBG2;
	cocos2d::TMXLayer*			tBG3;
	cocos2d::TMXLayer*			tCloud;
	cocos2d::TMXLayer*			tSky;

	cocos2d::TMXLayer*			pItem;
	cocos2d::TMXLayer*			pMeta;

	int                         stopDamage;
	b2Body* localBody;

	std::map<C_ENEMY*, int> m_pEnemy;
	std::map<float, int> m_dataFromCurrentStage; //float = healh , int = JumpCount

	float _bakHp;
	int _bakJumpCount;

public:
	static cocos2d::Scene* createScene();
	virtual bool init();

	CREATE_FUNC(C_STAGE3);

	void setHUDLayer(C_HUDLAYER* layer);
	static C_STAGE3* createWithHudAndBackGround(C_HUDLAYER* hudLayer); //backgroundº¸·ù
	
	void initTiledMap();
	void initBox2dWorld(bool debug);

	void createTerrain();
	void updatePhysics(float dt);

	cocos2d::Vec2 tileCoordForPosition(cocos2d::Vec2 position);
	void setPlayerPosForPlayer(cocos2d::Vec2 position);
	void setPlayerPosForEnemy(cocos2d::Vec2 position);
	void setViewpointCenter(cocos2d::Vec2 position);

	void updateContact();
	void updateMovingBox();
	void addNewSpriteWithBody(cocos2d::Vec2 pos,cocos2d::Size size,b2BodyType bodyType, float boxVelocity,const char * spriteName,int shape, Entity::categoryValue categoryBits);
	b2Body* addNewSpriteWithBody(cocos2d::Vec2 pos, cocos2d::Size size, b2BodyType bodyType, const char* spriteName, int shape, Entity::categoryValue categoryBits);

	void setDataToNextStage(std::map<float, int> dataFromPreviousStage);

	void update(float dt);
	
private:
	//For Debugging
	GLESDebugDraw*				m_debugDraw;
	cocos2d::CustomCommand		_customCmd;

	virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)override;
	void onDraw(const cocos2d::Mat4& transform, uint32_t flags);

	bool debug;
	cocos2d::Scene* nScene;

	cocos2d::Vec2 tileCoord;
	//

};

#endif