#include "pch.h"

C_BST::C_BST() :
	m_pRoot{}
{
}

void C_BST::insert(int nId, int nData)
{
	S_NODE * pFind = m_pRoot;
	S_NODE * pUp = nullptr;

	findNode(nId, pFind, pUp);

	if (pFind && pFind->nId == nId)
		return;

	if (!pUp)
		m_pRoot = new S_NODE{ nId , nData };
	else if (pUp->nId > nId)
		pUp->pLeft = new S_NODE{ nId , nData };
	else if (pUp->nId < nId)
		pUp->pRight = new S_NODE{ nId , nData };

}

void C_BST::erase(int nId)
{
	S_NODE * pFind = m_pRoot;
	S_NODE * pUp = nullptr;

	findNode(nId, pFind, pUp);

	if (!pFind)
		return;

	S_NODE * pNext = nullptr;

	if (pFind->pLeft && pFind->pRight)
	{

		return;
	}
	else if (pFind->pLeft)
		pNext = pFind->pLeft;
	else if (pFind->pRight)
		pNext = pFind->pRight;

	if (!pUp)
		m_pRoot = pNext;
	else if (pUp->pLeft == pFind)
		pUp->pLeft = pNext;
	else if (pUp->pRight == pFind)
		pUp->pRight = pNext;

	delete pFind;
}

void C_BST::printData()
{
	printNode(m_pRoot);
}


void C_BST::printNode(S_NODE * pNode)
{
	if (!pNode)
		return;

	printf("id = %d , data = %d\n", pNode->nId, pNode->nData);

	printNode(pNode->pLeft);
	printNode(pNode->pRight);
}

void C_BST::findNode(int nId, S_NODE *& pFind, S_NODE *& pUp)
{
	pFind = m_pRoot;
	pUp = nullptr;

	while (pFind && pFind->nId != nId)
	{
		pUp = pFind;
		if (pFind->nId > nId)
			pFind = pFind->pLeft;
		else if (pFind->nId < nId)
			pFind = pFind->pRight;
	}
}
