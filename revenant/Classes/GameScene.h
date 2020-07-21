/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#pragma execution_character_set("utf-8")
#endif

#include "cocos2d.h"
#include "AppManager.h"
#include "Box2D/Box2D.h"
#include "GLES-Render.h"
#include "Entities/Entity.h"
#include "Entities/Player.h"
#include "Entities/Enemy.h"
#include "Extras/ContactListener.h"
#include "HudLayer.h"
#include "Stage2.h"

#define TIME_PAUSE 40


class Game : public cocos2d::Layer
{
private:
    cocos2d::Size winSize;
    cocos2d::Size visibleSize;
    cocos2d::Point origin;
    cocos2d::Size           worldSize;

    b2World*                _world;
    ContactListener*        contactListener;
    Player*                 player;
    C_ENEMY*                enemy;
    C_HUDLAYER*             hudLayer;

    Label* labelJumpCount;
    //objects

    cocos2d::TMXTiledMap*       tMap;
    cocos2d::TMXLayer*          tGround;
    cocos2d::TMXLayer*          tProps;
    cocos2d::TMXLayer*          tProps2;
    cocos2d::TMXLayer*          tWall;
    cocos2d::TMXLayer*          tBG1;
    cocos2d::TMXLayer*          tBG2;
    cocos2d::TMXLayer*          tBG3;
    cocos2d::TMXLayer*          tBG4;
    cocos2d::TMXLayer*          tCloud;
    cocos2d::TMXLayer*          tSky;

    cocos2d::TMXLayer*          pItem;
    cocos2d::TMXLayer*          pMeta;

    int                         stopDamage;

    std::map<C_ENEMY*,int> m_pEnemy;
    std::map<float, int> m_dataFromCurrentStage; //float = healh , int = JumpCount

public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(Game);

    void setHUDLayer(C_HUDLAYER* layer);
    static Game* createWithHUDAndBackGround(C_HUDLAYER* hudLayer);

    void initTiledMap();
    void initBox2dWorld(bool debug);

    void createTerrain();
    void updatePhysics(float dt);

    cocos2d::Vec2 tileCoordForPosition(cocos2d::Vec2 position);
    void setPlayerPosForPlayer(cocos2d::Vec2 position);
    void setPlayerPosForEnemy(cocos2d::Vec2 position);
    void setViewpointCenter(cocos2d::Vec2 position);

    void update(float dt);

private:
    // For debugging
    GLESDebugDraw* m_debugDraw;
    cocos2d::CustomCommand _customCmd;

    virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
    void onDraw(const cocos2d::Mat4& transform, uint32_t flags);

    cocos2d::Scene* nScene;

    cocos2d::Vec2 tileCoord;
    //

};

#endif // __HELLOWORLD_SCENE_H__
