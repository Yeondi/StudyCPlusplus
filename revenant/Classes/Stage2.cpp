#include "Stage2.h"

USING_NS_CC;

Scene* C_STAGE2::createScene() //¿©±â µ¥ÀÌÅÍ¸¦ µû·Î ¿Å°Ü¼­ °ü¸®
{
	Scene* scene = Scene::create();

	C_HUDLAYER* hudLayer = C_HUDLAYER::create();
	scene->addChild(hudLayer, 2);

	C_STAGE2* layer = C_STAGE2::createWithHudAndBackGround(hudLayer);
	scene->addChild(layer, 1);

	return scene;
}

bool C_STAGE2::init()
{
	if (!Layer::init())
		return false;

	visibleSize = Director::getInstance()->getVisibleSize();
	winSize = Director::getInstance()->getWinSize();
	origin = Director::getInstance()->getVisibleOrigin();
	nScene = new Scene;

	Label* label = Label::createWithSystemFont("rev_0.7.1 - Stage02", "Arial", 24);
	label->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - label->getContentSize().height));
	hudLayer->addChild(label, 1);

	initTiledMap();

	worldSize = Size(tMap->getContentSize().width, tMap->getContentSize().height);

	initBox2dWorld(true);


	player = new Player();
	player->init(C_STAGE2::hudLayer, _world, tMap);
	player->setDataFromPreviousStage(_bakHp, _bakJumpCount);
	this->addChild(player->getBatchNode());

	m_dataFromCurrentStage.clear();

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
			enemy->addBodyToWorld(C_STAGE2::hudLayer, _world, tMap);
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

	scheduleUpdate();

	return true;
}

void C_STAGE2::setHUDLayer(C_HUDLAYER* layer)
{
	hudLayer = layer;
}

C_STAGE2* C_STAGE2::createWithHudAndBackGround(C_HUDLAYER* hudLayer)
{
	C_STAGE2* object = new C_STAGE2();
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
		return nullptr;
	}
}

void C_STAGE2::initTiledMap()
{
	tMap = TMXTiledMap::create("BG/stage2.tmx");
	tGround = tMap->getLayer("Ground");
	tProps = tMap->getLayer("props");
	tWall = tMap->getLayer("wall");
	tBG1 = tMap->getLayer("BG1");
	tBG2 = tMap->getLayer("BG2");
	tBG3 = tMap->getLayer("BG3");
	tCloud = tMap->getLayer("cloud");
	tSky = tMap->getLayer("Sky");
	pMeta = tMap->getLayer("meta");
	pItem = tMap->getLayer("Items");

	pMeta->setVisible(false);

	tMap->setAnchorPoint(Vec2(0.0f, 0.0f));
	tMap->setPosition(Vec2(0.0f, -14.0f));

	this->addChild(tMap, 0, 11);
}

void C_STAGE2::initBox2dWorld(bool debug)
{
	b2Vec2 gravity = b2Vec2(0.0f, -9.8f);
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
		//flags += b2Draw::e_jointBit;
		//flags += b2Draw::e_aabbBit;
		//flags += b2Draw::e_pairBit;
		//flags += b2Draw::e_centerOfMassBit;
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

	groundBox.Set(b2Vec2(0, 3), b2Vec2(4, 3));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(4, 2), b2Vec2(9, 2));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(9, 3), b2Vec2(13, 3));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(13, 4), b2Vec2(16, 4));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(16, 4), b2Vec2(16, -1));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	//
	groundBox.Set(b2Vec2(19, -1), b2Vec2(19, 2));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(19, 2), b2Vec2(21, 2));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(21, 4), b2Vec2(25, 4));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(25, 4), b2Vec2(25, -1));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(25, 0), b2Vec2(31, 0));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	//
	groundBox.Set(b2Vec2(33, 1), b2Vec2(36, 1));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef); //´Ù¸®1

	groundBox.Set(b2Vec2(39, 2), b2Vec2(40, 2));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(42, 2), b2Vec2(43, 2));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	//ÇÏ´Ã
	groundBox.Set(b2Vec2(29, 10), b2Vec2(34, 10));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(37, 10), b2Vec2(38, 10));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(42, 10), b2Vec2(45, 10));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(48, 10), b2Vec2(49, 10));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(52, 10), b2Vec2(53, 10));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(56, 10), b2Vec2(58, 10));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(61, 10), b2Vec2(62, 10));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	//
	groundBox.Set(b2Vec2(45, 3), b2Vec2(49, 3));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(45, 1), b2Vec2(45, 3));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(49, 1), b2Vec2(49, 3));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	//2´ë·ú
	groundBox.Set(b2Vec2(50, -1), b2Vec2(50, 3));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(50, 3), b2Vec2(52, 3));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(52, 2), b2Vec2(57, 2));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(57, 0), b2Vec2(74, 0));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(74, -1), b2Vec2(74, 1));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(74, 1), b2Vec2(76, 1));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(76, 0), b2Vec2(93, 0));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(93, -1), b2Vec2(93, 2));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(93, 2), b2Vec2(92, 2));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(92, 2), b2Vec2(92, 15));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	//2´ë·ú ´Ù¸®
	groundBox.Set(b2Vec2(85, 2), b2Vec2(88, 2));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(89, 4), b2Vec2(91, 4));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(86, 6), b2Vec2(87, 6));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	//2´ë·ú »ê
	groundBox.Set(b2Vec2(65, 10), b2Vec2(73, 10));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(73, 8), b2Vec2(79, 8));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(79, 7), b2Vec2(81, 7));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(81, 7), b2Vec2(81, 5));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(81, 5), b2Vec2(82, 5));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(82, 5), b2Vec2(82, 3));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(65, 10), b2Vec2(65, 5));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

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

void C_STAGE2::createTerrain()
{
	b2BodyDef bodyDef1;
	bodyDef1.type = b2_dynamicBody;
	bodyDef1.position.Set(4.02f, 4.0f);
	b2Body* body1 = _world->CreateBody(&bodyDef1);

	b2PolygonShape slopeShape1;

	b2Vec2 slope1[3];
	slope1[0].x = 0.0f;
	slope1[0].y = 0.0f;

	slope1[1].x = 1.0f;
	slope1[1].y = 0.0f;

	slope1[2].x = 0.0f;
	slope1[2].y = 1.0f;

	slopeShape1.Set(slope1, 3);

	b2FixtureDef fixtureDef1;
	fixtureDef1.shape = &slopeShape1;
	fixtureDef1.density = TAG_ADAMENT;
	fixtureDef1.friction = 0.0f;
	body1->CreateFixture(&fixtureDef1);

	//
	bodyDef1.type = b2_dynamicBody;
	bodyDef1.position.Set(7.98f, 4.0f);
	body1 = _world->CreateBody(&bodyDef1);

	slope1[0].x = 0.0f;
	slope1[0].y = 0.0f;

	slope1[1].x = 1.0f;
	slope1[1].y = 0.0f;

	slope1[2].x = 1.0f;
	slope1[2].y = 1.0f;

	slopeShape1.Set(slope1, 3);

	body1->CreateFixture(&fixtureDef1);

	//
	bodyDef1.type = b2_dynamicBody;
	bodyDef1.position.Set(11.98f, 4.0f);
	body1 = _world->CreateBody(&bodyDef1);


	slope1[0].x = 0.0f;
	slope1[0].y = 0.0f;

	slope1[1].x = 1.0f;
	slope1[1].y = 0.0f;

	slope1[2].x = 1.0f;
	slope1[2].y = 1.0f;

	slopeShape1.Set(slope1, 3);

	body1->CreateFixture(&fixtureDef1);

	//À°Áö2
	bodyDef1.type = b2_dynamicBody;
	bodyDef1.position.Set(18.98f, 3.0f);
	body1 = _world->CreateBody(&bodyDef1);


	slope1[0].x = 0.0f;
	slope1[0].y = 0.0f;

	slope1[1].x = 2.0f;
	slope1[1].y = 0.0f;

	slope1[2].x = 2.0f;
	slope1[2].y = 2.0f;

	slopeShape1.Set(slope1, 3);

	body1->CreateFixture(&fixtureDef1);

	//´ë·ú2
	bodyDef1.type = b2_dynamicBody;
	bodyDef1.position.Set(52.02f, 3.0f);
	body1 = _world->CreateBody(&bodyDef1);


	slope1[0].x = 0.0f;
	slope1[0].y = 0.0f;

	slope1[1].x = 1.0f;
	slope1[1].y = 0.0f;

	slope1[2].x = 0.0f;
	slope1[2].y = 1.0f;

	slopeShape1.Set(slope1, 3);

	body1->CreateFixture(&fixtureDef1);

	//
	bodyDef1.type = b2_dynamicBody;
	bodyDef1.position.Set(57.02f, 0.f);
	body1 = _world->CreateBody(&bodyDef1);


	slope1[0].x = 0.0f;
	slope1[0].y = 0.0f;

	slope1[1].x = 2.0f;
	slope1[1].y = 0.0f;

	slope1[2].x = 0.0f;
	slope1[2].y = 1.98f;

	slopeShape1.Set(slope1, 3);

	body1->CreateFixture(&fixtureDef1);

	//2´ë·ú »ê
	bodyDef1.type = b2_dynamicBody;
	bodyDef1.position.Set(73.02f, 9.0f);
	body1 = _world->CreateBody(&bodyDef1);


	slope1[0].x = 0.0f;
	slope1[0].y = 0.0f;

	slope1[1].x = 2.0f;
	slope1[1].y = 0.0f;

	slope1[2].x = 0.0f;
	slope1[2].y = 1.98f;

	slopeShape1.Set(slope1, 3);

	body1->CreateFixture(&fixtureDef1);

}

void C_STAGE2::updatePhysics(float dt)
{
	int velocityIterations = 8;
	int positionIterations = 3;
	_world->Step(dt, velocityIterations, positionIterations);
}

void C_STAGE2::update(float dt)
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

		auto test = bodyB->GetFixtureList()->GetFilterData().categoryBits;



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
						cocos2d::log("expect error");
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
						cocos2d::log("·¹¹ö³ÍÆ®°¡ ¸÷À» ‹š·È´Ù!");
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
			cocos2d::log("¸÷ÀÇ ÇöÀç Ã¼·Â : %f", entityB->getHealth());
			cocos2d::log("ÇÃ·¹ÀÌ¾îÀÇ ÇöÀç Ã¼·Â : %f", entityA->getHealth());
		}
	}
}

cocos2d::Vec2 C_STAGE2::tileCoordForPosition(cocos2d::Vec2 position)
{
	int x = position.x / tMap->getTileSize().width;
	int y = ((tMap->getMapSize().height * tMap->getTileSize().height) - position.y) / tMap->getTileSize().height;

	return Vec2(x, y);
}

void C_STAGE2::setPlayerPosForPlayer(cocos2d::Vec2 position)
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
					C_STAGE3* cStage3 = new C_STAGE3;
					cStage3->setDataToNextStage(m_dataFromCurrentStage);
					Director::getInstance()->pushScene(C_STAGE3::createScene());
					return;
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

void C_STAGE2::setPlayerPosForEnemy(cocos2d::Vec2 position)
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

void C_STAGE2::setViewpointCenter(cocos2d::Vec2 position)
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

void C_STAGE2::setDataToNextStage(std::map<float, int> dataFromPreviousStage)
{
	for (auto i : dataFromPreviousStage)
	{
		_bakHp = i.first;
		_bakJumpCount = i.second;
	}
}

void C_STAGE2::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
	nScene->draw(renderer, transform, flags);

	_customCmd.init(_globalZOrder, transform, flags);
	_customCmd.func = CC_CALLBACK_0(C_STAGE2::onDraw, this, transform, flags);

	renderer->addCommand(&_customCmd);
}

void C_STAGE2::onDraw(const cocos2d::Mat4& transform, uint32_t flags)
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
