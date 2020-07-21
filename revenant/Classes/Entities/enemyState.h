#ifndef ENEMYSTATE_H
#define ENEMYSTATE_H

#include "C_STATE.h"

class C_ENEMY;


class StayEnemy : public C_STATE
{
private:
	StayEnemy();
	StayEnemy(const StayEnemy&);
	StayEnemy& operator=(const StayEnemy&);
	//static StayEnemy* m_pStayEnemy;
public:
	static StayEnemy* Instance();

	//static void createInstance();
	//static void releaseInstance();
	//static StayEnemy* getInstance();


	virtual void Enter(C_ENEMY* enemy);
	virtual void Execute(C_ENEMY* enemy);
	virtual void Exit(C_ENEMY* enemy);

	//virtual void Enter();
	//virtual void Excute();
	//virtual void Exit();
};

class PatrolEnemy : public C_STATE
{
private:
	PatrolEnemy();
	PatrolEnemy(const PatrolEnemy&);
	PatrolEnemy& operator=(const PatrolEnemy&);
	//static PatrolEnemy* m_pPatrolEnemy;
public:
	static PatrolEnemy* Instance();

	//static void createInstance();
	//static void releaseInstance();
	//static PatrolEnemy* getInstance();

	virtual void Enter(C_ENEMY* enemy);
	virtual void Execute(C_ENEMY* enemy);
	virtual void Exit(C_ENEMY* enemy);

	//virtual void Enter();
	//virtual void Excute();
	//virtual void Exit();

};

class AttackEnemy : public C_STATE
{
private:
	AttackEnemy();
	AttackEnemy(const AttackEnemy&);
	AttackEnemy& operator=(const AttackEnemy&);
	//static AttackEnemy* m_pAttackEnemy;
public:
	static AttackEnemy* Instance();

	//static void createInstance();
	//static void releaseInstance();
	//static AttackEnemy* getInstance();

	virtual void Enter(C_ENEMY* enemy);
	virtual void Execute(C_ENEMY* enemy);
	virtual void Exit(C_ENEMY* enemy);

	//virtual void Enter();
	//virtual void Excute();
	//virtual void Exit();

};



#endif