#include "enemyState.h"
#include "C_STATE.h"
#include "Enemy.h"

USING_NS_CC;

StayEnemy::StayEnemy()
{
}

StayEnemy* StayEnemy::Instance()
{
	static StayEnemy instance;

	return &instance;
}



void StayEnemy::Enter(C_ENEMY* enemy)
{
	if (enemy->getState() != 1)
	{
		enemy->setStateDefault();
		cocos2d::log("StayEnemy :: Enter");
	}
}

void StayEnemy::Execute(C_ENEMY* enemy)
{
	enemy->fTimerForStay--;
	cocos2d::log("StayEnemy :: Execute");
	if (enemy->fTimerForStay <= 0.0f)
	{
		cocos2d::log("StayEnemy :: Execute :: ChangeState");
		enemy->ChangeState(PatrolEnemy::Instance());
	}
}

void StayEnemy::Exit(C_ENEMY* enemy)
{
	cocos2d::log("StayEnemy :: Exit");
}

PatrolEnemy::PatrolEnemy()
{
}

PatrolEnemy* PatrolEnemy::Instance()
{
	static PatrolEnemy instance;

	return &instance;
}



void PatrolEnemy::Enter(C_ENEMY* enemy)
{
	if (enemy->getState() != 2)
	{
		enemy->setStateMoving();
		cocos2d::log("PatrolEnemy :: Enter");
		enemy->fTimerForStay = 180.0f;
	}
}

void PatrolEnemy::Execute(C_ENEMY* enemy)
{
	enemy->StartMoving();
	cocos2d::log("PatrolEnemy :: Execute");
	srand(time(NULL));
	int random = rand() % 10;
	float fForce = 2.5f;
	//랜덤 값 지정해서 왼/오 판정
	// 벽 만나면 changeDirection 벽 판정 추가
	// 나중에 id따와서 개별 작업
	if (random % 2 == 0)
	{
		if (enemy->IsWall == false)
		{
			enemy->changeDirection(1);
			enemy->getEnemyBody()->SetLinearVelocity(b2Vec2(fForce, 0.0f));
		}
		else if(enemy->IsWall == true)
		{
			enemy->changeDirection(2);
			enemy->getEnemyBody()->SetLinearVelocity(b2Vec2(fForce, 0.0f));
			enemy->IsWall = false;
		}
	}
	else if (random % 2 == 1)
	{
		if (enemy->IsWall == false)
		{
			enemy->changeDirection(2);
			enemy->getEnemyBody()->SetLinearVelocity(b2Vec2(-fForce, 0.0f));
		}
		else if (enemy->IsWall == true)
		{
			enemy->changeDirection(1);
			enemy->getEnemyBody()->SetLinearVelocity(b2Vec2(-fForce, 0.0f));
			enemy->IsWall = false;
		}
	}
	if (enemy->fMovingTimer <= 0.0f)
	{
		cocos2d::log("PatrolEnemy :: Execute :: ChangeState");
		enemy->ChangeState(StayEnemy::Instance());
	}
	if (enemy->detectTarget())
	{
		//detectTartget을 통해서 어떤 id를 가진 enemy만 attack상태
		enemy->ChangeState(AttackEnemy::Instance());
	}
	enemy->fMovingTimer--;
}

void PatrolEnemy::Exit(C_ENEMY* enemy)
{
	cocos2d::log("PatrolEnemy :: Exit");
	enemy->fMovingTimer = 420.0f;
}

AttackEnemy::AttackEnemy()
{
}

AttackEnemy* AttackEnemy::Instance()
{
	static AttackEnemy instance;

	return &instance;
}

void AttackEnemy::Enter(C_ENEMY* enemy)
{
	if (enemy->getState() != 4)
	{
		enemy->setStateAttack();
		cocos2d::log("AttackEnemy :: Enter");
	}
}

void AttackEnemy::Execute(C_ENEMY* enemy)
{
	cocos2d::log("AttackEnemy :: Execute");
	bool detect = enemy->detectTarget();
	if (!detect)
	{
		cocos2d::log("AttackEnemy :: Execute :: ChangeState");
		enemy->ChangeState(StayEnemy::Instance());
	}
	else
	{
		//enemy의 불렛함수
		enemy->startAttack();
		cocos2d::log(" * gun fire * ");

	}
}

void AttackEnemy::Exit(C_ENEMY* enemy)
{
	cocos2d::log("AttackEnemy :: Exit");
}

//void StayEnemy::Enter()
//{
//
//}
//
//void StayEnemy::Execute()
//{
//}
//
//void StayEnemy::Exit()
//{
//}
//
//void PatrolEnemy::Enter()
//{
//}
//
//void PatrolEnemy::Execute()
//{
//}
//
//void PatrolEnemy::Exit()
//{
//}
//
//void AttackEnemy::Enter()
//{
//}
//
//void AttackEnemy::Execute()
//{
//}
//
//void AttackEnemy::Exit()
//{
//}
//

//void StayEnemy::createInstance()
//{
//	if (!m_pStayEnemy)
//	{
//		m_pStayEnemy = new StayEnemy();
//	}
//}
//
//void StayEnemy::releaseInstance()
//{
//	if (m_pStayEnemy)
//	{
//		delete m_pStayEnemy;
//		m_pStayEnemy = nullptr;
//	}
//}
//
//StayEnemy* StayEnemy::getInstance()
//{
//	return m_pStayEnemy;
//}
//
//void PatrolEnemy::createInstance()
//{
//	if (!m_pPatrolEnemy)
//	{
//		m_pPatrolEnemy = new PatrolEnemy();
//	}
//}
//
//void PatrolEnemy::releaseInstance()
//{
//	if (m_pPatrolEnemy)
//	{
//		delete m_pPatrolEnemy;
//		m_pPatrolEnemy = nullptr;
//	}
//}
//
//PatrolEnemy* PatrolEnemy::getInstance()
//{
//	return m_pPatrolEnemy;
//}
//
//void AttackEnemy::createInstance()
//{
//	if (!m_pAttackEnemy)
//	{
//		m_pAttackEnemy = new AttackEnemy();
//	}
//}
//
//void AttackEnemy::releaseInstance()
//{
//	if (m_pAttackEnemy)
//	{
//		delete m_pAttackEnemy;
//		m_pAttackEnemy = nullptr;
//	}
//}
//
//AttackEnemy* AttackEnemy::getInstance()
//{
//	return m_pAttackEnemy;
//}