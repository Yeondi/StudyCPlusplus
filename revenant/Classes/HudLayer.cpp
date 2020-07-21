#include "HudLayer.h"

USING_NS_CC;

bool C_HUDLAYER::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initHpBar();
	initJoystick();

	auto InventoryItem = MenuItemImage::create("res/Items/meso.png", "res/Items/meso.png", CC_CALLBACK_1(C_HUDLAYER::openInventoryCallback, this));

	float x = origin.x + visibleSize.width - InventoryItem->getContentSize().width / 2;
	float y = origin.y + InventoryItem->getContentSize().height / 2;
	InventoryItem->setPosition(Vec2(x, y));

	auto menu = Menu::create(InventoryItem, NULL);
	menu->setPosition(Vec2(0.0f,580.0f));
	this->addChild(menu, 1);

	Vec2 setPos = Vec2(visibleSize.width / 2 + origin.x + 200, visibleSize.height / 2 + origin.y + 100);
	if (!inventory)
	{
		inventory = C_INVENTORY::create();
		inventory->setPosition(setPos);
		inventory->setVisible(false);
		this->addChild(inventory);
	}

	leftJoystick->onVelocityChanged = [=](SneakyJoystick* eventJoystick, Point oldValue, Point newValue) {
		cocos2d::log("Velocity x: %f y: %f", newValue.x, newValue.y);
	};

	action1Button->onFire = [=](SneakyButton* button) {
		cocos2d::log("Fire in the hole!!!");
	};

	action2Button->onFire = [=](SneakyButton* attack)
	{
		cocos2d::log("Attack!!Attack!!Attack!!Attack!!Attack!!");
	};


	//람다 함수 식
	// [] = Lambda Introducer (람다 소개자) 혹은 캡쳐 절
	// () = Parameter Specifier (파라미터 지정자)
	// {} = Lambda Body (람다 몸체)

	// [] : 아무것도 캡쳐하지 않음
	// [&x] : x만 레퍼런스에 의한 캡쳐
	// [x] : x만 값에 의한 캡쳐
	// [=] : 모든 변수를 값에 의한 캡쳐
	// [&] = 모든 변수를 레퍼런스에 의한 캡쳐
	// 그 외는 검색해볼것

	return true;
}

C_HUDLAYER::C_HUDLAYER() :
	inventory{},
	m_bInventorySwitch{}
{
	visibleSize = Director::getInstance()->getVisibleSize();
	winSize = Director::getInstance()->getWinSize();
	origin = Director::getInstance()->getVisibleOrigin();
}

void C_HUDLAYER::initHpBar()
{
	hpBar = Sprite::create("hpbarX2.png");
	hpgauge = Sprite::create("hpGauge.png");

	hpBar->setPosition(Vec2(25.0f, 610.0f));
	hpBar->setAnchorPoint(Vec2(0.0f, 1.0f));

	hpgauge->setPosition(Vec2(25.0f, 610.0f));
	hpgauge->setAnchorPoint(Vec2(0.0f, 1.0f));

	hpBar->retain();
	hpgauge->retain();

	this->addChild(hpBar, 3);
	this->addChild(hpgauge, 2);
}

void C_HUDLAYER::initJoystick()
{
	// add joystick
	int joystickOffset = 10;

	Rect joystickBaseDimensions = Rect(0, 0, 64.0f, 64.0f);
	Point joystickBasePosition = Point(origin.x + (joystickBaseDimensions.size.width / 2) + joystickOffset, origin.y + (joystickBaseDimensions.size.height / 2) + joystickOffset);

	Rect accelButtonDimensions = Rect(0, 0, 64.0f, 64.0f);
	Point accelButtonPosition = Point(origin.x + visibleSize.width - (joystickBaseDimensions.size.width / 2) - joystickOffset, origin.y + (joystickBaseDimensions.size.height / 2) + joystickOffset);
	Point TempButton2Pos = Point(accelButtonPosition.x - 80, accelButtonPosition.y);

	// add joystick base
	SneakyJoystickSkinnedBase* joystickBase = SneakyJoystickSkinnedBase::create();
	SneakyJoystick* joystick = new SneakyJoystick();
	joystick->initWithRect(joystickBaseDimensions);
	joystickBase->setBackgroundSprite(cocos2d::Sprite::create("res/joystick/dpadDown.png"));
	joystickBase->setThumbSprite(cocos2d::Sprite::create("res/joystick/joystickDown.png"));
	joystickBase->setJoystick(joystick);
	joystickBase->setPosition(joystickBasePosition);
	leftJoystick = joystickBase->getJoystick();
	this->addChild(joystickBase);
	
	// add joystick button
	SneakyButtonSkinnedBase* action1ButtonBase = SneakyButtonSkinnedBase::create();
	SneakyButton* button1 = new SneakyButton();
	button1->initWithRect(accelButtonDimensions);
	action1ButtonBase->setDefaultSprite(cocos2d::Sprite::create("res/joystick/action1Down.png"));
	action1ButtonBase->setActivatedSprite(cocos2d::Sprite::create("res/joystick/action1Down.png"));
	action1ButtonBase->setPressSprite(cocos2d::Sprite::create("res/joystick/action1Up.png"));
	action1ButtonBase->setButton(button1);
	action1ButtonBase->setPosition(accelButtonPosition);
	action1Button = action1ButtonBase->getButton();
	this->addChild(action1ButtonBase);

	// add joystick button
	SneakyButtonSkinnedBase* action2ButtonBase = SneakyButtonSkinnedBase::create();
	SneakyButton* button2 = new SneakyButton();
	button2->initWithRect(accelButtonDimensions);
	action2ButtonBase->setDefaultSprite(cocos2d::Sprite::create("res/joystick/action1Down.png"));
	action2ButtonBase->setActivatedSprite(cocos2d::Sprite::create("res/joystick/action1Down.png"));
	action2ButtonBase->setPressSprite(cocos2d::Sprite::create("res/joystick/action1Up.png"));
	action2ButtonBase->setButton(button2);
	action2ButtonBase->setPosition(TempButton2Pos);
	action2ButtonBase->setScale(0.5f);
	action2Button = action2ButtonBase->getButton();
	this->addChild(action2ButtonBase);
}

void C_HUDLAYER::updateHpBar(float ratio)
{
	hpgauge->setScaleX(ratio);
}

void C_HUDLAYER::openInventoryCallback(cocos2d::Ref* pSender)
{
	if (!m_bInventorySwitch)
	{
		m_bInventorySwitch = true;
		inventory->setVisible(true);
	}
	else if (m_bInventorySwitch)
	{
		m_bInventorySwitch = false;
		inventory->setVisible(false);
	}
}

C_INVENTORY* C_HUDLAYER::getInventory()
{
	return inventory;
}
