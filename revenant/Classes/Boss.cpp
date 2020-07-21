#include "Stage2.h"

USING_NS_CC;

Scene* C_BOSS::createScene() //여기 데이터를 따로 옮겨서 관리
{
	Scene* scene = Scene::create();

	C_HUDLAYER* hudLayer = C_HUDLAYER::create();
	scene->addChild(hudLayer, 2);

	C_BOSS* layer = C_BOSS::createWithHudAndBackGround(hudLayer);
	scene->addChild(layer, 1);

	return scene;
}

bool C_BOSS::init()
{
	if (!Layer::init())
		return false;

	visibleSize = Director::getInstance()->getVisibleSize();
	winSize = Director::getInstance()->getWinSize();
	origin = Director::getInstance()->getVisibleOrigin();
	nScene = new Scene;

	Label* label = Label::createWithSystemFont("rev - Forest of Patience", "Arial", 24);
	label->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - label->getContentSize().height));
	hudLayer->addChild(label, 1);

	initTiledMap();

	worldSize = Size(tMap->getContentSize().width, tMap->getContentSize().height);

	initBox2dWorld(true);


	player = new Player();
	player->init(C_BOSS::hudLayer, _world, tMap);
	player->setDataFromPreviousStage(_bakHp, _bakJumpCount);
	this->addChild(player->getBatchNode());

	m_dataFromCurrentStage.clear();

	labelJumpCount = Label::createWithCharMap("fonts/tuffy_bold_italic-charmap.png", 48, 64, ' ');
	labelJumpCount->setScale(0.8f);
	labelJumpCount->setString(StringUtils::format("%d", player->getJumpCount() / 2));
	labelJumpCount->setPosition(Vec2(900, origin.y + visibleSize.height - label->getContentSize().height));
	hudLayer->addChild(labelJumpCount, 1);

	scheduleUpdate();

	return true;
}

void C_BOSS::setHUDLayer(C_HUDLAYER* layer)
{
	hudLayer = layer;
}

C_BOSS* C_BOSS::createWithHudAndBackGround(C_HUDLAYER* hudLayer)
{
	C_BOSS* object = new C_BOSS();
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

void C_BOSS::initTiledMap()
{
	tMap = TMXTiledMap::create("BG/testMap.tmx");
	tGround = tMap->getLayer("Ground");
	tBG = tMap->getLayer("BG");
	pMeta = tMap->getLayer("meta");
	pItem = tMap->getLayer("Items");

	pMeta->setVisible(true);

	tMap->setAnchorPoint(Vec2(0.0f, 0.0f));
	tMap->setPosition(Vec2(0.0f, -14.0f));

	tMap->setVisible(false);
	this->addChild(tMap, 0, 11);

	cocos2d::Sprite* sRedLine = Sprite::create("BG/testMap.png");
	sRedLine->setAnchorPoint(Vec2(0.0f, 0.0f));
	sRedLine->setPosition(Vec2(0.0f, -14.0f));
	this->addChild(sRedLine);
}

void C_BOSS::initBox2dWorld(bool debug)
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

void C_BOSS::createTerrain()
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

void C_BOSS::updatePhysics(float dt)
{
	int velocityIterations = 8;
	int positionIterations = 3;
	_world->Step(dt, velocityIterations, positionIterations);
}

void C_BOSS::update(float dt)
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

cocos2d::Vec2 C_BOSS::tileCoordForPosition(cocos2d::Vec2 position)
{
	int x = position.x / tMap->getTileSize().width;
	int y = ((tMap->getMapSize().height * tMap->getTileSize().height) - position.y) / tMap->getTileSize().height;

	return Vec2(x, y);
}

void C_BOSS::setPlayerPosForPlayer(cocos2d::Vec2 position)
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

void C_BOSS::setPlayerPosForEnemy(cocos2d::Vec2 position)
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

void C_BOSS::setViewpointCenter(cocos2d::Vec2 position)
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

void C_BOSS::updateContact()
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
				if (stopDamage < 1)
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

void C_BOSS::updateMovingBox()
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

void C_BOSS::addNewSpriteWithBody(cocos2d::Vec2 pos, cocos2d::Size size, b2BodyType bodyType, float boxVelocity, const char* spriteName, int shape, Entity::categoryValue categoryBits)
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

b2Body* C_BOSS::addNewSpriteWithBody(cocos2d::Vec2 pos, cocos2d::Size size, b2BodyType bodyType, const char* spriteName, int shape, Entity::categoryValue categoryBits)
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



void C_BOSS::setDataToNextStage(std::map<float, int> dataFromPreviousStage)
{
	for (auto i : dataFromPreviousStage)
	{
		_bakHp = i.first;
		_bakJumpCount = i.second;
	}
}

void C_BOSS::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
	nScene->draw(renderer, transform, flags);

	_customCmd.init(_globalZOrder, transform, flags);
	_customCmd.func = CC_CALLBACK_0(C_BOSS::onDraw, this, transform, flags);

	renderer->addCommand(&_customCmd);
}

void C_BOSS::onDraw(const cocos2d::Mat4& transform, uint32_t flags)
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
