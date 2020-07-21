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

#include "GameScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* Game::createScene()
{
	Scene* scene = Scene::create();

	C_HUDLAYER* hudLayer = C_HUDLAYER::create();
	scene->addChild(hudLayer, 2);

	Game* layer = Game::createWithHUDAndBackGround(hudLayer);
	scene->addChild(layer, 1);

	return scene;
}

bool Game::init()
{
	if (!Layer::init())
	{
		return false;
	}
	stopDamage = 0;

	visibleSize = Director::getInstance()->getVisibleSize();
	winSize = Director::getInstance()->getWinSize();
	origin = Director::getInstance()->getVisibleOrigin();
	nScene = new Scene();

	// create bottom label
	Label* label = Label::createWithSystemFont("rev_0.7.1 - Stage01", "Arial", 24);
	label->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - label->getContentSize().height));
	hudLayer->addChild(label, 1);


	initTiledMap();

	worldSize = Size(tMap->getContentSize().width, tMap->getContentSize().height);

	// create physics
	initBox2dWorld(true);

	// add the player
	player = new Player();
	player->init(Game::hudLayer, _world, tMap);
	addChild(player->getBatchNode());

	labelJumpCount = Label::createWithCharMap("fonts/tuffy_bold_italic-charmap.png", 48, 64, ' ');
	labelJumpCount->setScale(0.8f);
	labelJumpCount->setString(StringUtils::format("%d", player->getJumpCount() / 2));
	labelJumpCount->setPosition(Vec2(900, origin.y + visibleSize.height - label->getContentSize().height));
	hudLayer->addChild(labelJumpCount, 1);

	int nDataNum = 10000;
	auto objects = tMap->getObjectGroup("Objects")->getObjects();
	for (auto i : objects)
	{
		auto j = i.asValueMap();
		if ("Enemy" == j["name"].asString())
		{
			C_ENEMY* enemy = nullptr;
			std::string str(j["type"].asString());
			if (str == "Normal")
			{
				//enemy = new C_ENEMY();
			}
			if (str == "Shoot")
			{
				//enemy = C_ENEMY::create();
				enemy = new C_ENEMY();
			}
			if (str == "Boss")
			{
				//enemy = nullptr;
			}
			enemy->init();
			enemy->initEnemy(_world, tMap);
			enemy->setTMXMap(tMap, j);
			enemy->addBodyToWorld(Game::hudLayer, _world, tMap);
			//m_pEnemy.push_back(enemy);
			m_pEnemy.insert(std::map<C_ENEMY*, int>::value_type(enemy, nDataNum));
			nDataNum++;
			addChild(enemy->getBatchNode());
		}
	}
	for (auto i : m_pEnemy)
	{
		i.first->setData(m_pEnemy);
		i.first->setPlayer(player);
	}

	// rest of framework init process

	scheduleUpdate();

	return true;
}

void Game::setHUDLayer(C_HUDLAYER* layer)
{
	hudLayer = layer;
}

Game* Game::createWithHUDAndBackGround(C_HUDLAYER* hudLayer)
{
	Game* object = new Game();
	object->setHUDLayer(hudLayer);

	if (object && object->init())
	{
		object->autorelease();
		return object;
	}
	else
	{
		delete object;
		object = NULL;
		return NULL;
	}

}

void Game::initTiledMap()
{
	tMap = TMXTiledMap::create("BG/stage1.tmx");
	tGround = tMap->getLayer("Ground");
	tProps = tMap->getLayer("props");
	tProps2 = tMap->getLayer("props2");
	tWall = tMap->getLayer("wall");
	tBG1 = tMap->getLayer("BG1");
	tBG2 = tMap->getLayer("BG2");
	tBG3 = tMap->getLayer("BG3");
	tBG4 = tMap->getLayer("BG4");
	tCloud = tMap->getLayer("cloud");
	tSky = tMap->getLayer("Sky");
	pMeta = tMap->getLayer("meta");
	pItem = tMap->getLayer("Items");

	pMeta->setVisible(false);

	tMap->setAnchorPoint(Vec2(0.0f, 0.0f));
	tMap->setPosition(Vec2(0.0f, -14.0f));

	this->addChild(tMap, 0, 11);
}

void Game::initBox2dWorld(bool debug)
{
	// _world
	b2Vec2 gravity = b2Vec2(0.0f, -9.8f);
	//b2Vec2 gravity = b2Vec2(0.0f, -1000.0f);
	_world = new b2World(gravity);

	// contact listener
	contactListener = new ContactListener();
	_world->SetContactListener(contactListener);

	//debug
	if (debug)
	{
		m_debugDraw = new GLESDebugDraw(PTM_RATIO);
		_world->SetDebugDraw(m_debugDraw);

		uint32 flags = 0;
		flags += b2Draw::e_shapeBit;
		m_debugDraw->SetFlags(flags);
	}


	// create collision wall and ground
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0, (20 / PTM_RATIO));
	b2Body* _groundBody;
	_groundBody = _world->CreateBody(&groundBodyDef);

	b2EdgeShape groundBox;
	b2FixtureDef groundBoxDef;
	groundBoxDef.shape = &groundBox;

	b2Fixture* _bottomFixture;

	auto getMapSize = tMap->getMapSize(); // 100,15
	auto getTileSize = tMap->getTileSize(); // 32,32


	groundBox.Set(b2Vec2(0, 0), b2Vec2(18, 0));  // 3200,480
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(21, 2), b2Vec2(24, 2));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef); // 다리1

	groundBox.Set(b2Vec2(25, 3), b2Vec2(29, 3));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef); // 다리2

	groundBox.Set(b2Vec2(31, 4), b2Vec2(32, 4));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef); // 다리3

	groundBox.Set(b2Vec2(35, 4), b2Vec2(58, 4));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef); // 사막 이전

	groundBox.Set(b2Vec2(58, 5), b2Vec2(65, 5));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef); // 사막 초입

	groundBox.Set(b2Vec2(65, 4), b2Vec2(66, 4));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(66, 3), b2Vec2(67, 3));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(67, 2), b2Vec2(68, 2));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(68, 1), b2Vec2(69, 1));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef); //계단

	groundBox.Set(b2Vec2(69, 0), b2Vec2(85, 0));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef); //사막끝 ~ 동굴초입

	groundBox.Set(b2Vec2(85, 3), b2Vec2(89, 3));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef); //동굴 언덕1
	groundBox.Set(b2Vec2(89, 3), b2Vec2(89, -1));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef); //동굴 언덕1


	groundBox.Set(b2Vec2(91, 5), b2Vec2(91, -1));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(91, 5), b2Vec2(93, 5));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(93, 6), b2Vec2(94, 6));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(94, 7), b2Vec2(96, 7));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(96, 7), b2Vec2(96, -1));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(99, 0), b2Vec2(99, 12));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef); //끝 벽

	groundBox.Set(b2Vec2(0, 0), b2Vec2(0, (worldSize.height / PTM_RATIO) + 32.0f));
	_groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(0, worldSize.height / PTM_RATIO), b2Vec2(worldSize.width / PTM_RATIO,
		worldSize.height / PTM_RATIO));
	_groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(worldSize.width / PTM_RATIO, worldSize.height / PTM_RATIO),
		b2Vec2(worldSize.width / PTM_RATIO, 0));

	_groundBody->CreateFixture(&groundBoxDef);

	createTerrain();

}

void Game::createTerrain()
{
	b2BodyDef bodyDef1;
	bodyDef1.type = b2_dynamicBody;
	bodyDef1.position.Set(10, 1);
	b2Body* body1 = _world->CreateBody(&bodyDef1);

	b2PolygonShape staticBox1;

	b2Vec2 stairs[6];
	stairs[0].x = 0.0f;
	stairs[0].y = 0.0f;

	stairs[1].x = 5.0f;
	stairs[1].y = 0.0f;

	stairs[2].x = 5.0f;
	stairs[2].y = 2.0f;

	stairs[3].x = 1.0f;
	stairs[3].y = 2.0f;

	stairs[4].x = 1.0f;
	stairs[4].y = 1.0f;

	stairs[5].x = 0.0f;
	stairs[5].y = 1.0f;

	staticBox1.Set(stairs, 6);

	b2FixtureDef fixtureDef1;
	fixtureDef1.shape = &staticBox1;
	fixtureDef1.density = TAG_ADAMENT;
	fixtureDef1.friction = 0.0f;
	body1->CreateFixture(&fixtureDef1);

	//

	b2BodyDef bodyDef2;
	bodyDef2.type = b2_dynamicBody;
	bodyDef2.position.Set(16, 1);
	b2Body* body2 = _world->CreateBody(&bodyDef2);

	b2PolygonShape slopeBox1;
	b2Vec2 slope[4];
	slope[0].x = 0.0f;
	slope[0].y = 0.0f;

	slope[1].x = 2.0f;
	slope[1].y = 0.0f;

	slope[2].x = 2.0f;
	slope[2].y = 1.0f;

	slope[3].x = 1.0f;
	slope[3].y = 1.0f;

	slopeBox1.Set(slope, 4);
	b2FixtureDef fixtureDef2;
	fixtureDef2.shape = &slopeBox1;
	fixtureDef2.density = TAG_ADAMENT;
	fixtureDef2.friction = 0.0f;
	body2->CreateFixture(&fixtureDef2);

	b2BodyDef bodyDef3;
	bodyDef3.type = b2_dynamicBody;
	bodyDef3.position.Set(82, 1);
	b2Body* body3 = _world->CreateBody(&bodyDef3);

	b2PolygonShape slopeBox2;
	b2Vec2 slope2[5];
	slope2[0].x = 0.0f;
	slope2[0].y = 0.0f;

	slope2[1].x = 3.0f;
	slope2[1].y = 0.0f;

	slope2[2].x = 3.0f;
	slope2[2].y = 2.0f;

	slope2[3].x = 2.0f;
	slope2[3].y = 1.0f;

	slope2[4].x = 1.0f;
	slope2[4].y = 1.0f;

	slopeBox2.Set(slope2, 5);
	b2FixtureDef fixtureDef3;
	fixtureDef3.shape = &slopeBox2;
	fixtureDef3.density = TAG_ADAMENT;
	fixtureDef3.friction = 0.0f;
	body3->CreateFixture(&fixtureDef3);

}

void Game::updatePhysics(float dt)
{
	int velocityIterations = 8;
	int positionIterations = 3;
	_world->Step(dt, velocityIterations, positionIterations);
}

void Game::update(float dt)
{
	updatePhysics(dt);


	if (player)
	{
		player->update(dt);

		player->updateVelocity(hudLayer->leftJoystick->getVelocity());
		if (hudLayer->action1Button->getIsActive())
		{
			player->actionButtonPressed(1);
		}
		if (hudLayer->action2Button->getIsActive())
		{
			player->attackButtonPressed(1);
		}
	}

	labelJumpCount->setString(StringUtils::format("%d", player->getJumpCount() / 2));

	if (!m_pEnemy.empty())
	{
		for (auto i : m_pEnemy)
		{
			i.first->setNumTag(i.second);
			i.first->update(dt);
			this->setPlayerPosForEnemy(i.first->getBackupPos());
			i.first->detectTarget();
		}
	}

	this->setPlayerPosForPlayer(player->getBackupPos());

	this->setViewpointCenter(Vec2(player->getLocationX(), player->getLocationY()));

	// update collisions
	std::vector<ContactData>::iterator pos;
	for (pos = contactListener->_contacts.begin(); pos != contactListener->_contacts.end(); ++pos)
	{
		ContactData contact = *pos;

		// get the box2d bodies for each object
		b2Body* bodyA = contact.fixtureA->GetBody();
		b2Body* bodyB = contact.fixtureB->GetBody();

		auto catA = contact.fixtureA->GetFilterData().categoryBits;
		auto maskA = contact.fixtureA->GetFilterData().maskBits;
		auto catB = contact.fixtureB->GetFilterData().categoryBits;
		auto maskB = contact.fixtureB->GetFilterData().maskBits;

		if (bodyA->GetUserData() != NULL && bodyB->GetUserData() != NULL)
		{
			Entity* entityA = (Entity*)bodyA->GetUserData();
			Entity* entityB = (Entity*)bodyB->GetUserData();

			int iTagA = entityA->getTag();
			int iTagB = entityB->getTag();

			int _currentStateA = 1; // 1 = default
			if ((catA & maskB) != 0 && (catB & maskA) != 0)
			{
				if (catA == Entity::categoryPlayer || catA == Entity::categoryEnemyBullet &&
					catB == Entity::categoryPlayer || catB == Entity::categoryEnemyBullet && catA != catB)
				{
					cocos2d::log("catA is %d , catB is %d", catA, catB);
					if (catA == Entity::categoryEnemyBullet)
					{
						_world->DestroyBody(bodyA);
						cocos2d::log("expected error");
					}
					else if (catB == Entity::categoryEnemyBullet)
					{
						_world->DestroyBody(bodyB);
						entityA->getDamage(entityB->getOnesDamage(), 0);
						break;
					}
				}

				if (catA == Entity::categoryPlayer && catB == Entity::categoryEnemy)
				{
					_currentStateA = entityA->getState();
					if (_currentStateA == 4 && stopDamage != 1)
					{
						cocos2d::log("레버넌트가 몹을 떄렸다!");
						entityB->getDamage(entityA->getOnesDamage(), 0);
						stopDamage++;
						if (entityB->getHealth() <= 0.0f)
						{
							Sprite* userData = (Sprite*)bodyB->GetUserData();
							entityB->getCleanUp();
							_world->DestroyBody(bodyB);
							break;
						}
					}
					if (_currentStateA != 4)
					{
						stopDamage = 0;
					}
					auto _hpratio = entityA->getHealth() / entityA->getMaxHealth();
					hudLayer->updateHpBar(_hpratio);
				}
			}
			cocos2d::log("몹의 현재 체력 : %f", entityB->getHealth());
			cocos2d::log("플레이어의 현재 체력 : %f", entityA->getHealth());
		}
	}
}

cocos2d::Vec2 Game::tileCoordForPosition(cocos2d::Vec2 position)
{
	int x = position.x / tMap->getTileSize().width;
	int y = ((tMap->getMapSize().height * tMap->getTileSize().height) - position.y) / tMap->getTileSize().height;

	return Vec2(x, y);
}

void Game::setPlayerPosForPlayer(cocos2d::Vec2 position)
{
	Vec2 tileCoord = this->tileCoordForPosition(position);
	if (position.y <= 0.0f)
	{
		return;
	}
	else
	{
		int tileGid = this->pMeta->getTileGIDAt(tileCoord);

		if (tileGid)
		{
			Value vProperties = tMap->getPropertiesForGID(tileGid);

			if (!vProperties.isNull())
			{
				std::string wall = vProperties.asValueMap()["type"].asString();
				if (wall == "portal")
				{
					m_dataFromCurrentStage.insert(std::map<float, int>::value_type(player->getHealth(), player->getJumpCount()));
					C_STAGE2* cStage2 = new C_STAGE2;
					cStage2->setDataToNextStage(m_dataFromCurrentStage);
					Director::getInstance()->pushScene(C_STAGE2::createScene());
				}
				if (wall == "gameover")
				{
					Director::getInstance()->replaceScene(C_STARTSCENE::createScene());
				}
				if (wall == "item")
				{
					this->pMeta->removeTileAt(tileCoord);
					pItem->removeTileAt(tileCoord);
					player->addJumpCount(2);
				}
			}
		}
	}
}

void Game::setPlayerPosForEnemy(cocos2d::Vec2 position)
{
	Vec2 tileCoord = this->tileCoordForPosition(position);
	if (position.y <= 0.0f)
	{
		return;
	}
	else
	{
		int tileGid = this->pMeta->getTileGIDAt(tileCoord);

		if (tileGid)
		{
			Value vProperties = tMap->getPropertiesForGID(tileGid);

			if (!vProperties.isNull())
			{
				std::string wall = vProperties.asValueMap()["type"].asString();
				if (wall == "turnPoint")
				{
					for (auto i : m_pEnemy)
					{
						i.first->IsWall = true;
					}
				}
			}
		}
	}
}

void Game::setViewpointCenter(cocos2d::Vec2 position)
{
	int x = MAX(position.x, winSize.width / 2);
	int y = MAX(position.y, winSize.height / 2);

	x = MIN(x, (tMap->getMapSize().width * tMap->getTileSize().width) - winSize.width / 2);
	y = MIN(y, (tMap->getMapSize().height * tMap->getTileSize().height) - winSize.height / 2);

	Vec2 actualPosition = Vec2(x, y);
	Vec2 centerView = Vec2(winSize.width / 2, winSize.height / 2);
	Vec2 viewPoint = centerView - actualPosition;

	this->setPosition(viewPoint);

}

void Game::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
	nScene->draw(renderer, transform, flags);

	_customCmd.init(_globalZOrder, transform, flags);
	_customCmd.func = CC_CALLBACK_0(Game::onDraw, this, transform, flags);

	renderer->addCommand(&_customCmd);
}

void Game::onDraw(const cocos2d::Mat4& transform, uint32_t flags)
{
	Director* director = Director::getInstance();
	CCASSERT(nullptr != director, "Director is null when setting matrix stack");
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);

	GL::enableVertexAttribs(cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION);
	_world->DrawDebugData();
	CHECK_GL_ERROR_DEBUG();

	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}