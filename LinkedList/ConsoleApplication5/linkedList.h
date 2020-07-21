#pragma once

#include"node.h"

class C_LINKEDLIST
{
private:
	C_NODE m_cBeginDummy;
	C_NODE m_cEndDummy;

	C_NODE * m_pBegin;
	C_NODE * m_pEnd;

private:
	C_NODE * createNode(int nData);
	void insertNode(C_NODE * pPrivNode, C_NODE *pNextNode, C_NODE* pInsertNode);

public:
	C_LINKEDLIST();
	void pushBack(int nData);
	void pushFront(int nData);
	C_NODE * getBegin();
	C_NODE * getEnd();
	void erase(C_NODE *pDelNode);
	void clear();
};