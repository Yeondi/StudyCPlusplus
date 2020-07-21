//
//  AppManager.cpp
//
//  Created by Choi Kyu Hwan on 2015. 7. 24..
//
//

#include "AppManager.h"

AppManager* AppManager::instance = NULL;
AppManager* AppManager::getInstance()
{
    if(instance == nullptr)
    {
        instance = new AppManager();
    }

    return instance;
}
