#pragma once
#include "AITypes.h"
#include<iostream>
using namespace std;

class  ChessShape
{
public:
	static void initShape();
	static short calPrior(UCHAR pattern[4][2],int who);
	static char shapeTable[256][256];	//���ͱ�
	static short shapePrior[256][256];	//���͵ȼ���
	static short shapeRank[256][256];	//���͹�ֵ��
	static char fourShapeTable[10][10][10][10];//������α�
	static const int fourShapeScore[16];	//������ͷ�ֵ��
	
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


	
