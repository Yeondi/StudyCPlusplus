#include "Stage2.h"

USING_NS_CC;

Scene* C_STAGE3::createScene() //여기 데이터를 따로 옮겨서 관리
{
	Scene* scene = Scene::create();

	C_HUDLAYER* hudLayer = C_HUDLAYER::create();
	scene->addChild(hudLayer, 2);

	C_STAGE3* layer = C_STAGE3::createWithHudAndBackGround(hudLayer);
	scene->addChild(layer, 1);

	return scene;
}

bool C_STAGE3::init()
{
	if (!Layer::init())
		return false;

	visibleSize = Director::getInstance()->getVisibleSize();
	winSize = Director::getInstance()->getWinSize();
	origin = Director::getInstance()->getVisibleOrigin();
	nScene = new Scene;

	Label* label = Label::createWithSystemFont("rev_0.7.1 - Stage03", "Arial", 24);
	label->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - label->getContentSize().height));
	hudLayer->addChild(label, 1);

	initTiledMap();

	worldSize = Size(tMap->getContentSize().width, tMap->getContentSize().height);

	initBox2dWorld(true);


	player = new Player();
	player->init(C_STAGE3::hudLayer, _world, tMap);
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
			if (str == "wisp")
			{
				//enemy = new C_ENEMY();
				continue;
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
			enemy->addBodyToWorld(C_STAGE3::hudLayer, _world, tMap);
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

	//TestSprite();
	//this->addChild(entity->getBatchNode());

	scheduleUpdate();

	return true;
}

void C_STAGE3::setHUDLayer(C_HUDLAYER* layer)
{
	hudLayer = layer;
}

C_STAGE3* C_STAGE3::createWithHudAndBackGround(C_HUDLAYER* hudLayer)
{
	C_STAGE3* object = new C_STAGE3();
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

void C_STAGE3::initTiledMap()
{
	tMap = TMXTiledMap::create("BG/stage3.tmx");
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

	pMeta->setVisible(true);

	tMap->setAnchorPoint(Vec2(0.0f, 0.0f));
	tMap->setPosition(Vec2(0.0f, -14.0f));

	tMap->setVisible(true);
	this->addChild(tMap, 0, 11);
}

void C_STAGE3::initBox2dWorld(bool debug)
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

	groundBox.Set(b2Vec2(0, 26), b2Vec2(6, 26));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(6, 26), b2Vec2(8, 24));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(8, 24), b2Vec2(10, 24));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(10, 24), b2Vec2(10, 20));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(10, 20), b2Vec2(9, 20));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(9, 20), b2Vec2(9, 19));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(9, 19), b2Vec2(0, 19));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(15, 29), b2Vec2(15, 18));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(15, 18), b2Vec2(20, 18));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(20, 18), b2Vec2(20, 29));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(1, 12), b2Vec2(2, 12));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(3, 12), b2Vec2(4, 12));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(8, 14), b2Vec2(11, 14));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(16, 13), b2Vec2(19, 13));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(12, 4), b2Vec2(15, 4));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(32, 29), b2Vec2(32, 23));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(32, 23), b2Vec2(26, 23));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(26, 23), b2Vec2(24, 21));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(24, 21), b2Vec2(24, 13));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(24, 13), b2Vec2(40, 13));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(40, 13), b2Vec2(40, 17));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(40, 17), b2Vec2(36, 21));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef); //경사
	groundBox.Set(b2Vec2(36, 21), b2Vec2(36, 29));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	//구름다리
	groundBox.Set(b2Vec2(19, 5), b2Vec2(22, 5));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(30, 5), b2Vec2(33, 5));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(40, 5), b2Vec2(43, 5));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(48, 1), b2Vec2(51, 1));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(56, 5), b2Vec2(59, 5));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(61, 0), b2Vec2(61, 9));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(61, 9), b2Vec2(68, 9));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(68, 9), b2Vec2(68, 14));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(68,14), b2Vec2(69, 15));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(69, 15), b2Vec2(70, 14));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(70, 14), b2Vec2(70, 0));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(58, 10), b2Vec2(55, 10));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(52, 11), b2Vec2(49, 11));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(47, 14), b2Vec2(44, 14));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(46, 20), b2Vec2(67, 20));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(124, 3), b2Vec2(127, 3));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(136, 5), b2Vec2(139, 5));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(143, 8), b2Vec2(143, 0));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(143, 8), b2Vec2(151, 8));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(151, 8), b2Vec2(151, 9));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(151, 9), b2Vec2(153, 9));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(153, 9), b2Vec2(153, 7));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(153, 7), b2Vec2(157, 3));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(157, 3), b2Vec2(171, 3));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(171, 3), b2Vec2(171, 17));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(171, 17), b2Vec2(176,22));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(176, 22), b2Vec2(178, 22));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(178,22), b2Vec2(180, 24));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(180, 24), b2Vec2(200, 24));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(166, 6), b2Vec2(169, 6));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(159, 8), b2Vec2(162, 8));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(166, 11), b2Vec2(169, 11));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(159, 14), b2Vec2(162, 14));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(166, 17), b2Vec2(169, 17));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
	groundBox.Set(b2Vec2(159, 20), b2Vec2(162, 20));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(worldSize.width / PTM_RATIO, worldSize.height / PTM_RATIO),
		b2Vec2(worldSize.width / PTM_RATIO, 0));

	groundBox.Set(b2Vec2(0, 0), b2Vec2(0, (worldSize.height / PTM_RATIO) + 32.0f));
	_groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(0, worldSize.height / PTM_RATIO), b2Vec2(worldSize.width / PTM_RATIO,
		worldSize.height / PTM_RATIO));
	_groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(worldSize.width / PTM_RATIO, worldSize.height / PTM_RATIO),
		b2Vec2(worldSize.width / PTM_RATIO, 0));

	createTerrain();

	_groundBody->CreateFixture(&groundBoxDef);

}

void C_STAGE3::createTerrain()
{
	addNewSpriteWithBody(Vec2(75, 18), Size(2.0f, 0.5f), b2_kinematicBody, 3.0f, nullptr, 0, Entity::categoryMovingBox1);
	addNewSpriteWithBody(Vec2(80, 13), Size(2.0f, 0.5f), b2_kinematicBody, 3.5f, nullptr, 0, Entity::categoryMovingBox2);
	addNewSpriteWithBody(Vec2(72, 6), Size(2.0f, 0.5f), b2_kinematicBody, 4.0f, nullptr, 0, Entity::categoryMovingBox3);
	addNewSpriteWithBody(Vec2(79, 2), Size(2.0f, 0.5f), b2_kinematicBody, 5.0f, nullptr, 0, Entity::categoryMovingBox4);

	//b2Body* body1 = this->addNewSpriteWithBody(Vec2(98, 11), Size(0.5f, 0.5f), b2_staticBody, nullptr, 0, Entity::categoryRevJoint1);
	//localBody = this->addNewSpriteWithBody(Vec2(98, 6), Size(3.0f, 3.0f), b2_dynamicBody, nullptr , 1, Entity::categoryRevJoint2);

	//b2RevoluteJointDef revJointDef;
	//b2RevoluteJoint* revJoint;

	//revJointDef.Initialize(body1, localBody, body1->GetPosition());

	//revJointDef.enableMotor = true;
	//revJointDef.motorSpeed = 5;
	//revJointDef.maxMotorTorque = 300;

	//revJoint = (b2RevoluteJoint*)_world->CreateJoint(&revJointDef);
}

void C_STAGE3::updatePhysics(float dt)
{
	int velocityIterations = 8;
	int positionIterations = 3;
	_world->Step(dt, velocityIterations, positionIterations);
}

void C_STAGE3::update(float dt)
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

	updateContact();
	updateMovingBox();



}

cocos2d::Vec2 C_STAGE3::tileCoordForPosition(cocos2d::Vec2 position)
{
	int x = position.x / tMap->getTileSize().width;
	int y = ((tMap->getMapSize().height * tMap->getTileSize().height) - position.y) / tMap->getTileSize().height;

	return Vec2(x, y);
}

void C_STAGE3::setPlayerPosForPlayer(cocos2d::Vec2 position)
{
	Vec2 tileCoord = this->tileCoordForPosition(position);
	if (position.y <= 0.0f)
	{
		return;
	}
	else
	{
		int tileGid = this->pMeta->getTileGIDAt(tileCoord);
		int tileItem = this->pItem->getTileGIDAt(tileCoord);

		if (tileGid)
		{
			Value vProperties = tMap->getPropertiesForGID(tileGid);

			if (!vProperties.isNull())
			{
				std::string wall = vProperties.asValueMap()["type"].asString();
				if (wall == "portal")
				{
					m_dataFromCurrentStage.insert(std::map<float, int>::value_type(player->getHealth(), player->getJumpCount()));
					C_BOSS* cBoss = new C_BOSS;
					cBoss->setDataToNextStage(m_dataFromCurrentStage);
					Director::getInstance()->pushScene(C_BOSS::createScene());
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
		else if (tileItem)
		{
			Value vProperties = tMap->getPropertiesForGID(tileItem);
			if (!vProperties.isNull())
			{
				std::string itemType = vProperties.asValueMap()["type"].asString();
				std::string itemName = vProperties.asValueMap()["name"].asString();
				if (itemType == "Weapon" || itemType == "Resource")
				{
					this->pItem->removeTileAt(tileCoord);
					hudLayer->getInventory()->addItem(0, itemName);
				}
				if (itemType == "Food")
				{
					this->pItem->removeTileAt(tileCoord);
					cocos2d::log("heeeeeeeeeeeeaaaaaaaaaaaaaaaaalllllllllllllllll");
				}
			}
		}
	}
}

void C_STAGE3::setPlayerPosForEnemy(cocos2d::Vec2 position)
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

void C_STAGE3::setViewpointCenter(cocos2d::Vec2 position)
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

void C_STAGE3::updateContact()
{
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

		bool collide = (maskA & catB) != 0 && (catA & maskB) != 0;



		if (bodyA->GetUserData() != NULL && bodyB->GetUserData() != NULL)
		{
			Entity* entityA = (Entity*)bodyA->GetUserData();
			Entity* entityB = (Entity*)bodyB->GetUserData();

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
		else if (bodyA->GetUserData() != NULL)
		{
			Entity* entityA = (Entity*)bodyA->GetUserData();
			Entity* entityB = (Entity*)bodyB->GetUserData();

			if (catB == Entity::categoryRevJoint2 && catA == Entity::categoryPlayer)
			{
				if(stopDamage < 1)
					entityA->getDamage(20.0f, 0);
				stopDamage++;
			}
			else
			{
				stopDamage = 0;
			}
			auto _hpratio = entityA->getHealth() / entityA->getMaxHealth();
			hudLayer->updateHpBar(_hpratio);
			cocos2d::log("플레이어의 현재 체력 : %f", entityA->getHealth());
		}
	}
}

void C_STAGE3::updateMovingBox()
{
	for (b2Body* b = _world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetType() == b2_kinematicBody)
		{
			if (b->GetFixtureList()->GetFilterData().categoryBits == Entity::categoryMovingBox1)
			{
				b2Vec2 v = b->GetPosition();
				if (v.x > 84.0f)
				{
					b->SetLinearVelocity(b2Vec2(-3.0f, 0.0f));
				}
				else if (v.x < 75.0f)
				{
					b->SetLinearVelocity(b2Vec2(3.0f, 0.0f));
				}
			}
			if (b->GetFixtureList()->GetFilterData().categoryBits == Entity::categoryMovingBox2)
			{
				b2Vec2 v = b->GetPosition();
				if (v.x > 95.0f)
				{
					b->SetLinearVelocity(b2Vec2(-3.5f, 0.0f));
				}
				else if (v.x < 80.0f)
				{
					b->SetLinearVelocity(b2Vec2(3.5f, 0.0f));
				}
			}
			if (b->GetFixtureList()->GetFilterData().categoryBits == Entity::categoryMovingBox3)
			{
				b2Vec2 v = b->GetPosition();
				if (v.x > 82.0f)
				{
					b->SetLinearVelocity(b2Vec2(-4.0f, 0.0f));
				}
				else if (v.x < 72.0f)
				{
					b->SetLinearVelocity(b2Vec2(4.0f, 0.0f));
				}
			}
			if (b->GetFixtureList()->GetFilterData().categoryBits == Entity::categoryMovingBox4)
			{
				b2Vec2 v = b->GetPosition();
				if (v.x > 116.0f)
				{
					b->SetLinearVelocity(b2Vec2(-7.0f, 0.0f));
				}
				else if (v.x < 79.0f)
				{
					b->SetLinearVelocity(b2Vec2(7.0f, 0.0f));
				}
			}
		}
	}
}

void C_STAGE3::addNewSpriteWithBody(cocos2d::Vec2 pos, cocos2d::Size size, b2BodyType bodyType, float boxVelocity, const char* spriteName, int shape, Entity::categoryValue categoryBits)
{
	b2BodyDef bodyDef;
	bodyDef.type = bodyType;
	bodyDef.position.Set(pos.x, pos.y);
	if (boxVelocity > 0.0f)
		bodyDef.linearVelocity = b2Vec2(boxVelocity, 0);

	if (spriteName)
	{
		Sprite* sprite = Sprite::create(spriteName);
		sprite->setPosition(pos);
		this->addChild(sprite);

		bodyDef.userData = sprite;
	}
	b2Body* body = _world->CreateBody(&bodyDef);

	b2FixtureDef fixtureDef;
	b2PolygonShape dynamicBox;
	b2CircleShape circle;

	if (shape == 0)
	{
		dynamicBox.SetAsBox(size.width, size.height);
		fixtureDef.shape = &dynamicBox;
	}
	else
	{
		circle.m_radius = (size.width / 2);
		fixtureDef.shape = &circle;
	}

	fixtureDef.density = 1.0f;
	fixtureDef.friction = 10.0f;
	fixtureDef.filter.categoryBits = categoryBits;
	fixtureDef.filter.maskBits = Entity::categoryEnemy | 0x0001 | Entity::categoryPlayer | Entity::categoryEnemyBullet;
	fixtureDef.filter.groupIndex = -10;
	//fixtureDef.filter.maskBits = Entity::categoryRevJoint2;

	body->CreateFixture(&fixtureDef);
}

b2Body* C_STAGE3::addNewSpriteWithBody(cocos2d::Vec2 pos, cocos2d::Size size, b2BodyType bodyType, const char* spriteName, int shape, Entity::categoryValue categoryBits)
{
	b2BodyDef bodyDef;
	bodyDef.type = bodyType;
	bodyDef.position.Set(pos.x, pos.y);

	if (spriteName == "orange.png")
	{
		bodyDef.userData = this;
	}
	else if (spriteName)
	{
		Sprite* ball = Sprite::create(spriteName);
		ball->setPosition(pos);
		ball->setTag(TAG_NOUPDATE);
		this->addChild(ball);

		bodyDef.userData = ball;
	}
	b2Body* body = _world->CreateBody(&bodyDef);

	b2FixtureDef fixtureDef;
	b2PolygonShape dynamicBox;
	b2CircleShape circle;

	if (shape == 0)
	{
		dynamicBox.SetAsBox(size.width, size.height);
		fixtureDef.shape = &dynamicBox;
	}
	else
	{
		circle.m_radius = (size.width / 2);
		fixtureDef.shape = &circle;
	}

	fixtureDef.density = 1.0f;
	fixtureDef.filter.categoryBits = categoryBits;
	fixtureDef.filter.maskBits = Entity::categoryEnemy | 0x0001 | Entity::categoryPlayer | Entity::categoryEnemyBullet;
	fixtureDef.filter.groupIndex = -10;
	//fixtureDef.filter.maskBits = Entity::categoryMovingBox2;

	body->CreateFixture(&fixtureDef);

	return body;
}

	

void C_STAGE3::setDataToNextStage(std::map<float, int> dataFromPreviousStage)
{
	for (auto i : dataFromPreviousStage)
	{
		_bakHp = i.first;
		_bakJumpCount = i.second;
	}
}

void C_STAGE3::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
	nScene->draw(renderer, transform, flags);

	_customCmd.init(_globalZOrder, transform, flags);
	_customCmd.func = CC_CALLBACK_0(C_STAGE3::onDraw, this, transform, flags);

	renderer->addCommand(&_customCmd);
}

void C_STAGE3::onDraw(const cocos2d::Mat4& transform, uint32_t flags)
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
