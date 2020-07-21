#include "Inventory.h"

USING_NS_CC;

int C_INVENTORY::hashFunction(int _id)
{
	return _id % hashGroups; // 5011 -> 1
}

C_INVENTORY::C_INVENTORY() :
	m_pBegin{},
	m_pEnd{},
	m_nCheckCount{},
	m_nCount{}
{
	m_nTag = 5023;
}

C_INVENTORY::~C_INVENTORY()
{
}

bool C_INVENTORY::init()
{
	CCAssert(Node::init(), "Node init Failed");

	setInventory();



	return true;
}

C_INVENTORY* C_INVENTORY::create()
{
	C_INVENTORY* pNewInven = new C_INVENTORY();
	if (pNewInven && pNewInven->init())
	{
		pNewInven->autorelease();
		return pNewInven;
	}
	else
	{
		CC_SAFE_DELETE(pNewInven);
		return nullptr;
	}
}

void C_INVENTORY::setInventory()
{
	//m_Inventory = Sprite::create("res/inventory.png");
	m_Inventory = Sprite::create("res/Items/inv_bak.png");
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Vec2 setPos = Vec2(visibleSize.width / 2 + origin.x + 200, visibleSize.height / 2 + origin.y + 100);
	Vec2 pos = Vec2(m_Inventory->getContentSize().width, m_Inventory->getContentSize().height);

	m_Inventory->setPosition(Vec2(pos.x, pos.y - 100));
	m_Inventory->setAnchorPoint(Vec2(0.0f, 1.0f));
	this->addChild(m_Inventory, 10);


	float fItemPosX = 0.f;
	float fItemPosY = pos.y;
	for (int i = 0; i < MAX_ITEM_CAPACITY; i++)
	{
		if (i % 3 == 0)
		{
			fItemPosX = 0.0f;
			if (m_nCount == 3)
				fItemPosY -= 38.0f;
			m_nCount = 0;
		}
		fItemPosX = 38 * m_nCount;
		if (i == 0)
			pMenuBox[i] = MenuItemImage::create("res/Items/weapon.png", "res/Items/weapon_pressed.png", CC_CALLBACK_1(C_INVENTORY::userClickItem, this));
		else if (i == 1)
			pMenuBox[i] = MenuItemImage::create("res/Items/food.png", "res/Items/food_pressed.png", CC_CALLBACK_1(C_INVENTORY::userClickItem, this));
		else if (i == 2)
			pMenuBox[i] = MenuItemImage::create("res/Items/resource.png", "res/Items/resource_pressed.png", CC_CALLBACK_1(C_INVENTORY::userClickItem, this));

		if (i != 0 && i % 3 == 0)
			pMenuBox[i] = MenuItemImage::create("res/inv_left.png", "res/inv_left.png", CC_CALLBACK_1(C_INVENTORY::userClickItem, this));
		else if (i != 1 && i % 3 == 1)
			pMenuBox[i] = MenuItemImage::create("res/inv_center.png", "res/inv_center.png", CC_CALLBACK_1(C_INVENTORY::userClickItem, this));
		else if (i != 2 && i % 3 == 2)
			pMenuBox[i] = MenuItemImage::create("res/inv_right.png", "res/inv_right.png", CC_CALLBACK_1(C_INVENTORY::userClickItem, this));

		pMenuBox[i]->setAnchorPoint(Vec2(0.0f, 1.0f));
		pMenu[i] = Menu::createWithItem(pMenuBox[i]);
		pMenu[i]->setPosition(Vec2(fItemPosX, fItemPosY));
		m_Inventory->addChild(pMenu[i], 11);
		pMenuBox[i]->setTag(i + 5000);
		pMenu[i]->setTag(i + 5000);
		ItemCount[i] = Label::createWithSystemFont("", "Arial", 12);
		ItemCount[i]->setPosition(Vec2(fItemPosX + 10.0f, fItemPosY - 10.0f));
		m_Inventory->addChild(ItemCount[i], 12);
		m_nCount++;
	}
	pos = m_Inventory->getPosition();
	MenuItemImage* pXbox = MenuItemImage::create("res/close.png", "res/close.png", CC_CALLBACK_1(C_INVENTORY::removeALLItem, this));
	Menu* pClose = Menu::createWithItem(pXbox);
	m_Inventory->addChild(pClose);
	pClose->setPosition(Vec2(114.0f, 240.0f));
	pClose->setAnchorPoint(Vec2(1.0f, 1.0f));
	pXbox->setTag(50);


}

void C_INVENTORY::addItem(int _id, std::string _str)
{
	std::string strPath = "res/Items/";
	std::string strPng = ".png";
	_id = m_nTag;
	Vec2 pos = Vec2(m_Inventory->getContentSize().width, m_Inventory->getContentSize().height);

	S_NODE* pCheck = m_pBegin;
	if (pCheck)
	{
		m_nCheckCount++;
		pCheck = pCheck->pNext;
		if (m_nCheckCount >= MAX_ITEM_CAPACITY - 3)
		{
			log("더 이상 아이템을 획득할 수 없습니다.");
			return;
		}
	}

	spriteData = Sprite::create(strPath + _str + strPng);

	S_NODE* pNewNode = new S_NODE{ _id,m_nCount,spriteData,nullptr };

	if (m_nCount >= 18)
		m_nCount = 3;
	Vec2 getPos = pMenu[m_nCount]->getPosition();

	if (!m_pBegin)
	{
		m_pBegin = pNewNode;
		m_pEnd = pNewNode;
		m_fPlacePosX = 3.0f;
		m_fPlacePosY = pos.y - 40.0f;
		//m_nCount = 0;
	}
	else
	{
		m_pEnd->pNext = pNewNode;
		m_pEnd = pNewNode;
	}

	log("%d : %s\n", _id, _str.c_str());
	m_pEnd->spriteData->setPosition(Vec2(getPos.x + 3.0f, getPos.y - 2.0f));
	m_pEnd->spriteData->setTag(_id);
	m_pEnd->spriteData->setAnchorPoint(Vec2(0.0f, 1.0f));
	m_Inventory->addChild(m_pEnd->spriteData, 20);

	m_nCount++;
	m_nTag++;


}

void C_INVENTORY::removeALLItem(Ref* pSender)
{
	S_NODE* pFind = m_pBegin;
	S_NODE* pUp = nullptr;

	while (pFind)
	{
		S_NODE* pTmpNext = pFind->pNext;
		S_NODE* pDel = pFind;

		if (m_pBegin == pFind)
			m_pBegin = pTmpNext;
		else
			pUp->pNext = pTmpNext;

		m_Inventory->removeChildByTag(pFind->nId);
		delete pDel;

		if (pFind == m_pEnd)
			m_pEnd = pUp;

		pFind = pTmpNext;
	}
	m_nCheckCount = 0;
}

void C_INVENTORY::removeItemSpriteByKey(int _id)
{
	S_NODE* pFind = m_pBegin;
	S_NODE* pUp = nullptr;

	while (pFind)
	{
		S_NODE* pTmpNext = pFind->pNext;
		if (pFind->itemLocation == _id % 5000)
		{
			S_NODE* pDel = pFind;
			if (m_pBegin == pFind)
				m_pBegin = pTmpNext;
			else
				pUp->pNext = pTmpNext;

			m_nCheckCount--;
			m_Inventory->removeChildByTag(pFind->nId);
			pDel->itemLocation = NULL;
			pDel->nId = NULL;
			pDel->spriteData = nullptr;
			//delete pDel;
		}
		else
			pUp = pFind;
		if (pFind == m_pEnd)
			m_pEnd = pUp;

		pFind = pTmpNext;
	}
}



void C_INVENTORY::userClickItem(Ref* pSender)
{
	auto tItem = (Menu*)pSender;
	auto tag = tItem->getTag();



	if (tag == 5000)
	{
		log("장비창 연결 알고리즘");
	}
	else if (tag == 5001)
	{
		log("소비창 연결 알고리즘");
	}
	else if (tag == 5002)
	{
		log("재료창 연결 알고리즘");
	}
	else
	{
		cocos2d::log("USER SELECT SOMETHING SENDER %d , %d", tag);
		removeItemSpriteByKey(tag);
	}
}