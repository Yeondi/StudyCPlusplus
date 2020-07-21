//
//  AppManager.h
//
//  Created by Choi Kyu Hwan on 2015. 7. 24..
//
//

#ifndef KH_AppManager_h
#define KH_AppManager_h

#include "cocos2d.h"
USING_NS_CC;







//----------------------------------------------------------------------------------------
//
// >> KhSceneData
//    : Scene간 넘길 데이터
//    : 필요한 데이터는 추가로 등록하여 사용한다.
//
//    >> CC_SYNTHESIZE(데이터타입, 변수명, get/set뒤에 붙을 이름);
//    ex) CC_SYNTHESIZE(std::string, prev_scene_name, PrevSceneName);
//        KhSceneData * data = new KhSceneData();
//        data->setPrevSceneName("TitleScene");
//        std::string prevScene = data->getPrevSceneName();
//        log("이전씬 : %s", prevScene.c_str());
//
//        [결과화면]
//        이전씬 : TitleScene
//
//----------------------------------------------------------------------------------------

class KhSceneData
{
    //이전 씬 이름
    CC_SYNTHESIZE(std::string, prev_scene_name, PrevSceneName);
    CC_SYNTHESIZE(std::string, user_name, UserName);
    CC_SYNTHESIZE(int, user_age, UserAge);
};

//-------------------------------------------------------------------------[ KhSceneData ]







//----------------------------------------------------------------------------------------
//
// >> AppManager 싱글톤
//
//----------------------------------------------------------------------------------------

class AppManager
{
private:
    AppManager(){}
    ~AppManager(){}
    static AppManager* instance;
public:
    static AppManager* getInstance();
    
    float STD_W;
    float STD_H;
    float STD_DIV_WH;
    
    float DVI_W;
    float DVI_H;
    float DVI_DIV_WH;
    
    float REAL_W;
    float REAL_H;
    float REAL_DIV_WH;
    
    float PADDING_LEFT;
    float PADDING_RIGHT;
    float PADDING_TOP;
    float PADDING_BOTTOM;
};

//---------------------------------------------------------------------[ AppManager 싱글톤 ]







//----------------------------------------------------------------------------------------
//
// >> 씬(KhScene)
//
//----------------------------------------------------------------------------------------
#define KH_CREATE_FUNC(__TAPE__)\
public:\
static cocos2d::Scene* createScene(KhSceneData * khSceneData)\
{\
auto scene = Scene::create();\
auto layer = __TAPE__::create();\
layer->initScene(khSceneData);\
scene->addChild(layer);\
CC_SAFE_DELETE(khSceneData);\
return scene;\
}\
CREATE_FUNC(__TAPE__);\
private:\
virtual void initScene(KhSceneData * khSceneData);

class KhScene : public cocos2d::Layer
{
public:
    virtual void initScene(KhSceneData * khSceneData){};
};
//--------------------------------------------------------------------------[ 씬(KhScene) ]






//----------------------------------------------------------------------------------------
//
// >> 중간씬(LoadingScene)
//
//----------------------------------------------------------------------------------------

#define APPMANAGE_REPLACE_SCENE(X,V) (Director::getInstance()->replaceScene(KhLoadingScene<X>::createScene(V)))

template<typename T>
class KhLoadingScene : public KhScene
{
private:
    KhSceneData * data;
    
    virtual void onEnterTransitionDidFinish()
    {
        Layer::onEnterTransitionDidFinish();
        Director::getInstance()->replaceScene(T::createScene(data));
    }
public:
    static cocos2d::Scene* createScene(KhSceneData * khSceneData)
    {
        auto scene = Scene::create();
        auto layer = KhLoadingScene::create();
        layer->data = khSceneData;
        scene->addChild(layer);
        return scene;
    }
    CREATE_FUNC(KhLoadingScene);
};

//------------------------------------------------------------------[ 중간씬(LoadingScene) ]







//----------------------------------------------------------------------------------------
//
// >> RESOLUTION_SETTING
//
// STD_WH : 기준이 되는 해상도
// DVI_WH : 디바이스의 해상도
// REAL_WH : 실제 해상도(좌표해상도)
//
//----------------------------------------------------------------------------------------

#define RESOLUTION_SETTING(_WIDTH, _HEIGHT)\
AppManager * appManager = AppManager::getInstance();\
appManager->STD_W = _WIDTH;\
appManager->STD_H = _HEIGHT;\
appManager->STD_DIV_WH = appManager->STD_W/appManager->STD_H;\
appManager->DVI_W = Director::getInstance()->getOpenGLView()->getFrameSize().width;\
appManager->DVI_H = Director::getInstance()->getOpenGLView()->getFrameSize().height;\
appManager->DVI_DIV_WH = appManager->DVI_W/appManager->DVI_H;\
appManager->REAL_W = appManager->STD_W;\
appManager->REAL_H = appManager->STD_H;\
appManager->REAL_DIV_WH = appManager->REAL_W/appManager->REAL_H;\
if(appManager->STD_DIV_WH > appManager->DVI_DIV_WH){\
appManager->REAL_H = appManager->DVI_H * (appManager->STD_W/appManager->DVI_W);\
Director::getInstance()->getOpenGLView()->setDesignResolutionSize(appManager->STD_W, appManager->STD_H, ResolutionPolicy::FIXED_WIDTH);\
}else{\
appManager->REAL_W = appManager->DVI_W * (appManager->STD_H/appManager->DVI_H);\
Director::getInstance()->getOpenGLView()->setDesignResolutionSize(appManager->STD_W, appManager->STD_H, ResolutionPolicy::FIXED_HEIGHT);\
}\
appManager->REAL_DIV_WH = appManager->REAL_W/appManager->REAL_H;\
appManager->PADDING_LEFT = (appManager->REAL_W-appManager->STD_W)/2;\
appManager->PADDING_RIGHT = appManager->PADDING_LEFT;\
appManager->PADDING_TOP = (appManager->REAL_H-appManager->STD_H)/2;\
appManager->PADDING_BOTTOM = appManager->PADDING_TOP;

//-----------------------------------------------------------------[ RESOLUTION_SETTING ]







//----------------------------------------------------------------------------------------
//
// >> POINT_SETTING
//
// STD_POINT : POINT 값을 기준해상도(STD_FRAME)에 맞춤
// STD_POINT_XY : X,Y 값을 기준해상도(STD_FRAME)에 맞춘 POINT로 변경
// STD_X : X 값을 기준해상도(STD_FRAME)에 맞춤
// STD_Y : Y 값을 기준해상도(STD_FRAME)에 맞춤
//
//----------------------------------------------------------------------------------------

#define STD_POINT(_POINT) Point((_POINT.x)-AppManager::getInstance()->PADDING_LEFT,(_POINT.y)-AppManager::getInstance()->PADDING_TOP)
#define STD_POINT_XY(_X,_Y) Point((_X)+AppManager::getInstance()->PADDING_LEFT,(_Y)+AppManager::getInstance()->PADDING_BOTTOM)
#define STD_X(_X) (_X)+AppManager::getInstance()->PADDING_LEFT
#define STD_Y(_Y) (_Y)+AppManager::getInstance()->PADDING_BOTTOM

//----------------------------------------------------------------------[ POINT_SETTING ]







//----------------------------------------------------------------------------------------
//
// >> LAYOUT_SETTING
//
//----------------------------------------------------------------------------------------

#define FRAME_PADDING_RIGHT AppManager::getInstance()->PADDING_RIGHT
#define FRAME_PADDING_BOTTOM AppManager::getInstance()->PADDING_BOTTOM

#define STD_FRAME_WIDTH AppManager::getInstance()->STD_W
#define STD_FRAME_HEIGHT AppManager::getInstance()->STD_H
#define STD_FRAME_LEFT STD_POINT_XY(0,0).x
#define STD_FRAME_RIGHT STD_POINT_XY(STD_FRAME_WIDTH,0).x
#define STD_FRAME_TOP STD_POINT_XY(0,STD_FRAME_HEIGHT).y
#define STD_FRAME_BOTTOM STD_POINT_XY(0,0).y
#define STD_FRAME_CENTER_X STD_FRAME_WIDTH/2 + STD_FRAME_LEFT
#define STD_FRAME_CENTER_Y STD_FRAME_HEIGHT/2 + STD_FRAME_BOTTOM
#define STD_FRAME_CENTER_POINT Point(STD_FRAME_CENTER_X, STD_FRAME_CENTER_Y)

#define DVI_FRAME_WIDTH AppManager::getInstance()->DVI_W
#define DVI_FRAME_HEIGHT AppManager::getInstance()->DVI_H

#define REAL_FRAME_WIDTH AppManager::getInstance()->REAL_W
#define REAL_FRAME_HEIGHT AppManager::getInstance()->REAL_H
#define REAL_FRAME_LEFT 0.0f
#define REAL_FRAME_RIGHT REAL_FRAME_WIDTH
#define REAL_FRAME_TOP REAL_FRAME_HEIGHT
#define REAL_FRAME_BOTTOM 0.0f
#define REAL_FRAME_CENTER_X REAL_FRAME_WIDTH/2
#define REAL_FRAME_CENTER_Y REAL_FRAME_HEIGHT/2

//----------------------------------------------------------------------[ LAYOUT_SETTING ]






#endif
