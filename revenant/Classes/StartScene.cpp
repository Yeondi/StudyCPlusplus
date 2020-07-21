#include "StartScene.h"
#include "GameScene.h"

USING_NS_CC;

Scene* C_STARTSCENE::createScene()
{
	auto scene = Scene::create();
	auto layer = C_STARTSCENE::create();
	scene->addChild(layer);
	return scene;
}

bool C_STARTSCENE::init()
{
	if (!Layer::init())
	{
		return false;
	}
	auto wlayer = LayerColor::create(Color4B(178,204,238,255));
	this->addChild(wlayer);
	auto director = Director::getInstance();

	auto glview = director->getOpenGLView();

	winSize = glview->getDesignResolutionSize();

	initBG();
	initTitle();
	initButton();


	return true;
}

void C_STARTSCENE::initBG()
{
	back = Sprite::create("startScene/back_blue.png");
	back->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	this->addChild(back, 0);
}

void C_STARTSCENE::initTitle()
{
	auto title = Sprite::create("title_text2.png");
	title->setAnchorPoint(Vec2(0.5f, 1.0f));
	title->setScale(0.7f);
	title->setPosition(Vec2(winSize.width / 2, winSize.height - 30));

	this->addChild(title,10);

}

void C_STARTSCENE::initButton()
{
	auto button1 = MenuItemImage::create("startScene/TextBTN_New-Start.png", "startScene/TextBTN_New-Start_Pressed.png",CC_CALLBACK_1(C_STARTSCENE::startGame,this));
	button1->setPosition(Vec2(winSize.width / 2, 150));
	button1->setScale(0.4f);

	auto quit = MenuItemImage::create("startScene/quit.png", "startScene/quit_pressed.png", CC_CALLBACK_1(C_STARTSCENE::menuCloseCallback, this));
	quit->setPosition(Vec2(winSize.width / 2, 100));
	quit->setScale(0.4f);

	auto menu = Menu::create(button1,quit, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu);
}

void C_STARTSCENE::startGame(Ref* object)
{
	//auto Scene = C_STAGE3::createScene();
	auto Scene = Game::createScene();
	Director::getInstance()->pushScene(Scene);
}

void C_STARTSCENE::menuCloseCallback(cocos2d::Ref* pSender)
{
	Director::getInstance()->end();
}