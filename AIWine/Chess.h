#pragma once
#include "AITypes.h"

class Chess
{
public:
	void update1(int k);
	void update4();

	char piece;
	char neighbor;
	UCHAR pattern[4][2];
	UCHAR shape[4][2];
	UCHAR shape4[2];
	
	short prior(int who);
	bool isCand() { return neighbor > 0 && piece == EMPTY; }
};
