#ifndef C_STATE_H
#define C_STATE_H

#include "cocos2d.h"

class C_ENEMY;

class C_STATE : public cocos2d::Layer
{
private:

public:
	virtual void Enter(C_ENEMY*) = 0;
	virtual void Execute(C_ENEMY*) = 0;
	virtual void Exit(C_ENEMY*) = 0;

	~C_STATE() {}

	C_ENEMY* getStateMachine() { return _enemy; }
	void setStateMachine(C_ENEMY* stateMachine) { _enemy = stateMachine; }

protected:
	C_ENEMY* _enemy;

};




#endif // !C_STATE_H
