#include "Player.h"

USING_NS_CC;

Player::Player()
{
	direction = 2; // 2 = left, 1 = right
	state = 1; // 1= default , 2 = move , 3 = jump
	nActionToken = 1; // 0 = not available , 1 = available
	nAttackToken = 0;
	_health = 100.0f;
	_maxHealth = _health;
	_mana = 50.0f;
	_MaxMana = _mana;
	_damRate = 1.0f; // 데미지 계수 : 1배
	_playerDamage = 10.0f;
}

Player::~Player(void)
{
	actionStateDefault->release();
	actionStateMoving->release();
	actionStateJumping->release();
	actionStateAttack->release();
	sprite->removeFromParentAndCleanup(true);
	body = NULL;
}

bool Player::init(C_HUDLAYER* layer, b2World* world, TMXTiledMap* map)
{
	// some variables
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();

	nJumpCount = 2;

	// node and spite
	batchNode = SpriteBatchNode::create("res/characters/revenant/revenant_original.pvr.ccz");

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/characters/revenant/revenant_original.plist");
	sprite = cocos2d::Sprite::createWithSpriteFrameName("revenant_idle_00.png");

	sprite->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	sprite->setTag(TAG_PLAYER);

	// animation for default state
	animationDefault = Animation::create();

	for (int i = 0; i < 4; i++)
	{
		char szImageFileName[128] = { 0 };
		sprintf(szImageFileName, "revenant_idle_%02d.png", i);
		animationDefault->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(szImageFileName));
	}

	animationDefault->setDelayPerUnit(0.13f);
	animationDefault->setRestoreOriginalFrame(true);

	animateDefault = cocos2d::Animate::create(animationDefault);
	actionStateDefault = RepeatForever::create(animateDefault);
	actionStateDefault->retain();

	// animation for moving state
	animationMoving = Animation::create();

	for (int i = 0; i < 6; i++)
	{
		char szImageFileName[128] = { 0 };
		sprintf(szImageFileName, "revenant_run_%02d.png", i);
		animationMoving->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(szImageFileName));
	}

	animationMoving->setDelayPerUnit(0.1f);
	animationMoving->setRestoreOriginalFrame(true);

	animateMoving = cocos2d::Animate::create(animationMoving);
	actionStateMoving = RepeatForever::create(animateMoving);
	actionStateMoving->retain();

	// animation for Jump state
	animationJumping = Animation::create();
	for (int i = 0; i < 2; i++)
	{
		char szImageFileName[128] = { 0 };
		sprintf(szImageFileName, "revenant_jump_%02d.png", i);
		animationJumping->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(szImageFileName));
	}

	animationJumping->setDelayPerUnit(1.0f);
	animationJumping->setRestoreOriginalFrame(true);

	animateJumping = cocos2d::Animate::create(animationJumping);
	actionStateJumping = RepeatForever::create(animateJumping);
	actionStateJumping->retain();

	// animation for Attack state
	animationAttack = Animation::create();
	for (int i = 0; i < 7; i++)
	{
		char szImageFileName[128] = { 0 };
		sprintf(szImageFileName, "revenant_attack_%02d.png", i);
		animationAttack->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(szImageFileName));
	}

	animationAttack->setDelayPerUnit(0.1f);
	animationAttack->setRestoreOriginalFrame(false);

	animateAttack = cocos2d::Animate::create(animationAttack);
	actionStateAttack = RepeatForever::create(animateAttack);
	actionStateAttack->retain();


	_map = map;
	auto pos = _map->getObjectGroup("Objects")->getObject("Player");
	Vec2 temp{};
	if (!pos.empty())
	{
		temp = Vec2(pos["x"].asInt(), pos["y"].asInt());
		_pos.x = temp.x / map->getTileSize().width;
		_pos.y = map->getMapSize().height - ((map->getMapSize().height * map->getTileSize().height - temp.y) / map->getTileSize().height);
	}




	// create physics
	this->world = world;
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(_pos.x, _pos.y);
	bodyDef.userData = this;
	bodyDef.fixedRotation = true;
	body = world->CreateBody(&bodyDef);

	b2PolygonShape shape;
	shape.SetAsBox(0.6f, 1.0f); // 0.8 1.35 default

	b2FixtureDef playerShapeDef;
	playerShapeDef.shape = &shape;
	playerShapeDef.density = 0.1f; //밀도
	playerShapeDef.friction = 2.0f; //마찰
	playerShapeDef.filter.categoryBits = categoryPlayer;
	playerShapeDef.filter.maskBits = 0x0001 | categoryEnemy | categoryEnemyBullet | categoryRevJoint2;
	playerShapeDef.filter.groupIndex = 10;
	//shapeDef.restitution = 0; //반발력
	body->CreateFixture(&playerShapeDef);

	// set default state
	setStateDefault();
	changeDirection(direction);

	batchNode->addChild(sprite);

	return true;
}

void Player::update(float dt)
{
	if (world)
	{
		for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
		{
			if (b->GetUserData() != nullptr)
			{
				Player* userData = (Player*)b->GetUserData();

				if (userData->getName() != "ballorange")
				{
					userData->sprite->setPosition(Vec2(b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO));
					userData->sprite->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));
					auto ddda = userData->sprite->getTextureAtlas()->getTexture()->getPath();
					if (userData->sprite->getTag() == TAG_PLAYER)
						vBackupPos = Vec2(userData->getLocationX(), userData->getLocationY());
				}
			}
		}
	}
}

void Player::updateVelocity(Point velocity)
{
	move(velocity);
}

void Player::move(Point velocity)
{
	b2Vec2 impulse;
	float fForce = 0.35f;

	if (velocity.x > 0)
	{
		// move right
		setStateMoving();
		changeDirection(1);

		impulse = b2Vec2(fForce * body->GetMass(), 0.0f);
		body->ApplyLinearImpulse(impulse, body->GetWorldCenter(), true);
	}
	else if (velocity.x < 0)
	{
		// move left
		setStateMoving();
		changeDirection(2);

		impulse = b2Vec2(-fForce * body->GetMass(), 0.0f);
		body->ApplyLinearImpulse(impulse, body->GetWorldCenter(), true);
	}
	else if (pVelocity.x > 0)
	{
		if (velocity.x == 0)
		{
			changeDirection(2);
			changeDirection(1);
		}
	}
	else if (pVelocity.x < 0)
	{
		if (velocity.x == 0)
		{
			changeDirection(1);
			changeDirection(2);
		}
	}
	else
	{
		if (state == 4 && nActionToken == 1)
		{
			nActionToken = 0;
			return;
		}
		else
		{
			if (nActionToken == 0)
			{
				nAttackToken++;

				if (nAttackToken == 32 && state != 3)
				{
					// not moving
					setStateDefault();
					nActionToken = 1;
					nAttackToken = 0;
				}
			}
			else
			{
				setStateDefault();
			}
		}


		fForce = 0;
		body->ApplyForce(b2Vec2(fForce, 0.0f), body->GetWorldCenter(), true);
	}
	pVelocity = velocity;
}

void Player::setStateDefault()
{
	if (state != 1)
	{
		state = 1;
		sprite->stopAllActions();
		sprite->runAction(actionStateDefault);
	}
}

void Player::setStateMoving()
{
	if (state != 2)
	{
		state = 2;
		sprite->stopAllActions();
		sprite->runAction(actionStateMoving);
	}
}

void Player::setStateJumping()
{
	if (state != 3)
	{
		state = 3;
		sprite->pause();
		sprite->stopAllActions();
		sprite->runAction(actionStateJumping);
	}
}

void Player::setStateAttack()
{
	if (state != 4)
	{
		state = 4;
		sprite->stopAllActions();
		sprite->runAction(actionStateAttack);
	}
}

void Player::stopMoving()
{
	if (state != 1)
	{
		body->SetLinearVelocity(b2Vec2(0, 0));
	}
}

void Player::actionButtonPressed(int button)
{
	if (nJumpCount > 0)
	{
		nJumpCount--;
		if (button == 1)
		{
			body->ApplyLinearImpulse(b2Vec2(0, 1.5f * body->GetMass() * 3), body->GetWorldCenter(), true);

			setStateJumping();
		}
	}
}

void Player::attackButtonPressed(int button)
{
	if (nActionToken == button)
	{
		setStateAttack();
	}
}

void Player::changeDirection(int direction)
{
	if (this->direction != direction && direction == 1)
	{
		sprite->setFlippedX(true);
		this->direction = direction;

		if (state == 2)
		{
			stopMoving();
		}
	}
	else if (this->direction != direction && direction == 2)
	{
		sprite->setFlippedX(false);
		this->direction = direction;

		if (state == 2)
		{
			stopMoving();
		}
	}
}

void Player::setDataFromPreviousStage(float health, int jump)
{
	if (health > 0.0f && health <= 100.0f)
	{
		_health = health;
		nJumpCount = jump;
	}
	else
	{
		_health = 100.0f;
		nJumpCount = 2;
	}
}

void Player::setTMXMap(TMXTiledMap* map)
{
	_map = map;
	auto pos = _map->getObjectGroup("Objects")->getObject("Player");
	if (!pos.empty())
	{
		_pos = Vec2(pos["x"].asInt(), pos["y"].asInt());
	}
}

int Player::getTag()
{
	return Entity::TAG_PLAYER;
}

Player* Player::getPlayer()
{
	return this;
}

void Player::addJumpCount(int _add)
{
	nJumpCount += _add;
}

cocos2d::Vec2 Player::getBackupPos()
{
	return cocos2d::Vec2(vBackupPos);
}

void Player::setBackupPos(cocos2d::Vec2 pos)
{
	vBackupPos = pos;
}

void Player::getDamage(float damage, int attribute)
{
	float _healthRatio = 0.f;
	if (damage > 0)
	{//혹시 모를 속성에 따른 계산
		if (isCounter(_attribute, attribute))
		{
			//setStateHurt();
			_health -= damage * _damRate * _attributeRate;
		}
		else
		{
			_health -= damage * _damRate;
		}
	}
	if (_health > _maxHealth)
		_health = _maxHealth;

	_healthRatio = _health / _maxHealth;
	if (_healthRatio > 0.0f)
	{

	}
	if (_health <= 0.0f)
	{
		//gameover
		Director::getInstance()->replaceScene(C_STARTSCENE::createScene());
	}
}

bool Player::isCounter(int attack, int defense)
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

float Player::getOnesDamage()
{
	return _playerDamage;
}

float Player::getHealth()
{
	return _health;
}

float Player::getMaxHealth()
{
	return _maxHealth;
}

