#ifndef INVENTORY_H
#define INVENTORY_H

#include "cocos2d.h"

using namespace cocos2d;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#pragma execution_character_set("utf-8")
#endif



enum
{
	MAX_ITEM_CAPACITY = 18
};

enum class consumeEffect
{
	EFFECT_ITEM_WEAPONS_LEGENDARY,
	EFFECT_ITEM_FOOD_POWERUP,
};
class C_INVENTORY : public Node
{
private:
	struct S_NODE
	{
		int nId;
		int itemLocation;
		cocos2d::Sprite* spriteData;
		S_NODE* pNext;
	};

	S_NODE* m_pBegin;
	S_NODE* m_pEnd;

public:
	//insert , remove , search , display , createNode
	static const int hashGroups = 5;
	int hashFunction(int _id);

public:
	enum class ItemType
	{
		ITEM_WEAPONS = 5003,
		ITEM_FOOD,
		ITEM_RESOURCES,

	};

	C_INVENTORY();
	virtual ~C_INVENTORY();

	virtual bool init();

	static C_INVENTORY* create();

	void setInventory();
	void addItem(int _id, std::string _str);
	void removeALLItem(Ref* pSender);
	void removeItemSpriteByKey(int _id);
	void userClickItem(Ref* pSender);

private:
	Sprite* m_Inventory;
	MenuItemImage* pMenuBox[MAX_ITEM_CAPACITY];
	Menu* pMenu[MAX_ITEM_CAPACITY];
	Label* ItemCount[MAX_ITEM_CAPACITY];
	Sprite* spriteData;

	int m_nCount;
	float m_fPlacePosX;
	float m_fPlacePosY;
	int m_nCheckCount;
	int m_nTag;


};

#endif // !INVENTORY_H
