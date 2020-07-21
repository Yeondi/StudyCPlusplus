#include "pch.h"



C_LINKEDLIST::C_LINKEDLIST() :
	m_cBeginDummy{},
	m_cEndDummy{},
	m_pBegin{},
	m_pEnd{}
{
	m_pBegin = &m_cBeginDummy;
	m_pEnd = &m_cEndDummy;

	m_pBegin->m_pNext = m_pEnd;
	m_pEnd->m_pPriv = m_pBegin;
}

C_NODE * C_LINKEDLIST::createNode(int nData)
{
	C_NODE * pNew = new C_NODE{};
	pNew->m_nData = nData;

	return pNew;
}

void C_LINKEDLIST::insertNode(C_NODE * pPrivNode, C_NODE * pNextNode, C_NODE * pInsertNode)
{
	pPrivNode->m_pNext = pInsertNode;
	pNextNode->m_pPriv = pInsertNode;
	pInsertNode->m_pPriv = pPrivNode;
	pInsertNode->m_pNext = pNextNode;
}

void C_LINKEDLIST::pushBack(int nData)
{
	insertNode(m_pEnd->m_pPriv, m_pEnd, createNode(nData));
}

void C_LINKEDLIST::pushFront(int nData)
{
	insertNode(m_pBegin, m_pBegin->m_pNext, createNode(nData));
}

C_NODE * C_LINKEDLIST::getBegin()
{
	return m_pBegin->m_pNext;
}

C_NODE * C_LINKEDLIST::getEnd()
{
	return m_pEnd;
}

void C_LINKEDLIST::erase(C_NODE * pDelNode)
{
	pDelNode->m_pPriv->m_pNext = pDelNode->m_pNext;
	pDelNode->m_pNext->m_pPriv = pDelNode->m_pPriv;
}

void C_LINKEDLIST::clear()
{
	C_NODE *pDel = getBegin();
	while (pDel != getEnd())
	{
		C_NODE *pNext = pDel->getNext();
		erase(pDel);
		pDel = pNext;
	}
}
