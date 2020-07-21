#pragma once

class C_LINKEDLIST;

class C_NODE
{
friend C_LINKEDLIST;
	
private:
	int m_nData;

	C_NODE * m_pNext;
	C_NODE * m_pPriv;

public:
	C_NODE();
	int getData();
	C_NODE * getNext();
};
