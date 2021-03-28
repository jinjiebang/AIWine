#pragma once
#include "AITypes.h"
#include<iostream>
using namespace std;

class  ChessShape
{
public:
	static void initShape();
	static short calPrior(UCHAR pattern[4][2],int who);
	static char shapeTable[256][256];	//棋型表
	static short shapePrior[256][256];	//棋型等级表
	static short shapeRank[256][256];	//棋型估值表
	static char fourShapeTable[10][10][10][10];//组合棋形表
	static const int fourShapeScore[16];	//组合棋型分值表
	
private:
	static int getFourShape(int s1, int s2, int s3, int s4);
	static int checkFive(UCHAR p1, UCHAR p2);
	static int checkShape(UCHAR p1, UCHAR p2, int nextShape);
	static bool isEmpty(UCHAR p1, UCHAR p2, UCHAR mask) { return (p1 & mask) == 0 && (p2 & mask) == 0; }
	static bool isMyStone(UCHAR p1, UCHAR p2, UCHAR mask) { return (p1 & mask) > 0 && (p2 & mask) == 0; }		
	static void putStone(UCHAR &p1, UCHAR &p2, UCHAR mask) { p1 |= mask; p2 &= ~mask; }
	static short getShapePrior(UCHAR p1, UCHAR p2);
	static short getShapeRank(UCHAR p1, UCHAR p2);
	static UCHAR bitCount(UCHAR p);
};


	
