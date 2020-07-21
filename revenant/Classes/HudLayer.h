#ifndef _HUD_LAYER_H_
#define _HUD_LAYER_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <functional>
#include "SneakyInput/SneakyButton.h"
#include "SneakyInput/SneakyButtonSkinnedBase.h"
#include "SneakyInput/SneakyJoystick.h"
#include "SneakyInput/SneakyJoystickSkinnedBase.h"
#include "Inventory.h"
#include "Entities/Entity.h"

class C_HUDLAYER : public cocos2d::Layer
{
public:
	virtual bool init();

	CREATE_FUNC(C_HUDLAYER);

	SneakyJoystick* leftJoystick;
	SneakyButton* action1Button;
	SneakyButton* action2Button;


	C_HUDLAYER();

	void initHpBar();
	void initJoystick();

	void updateHpBar(float ratio);

	void openInventoryCallback(cocos2d::Ref* pSender);

	C_INVENTORY* getInventory();
private:
	bool m_bInventorySwitch;
	cocos2d::Size		winSize;
	cocos2d::Size		visibleSize;
	cocos2d::Point		origin;

	cocos2d::Sprite* hpBar;
	cocos2d::Sprite* hpgauge;

	C_INVENTORY* inventory;

};

#endif