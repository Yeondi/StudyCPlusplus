// bst.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"


int main()
{
	C_BST cBst{};

	cBst.insert(3, 214213);
	cBst.insert(7, 214213);
	cBst.insert(8, 214213);
	cBst.insert(5, 214213);
	cBst.insert(1, 214213);
	cBst.insert(2, 214213);
	cBst.insert(4, 214213);
	cBst.insert(6, 214213);
	cBst.insert(9, 214213);

	cBst.erase(4);

	cBst.printData();


}
