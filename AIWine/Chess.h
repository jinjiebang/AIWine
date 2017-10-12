#pragma once
#include "AITypes.h"

class Chess
{
public:
	void update1(int k);
	void update4();

	char piece;
	UCHAR pattern[4][2];
	UCHAR shape[4][2];
	UCHAR shape4[2];
	char neighbor;
	
	short prior();
	bool isCand() { return neighbor > 0 && piece == EMPTY; }
	bool isShape4(int who, int shape) { return shape4[who] == shape; }
};
