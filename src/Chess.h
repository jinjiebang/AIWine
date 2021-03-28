#pragma once
#include "AITypes.h"

class Chess
{
public:
	void update1(int k);
	void update4();
	void updateShape();

	char piece;
	char neighbor;		//8个方向上两格内的棋子数
	UCHAR pattern[4][2];
	UCHAR shape[4][2];
	UCHAR shape4[2];
	
	short prior(int who);
	bool isCand() { return neighbor > 0 && piece == EMPTY; }
};
