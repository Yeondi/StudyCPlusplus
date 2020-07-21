#include "Enemy.h"
#include "enemyState.h"

USING_NS_CC;

C_ENEMY::C_ENEMY() :
	m_pCurrentState(StayEnemy::Instance()),
	m_pNewState{},
	IsWall{}
{
	direction = 2; // 1 = left, 2 = right
	state = 1; // 1= default , 2 = move , 3 = jump

	_enemyDamage = 5.0f;

	//잔머리
	wakeDistance = 230.0f;
	fTimerForStay = 180.0f; // 60프레임 기준
	fMovingTimer = 420.0f;

	bullet = nullptr;
}

C_ENEMY::~C_ENEMY()
{
	actionStateDefault->release();
	actionStateMoving->release();
	actionStateAttack->release();
	actionStateDie->release();
	actionBall->release();
	sprite->removeFromParentAndCleanup(true);
	bullet = NULL;
	enemyBody = NULL;
}

bool C_ENEMY::init()
{
	if (!Node::init())
		return false;

	_health = 50.0f;
	_MaxHealth = _health;
	_speed = 8.0f;
	_attribute = 2;
	_damRate = 1.0f;
	_attributeRate = 1.0f;
	return true;
}

bool C_ENEMY::initEnemy(b2World* world, cocos2d::TMXTiledMap* map)
{
	_world = world;
	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

	//batchNode = SpriteBatchNode::create("res/characters/shade/shade.pvr.ccz");
	batchNode = SpriteBatchNode::create("res/characters/shade/shade_sample_2.pvr.ccz");

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/characters/shade/shade_sample_2.plist");
	sprite = cocos2d::Sprite::createWithSpriteFrameName("shade-idle-00.png");

	sprite->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	sprite->setTag(TAG_ENEMY);

	//// animation for default state
	animationDefault = Animation::create();

	for (int i = 0; i < 4; i++)
	{
		char szImageFileName[128] = { 0 };
		sprintf(szImageFileName, "shade-idle-%02d.png", i);
		animationDefault->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(szImageFileName));
	}

	animationDefault->setDelayPerUnit(0.13f);
	animationDefault->setRestoreOriginalFrame(true);

	animateDefault = cocos2d::Animate::create(animationDefault);
	actionStateDefault = RepeatForever::create(animateDefault);
	actionStateDefault->retain();

	// animation for moving state
	animationMoving = Animation::create();

	for (int i = 0; i < 4; i++)
	{
		char szImageFileName[128] = { 0 };
		sprintf(szImageFileName, "shade-move-%02d.png", i);
		animationMoving->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(szImageFileName));
	}

	animationMoving->setDelayPerUnit(0.1f);
	animationMoving->setRestoreOriginalFrame(true);

	animateMoving = cocos2d::Animate::create(animationMoving);
	actionStateMoving = RepeatForever::create(animateMoving);
	actionStateMoving->retain();

	// animation for Attack state
	animationAttack = Animation::create();
	for (int i = 0; i < 7; i++)
	{
		char szImageFileName[128] = { 0 };
		sprintf(szImageFileName, "shade-attack-%02d.png", i);
		animationAttack->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(szImageFileName));
	}

	animationAttack->setDelayPerUnit(0.1f);
	animationAttack->setRestoreOriginalFrame(false);

	animateAttack = cocos2d::Animate::create(animationAttack);
	actionStateAttack = RepeatForever::create(animateAttack);
	actionStateAttack->retain();

	// animation for Die state
	animationDie = Animation::create();
	for (int i = 0; i < 9; i++)
	{
		char szImageFileName[128] = { 0 };
		sprintf(szImageFileName, "shade-die-%02d.png", i);
		animationDie->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(szImageFileName));
	}

	animationDie->setDelayPerUnit(0.1f);
	animationDie->setRestoreOriginalFrame(false);

	animateDie = cocos2d::Animate::create(animationDie);
	actionStateDie = RepeatForever::create(animateDie);
	actionStateDie->retain();

	animationBall = Animation::create();
	for (int i = 0; i < 4; i++)
	{
		char szImageFileName[128] = { 0 };
		sprintf(szImageFileName, "shade-projectile-%02d.png", i);
		animationBall->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(szImageFileName));
	}

	animationBall->setDelayPerUnit(0.1f);
	animationBall->setRestoreOriginalFrame(false);

	animateBall = cocos2d::Animate::create(animationBall);
	actionBall = RepeatForever::create(animateBall);
	actionBall->retain();


	createTest();

	return true;
}


void C_ENEMY::addBodyToWorld(C_HUDLAYER* layer, b2World* world, cocos2d::TMXTiledMap* map)
{
	// create physics
	this->world = world;
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(_pos.x, _pos.y);

	bodyDef.userData = this;
	bodyDef.fixedRotation = true;
	enemyBody = world->CreateBody(&bodyDef);
	b2PolygonShape shape;
	shape.SetAsBox(0.7f, 0.8f); // 0.8 1.35 default

	b2FixtureDef enemyShapeDef;
	enemyShapeDef.shape = &shape;
	enemyShapeDef.density = 0.1f; //밀도
	enemyShapeDef.friction = 2.0f; //마찰
	enemyShapeDef.filter.categoryBits = categoryEnemy;
	enemyShapeDef.filter.maskBits = categoryEnemy | 0x0001 | categoryPlayer;
	enemyBody->CreateFixture(&enemyShapeDef);



	// set default state
	setStateDefault();
	changeDirection(direction);

	batchNode->addChild(sprite);

}

void C_ENEMY::update(float dt)
{
	if (world)
	{
		for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
		{
			if (b->GetUserData() != nullptr)
			{
				C_ENEMY* userData = (C_ENEMY*)b->GetUserData();

				m_pCurrentState->Execute(this);

				//aiAction(dt);
				doUpdate();

				if (userData->sprite->getTag() == TAG_ENEMY)
				{
					for (auto i : m_pEnemy)
					{
						if (i.second == Tag && i.first->sprite != nullptr)
						{
							pBackupPos = i.first->sprite->getPosition();
							return;
						}
					}
				}
				else if (userData->sprite->getTag() == TAG_PLAYER)
				{
					pBackupPos = userData->sprite->getPosition();
					m_pPlayer->setBackupPos(pBackupPos);
				}
			}
		}
	}

}

void C_ENEMY::stopMoving()
{
	if (state != 1)
	{
		enemyBody->SetLinearVelocity(b2Vec2(0, 0));
	}
}

void C_ENEMY::attackButtonPressed(int button)
{
	setStateAttack();
	//nActionToken = 0;
}

void C_ENEMY::setStateDefault()
{
	if (state != 1 && sprite)
	{
		state = 1;
		sprite->stopAllActions();
		sprite->runAction(actionStateDefault);
	}
}

void C_ENEMY::setStateMoving()
{
	if (state != 2 && sprite)
	{
		state = 2;
		sprite->stopAllActions();
		sprite->runAction(actionStateMoving);
	}
}

void C_ENEMY::setStateJumping()
{
	if (state != 3 && sprite)
	{
		state = 3;
		sprite->pause();
		sprite->stopAllActions();
		sprite->runAction(actionStateJumping);
	}
}

void C_ENEMY::setStateAttack()
{
	if (state != 4 && sprite)
	{
		state = 4;
		sprite->stopAllActions();
		sprite->runAction(actionStateAttack);
	}
}

void C_ENEMY::changeDirection(int direction)
{
	if (this->direction != direction && direction == 1 && sprite)
	{
		sprite->setFlippedX(true);
		this->direction = direction;

		if (state == 2)
		{
			stopMoving();
		}
	}
	else if (this->direction != direction && direction == 2 && sprite)
	{
		sprite->setFlippedX(false);
		this->direction = direction;

		if (state == 2)
		{
			stopMoving();
		}
	}
}

void C_ENEMY::setTMXMap(cocos2d::TMXTiledMap* map, ValueMap& pos)
{
	_map = map;
	Vec2 temp;
	int x = 0;
	int y = 0;
	if (!pos.empty())
	{
		temp = Vec2(pos["x"].asInt(), pos["y"].asInt());
		x = temp.x / map->getTileSize().width;
		y = ((map->getMapSize().height * map->getTileSize().height) - temp.y) / map->getTileSize().height;
	}
	_pos = Vec2(x, y);
}

int C_ENEMY::getTag()
{
	return Entity::TAG_ENEMY;
}

cocos2d::Vec2 C_ENEMY::getBackupPos()
{
	if (!m_pEnemy.empty())
	{
		for (auto i : m_pEnemy)
		{
			if (i.second == Tag)
			{
				return i.first->pBackupPos;
			}
		}
	}
	return Vec2(0.0f, 0.0f);
}

void C_ENEMY::getDamage(float damage, int attribute)
{
	if (damage > 0)
	{//혹시 모를 속성에 따른 계산
		if (isCounter(attribute, _attribute))
		{
			//setStateHurt();
			_health -= damage * _damRate * _attributeRate;
		}
		else
		{
			_health -= damage * _damRate;
		}
	}
	if (_health > _MaxHealth)
		_health = _MaxHealth;
}

bool C_ENEMY::isCounter(int attack, int defense)
{
	// 1 = water , 2 = fire , 3 = earth , 4 = Wind   / 물>땅>불>바람>물
	_isATTCounter = false;
	if (attack == 1)
	{
		if (defense == 1)
		{
			_attributeRate = 1.0f;
			_isATTCounter = false;
		}
		else if (defense == 2)
		{
			_attributeRate = 3.0f;
			_isATTCounter = true;
		}
		else if (defense == 3)
		{
			_attributeRate = 2.0f;
			_isATTCounter = true;
		}
		else if (defense == 4)
		{
			_attributeRate = 0.5f;
			_isATTCounter = false;
		}
	}
	else if (attack == 2)
	{
		if (defense == 1)
		{
			_attributeRate = 0.5f;
			_isATTCounter = false;
		}
		else if (defense == 2)
		{
			_attributeRate = 1.0f;
			_isATTCounter = false;
		}
		else if (defense == 3)
		{
			_attributeRate = 0.5f;
			_isATTCounter = false;
		}
		else if (defense == 4)
		{
			_attributeRate = 3.5f;
			_isATTCounter = true;
		}
	}
	else if (attack == 3)
	{
		if (defense == 1)
		{
			_attributeRate = 0.5f;
			_isATTCounter = false;
		}
		else if (defense == 2)
		{
			_attributeRate = 3.0f;
			_isATTCounter = true;
		}
		else if (defense == 3)
		{
			_attributeRate = 1.0f;
			_isATTCounter = false;
		}
		else if (defense == 4)
		{
			_attributeRate = 1.0f;
			_isATTCounter = false;
		}
	}
	else if (attack == 4)
	{
		if (defense == 1)
		{
			_attributeRate = 3.0f;
			_isATTCounter = true;
		}
		else if (defense == 2)
		{
			_attributeRate = 0.5f;
			_isATTCounter = false;
		}
		else if (defense == 3)
		{
			_attributeRate = 1.0f;
			_isATTCounter = false;
		}
		else if (defense == 4)
		{
			_attributeRate = 1.0f;
			_isATTCounter = false;
		}
	}
	else if (attack == 0)
	{
		_attributeRate = 1.0f;
		_isATTCounter = false;
	}
	return _isATTCounter;
}

float C_ENEMY::getHealth()
{
	return _health;
}

float C_ENEMY::getMaxHealth()
{
	return _MaxHealth;
}

void C_ENEMY::getCleanUp()
{
	sprite->removeChild(sprite, true);
	sprite = nullptr;
}

float C_ENEMY::getOnesDamage()
{
	return _enemyDamage;
}

void C_ENEMY::doUpdate()
{
	if (m_pCurrentState)
	{
		m_pCurrentState->Execute(this);
	}
}

bool C_ENEMY::IsState(C_STATE& state)
{
	if (m_pCurrentState == &state)
	{
		return true;
	}
	return false;
}

void C_ENEMY::ChangeState(C_STATE* pNewState)
{
	assert(m_pCurrentState && pNewState);

	m_pCurrentState->Exit(this);
	m_pCurrentState = pNewState;
	m_pCurrentState->Enter(this);
}

void C_ENEMY::StartMoving()
{
	cocos2d::log("C_ENEMY::StartMoving");
}

void C_ENEMY::startAttack()
{
	//여기서 makeBullet을 하되, 어떤 id를 가진 enemy만
	for (auto i : m_pEnemy)
	{
		if (i.second == Tag)
		{
			createBullet();
			makeBullet();
			auto force = 0.035f;
			auto hyp = m_pPlayer->getBody()->GetPosition() - enemyBody->GetPosition();
			hyp.Normalize();
			bullet->ApplyLinearImpulse(b2Vec2(force * hyp.x, 0.0f), enemyBody->GetWorldCenter(), true);
		}
	}
}

void C_ENEMY::makeBullet()
{
	if (bullet == nullptr)
	{
		for (auto i : m_pEnemy)
		{
			if (i.second == Tag)
			{
				float x = i.first->pBackupPos.x / 32.0f;
				if (playerDirection == 2)
					x -= 0.5;
				else if (playerDirection == 1)
					x += 0.5;
				float fakeY = ((15.0f * 32.0f) - i.first->pBackupPos.y);
				float y = ((15.0f * 32.0f) - fakeY) / 32.0f;
				this->world = world;
				b2BodyDef bodyDef;
				bodyDef.type = b2_dynamicBody;
				bodyDef.position.Set(x, y); //어떤 id를 가진 enemy의 좌표

				bodyDef.userData = this;
				//bodyDef.userData = nullptr;
				bodyDef.fixedRotation = true;
				bodyDef.bullet = true;
				bodyDef.active = true;
				bodyDef.linearDamping = 0.0f;
				bodyDef.angularDamping = 0.01f;
				bullet = world->CreateBody(&bodyDef);

				b2CircleShape shape;
				shape.m_radius = 0.2f;

				b2FixtureDef shapeDef;
				shapeDef.shape = &shape;
				shapeDef.density = 0.3f; //밀도
				shapeDef.friction = 0.01f; //마찰
				shapeDef.filter.categoryBits = categoryEnemyBullet;
				shapeDef.filter.maskBits = 0x0001 | categoryPlayer | categoryPlayerBullet;
				//shapeDef.restitution = 0; //반발력
				bullet->SetBullet(true);
				bullet->CreateFixture(&shapeDef);
			}
		}
	}
}

void C_ENEMY::createBullet()
{
	sprite = Sprite::createWithSpriteFrameName("shade-projectile-00.png");
	sprite->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	batchNode->addChild(sprite);
	//ball = Sprite::createWithSpriteFrameName("shade-projectile-00.png");
	//ball->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	//batchNode->addChild(ball);
}

bool C_ENEMY::detectTarget()
{
	//어떤 id를 가진 Enemy의 -200.0f && 200.0f 를 가진 범위 안에 player가 있을때
	for (auto i : m_pEnemy)
	{
		if (i.second == Tag)
		{
			if (i.first->getBackupPos().x - 200.0f <= m_pPlayer->getBackupPos().x && i.first->getBackupPos().x >= m_pPlayer->getBackupPos().x)
			{
				cocos2d::log("found Player! N.1");
				playerDirection = 2;
				return true;
			}
			else if (i.first->getBackupPos().x + 200.0f >= m_pPlayer->getBackupPos().x && i.first->getBackupPos().x <= m_pPlayer->getBackupPos().x)
			{
				cocos2d::log("found Player! on my right!");
				playerDirection = 1;
				return true;
			}
		}
	}
	return false;
}

void C_ENEMY::setNumTag(int _Tag)
{
	Tag = _Tag;
}

void C_ENEMY::setData(std::map<C_ENEMY*, int> _Data)
{
	for (auto i : _Data)
	{
		m_pEnemy.insert(i);
	}
}

std::map<C_ENEMY*, int> C_ENEMY::getData()
{
	return m_pEnemy;
}

void C_ENEMY::setPlayer(Player* player)
{
	m_pPlayer = player;
}

void C_ENEMY::createTest()
{
	b2Body* body1 = this->addNewSpriteWithBody(Vec2(98, 11), Size(0.5f, 0.5f), b2_staticBody, nullptr, 0, Entity::categoryRevJoint1);
	localBody = this->addNewSpriteWithBody(Vec2(98, 6), Size(3.0f, 3.0f), b2_dynamicBody, nullptr, 1, Entity::categoryRevJoint2);

	b2RevoluteJointDef revJointDef;
	b2RevoluteJoint* revJoint;

	revJointDef.Initialize(body1, localBody, body1->GetPosition());

	revJointDef.enableMotor = true;
	revJointDef.motorSpeed = 5;
	revJointDef.maxMotorTorque = 300;

	revJoint = (b2RevoluteJoint*)_world->CreateJoint(&revJointDef);
}

b2Body* C_ENEMY::addNewSpriteWithBody(cocos2d::Vec2 pos, cocos2d::Size size, b2BodyType bodyType, const char* spriteName, int shape, Entity::categoryValue categoryBits)
{
	b2BodyDef bodyDef;
	bodyDef.type = bodyType;
	bodyDef.position.Set(pos.x, pos.y);

	if (spriteName)
	{
		//auto ball = Sprite::createWithTexture(texture, Rect(0, 0, 37, 37));
		//ball->setName("ballorange");
		//ball->setPosition(pos.x, pos.y);
		//this->addChild(ball);

		//bodyDef.userData = ball;
		ball = Sprite::createWithSpriteFrameName("orange.png");
		ball->setName("ballorange");
		ball->setPosition(pos.x, pos.y);
		

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

