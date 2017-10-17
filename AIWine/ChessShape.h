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
	static char fourShapeTable[10][10][10][10];//组合棋形表
	
private:
	static int getFourShape(int s1, int s2, int s3, int s4);
	static int checkFive(UCHAR p1, UCHAR p2);
	static int checkShape(UCHAR p1, UCHAR p2, int nextShape);
	static int linePiece(UCHAR p1, UCHAR p2, UCHAR mask) { return ((p1&mask ? 1 : 0) << 1) + (p2&mask ? 1 : 0); }
	static void putStone(UCHAR &p1, UCHAR &p2, UCHAR mask) { p1 |= mask; p2 &= ~mask; }
	static short getShapePrior(UCHAR p1, UCHAR p2);
	static UCHAR bitCount(UCHAR p);
};


	
