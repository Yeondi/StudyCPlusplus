#include "pch.h"

C_NODE::C_NODE() :
	m_nData{},
	m_pNext{},
	m_pPriv{}
{
}

int C_NODE::getData()
{
	return m_nData;
}

C_NODE * C_NODE::getNext()
{
	return m_pNext;
}
