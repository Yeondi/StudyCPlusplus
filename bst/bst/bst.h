#pragma once

class C_BST
{
private:
	struct S_NODE
	{
		int nId;
		int nData;

		S_NODE * pLeft;
		S_NODE * pRight;
	};
private:
	S_NODE * m_pRoot;

private:
	void printNode(S_NODE *pNode);
	void findNode(int nId,S_NODE * &pFind, S_NODE * &pUp);

public:
	C_BST();
	void insert(int nId, int nData);
	void printData();
	void erase(int nId);
};