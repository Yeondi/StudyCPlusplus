// ConsoleApplication5.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"


int main()
{
	C_LINKEDLIST cList;

	cList.pushBack(3);
	cList.pushBack(3);
	cList.pushBack(3);
	cList.pushBack(4);


	C_NODE *pDelNode = cList.getBegin();
	while (pDelNode != cList.getEnd())
	{
		if (pDelNode->getData() == 3)
			cList.erase(pDelNode);
		pDelNode = pDelNode->getNext();
	}


	/*C_NODE *pDelNode = cList.getBegin();
	while (pDelNode != cList.getEnd())
	{
		if (pDelNode->getData() == 3)
		{
			C_NODE *pTmp = pDelNode;
			cList.erase(pTmp);
			pDelNode = pTmp->getNext();
			delete pTmp;
		}
		else
			pDelNode = pDelNode->getNext();
	}*/





	for (C_NODE *pNode = cList.getBegin(); pNode != cList.getEnd(); pNode = pNode->getNext())
	{
		printf("%d\n", pNode->getData());
	}
}
