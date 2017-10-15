#include "ChessShape.h"
#include<iostream>
#include<string>
using namespace std;
char ChessShape::shapeTable[256][256] = { 0 };
int ChessShape::shapePrior[256][256] = { 0 };
char ChessShape::fourShapeTable[10][10][10][10] = { 0 };
const string ChessShape::shapeName[10] = { "为空", "眠一", "活一", "眠二", "活二", "眠三", "活三", "眠四", "活四", "五连" };
const string ChessShape::shape4Name[9] = { "为空", "活三", "三二", "三三", "冲四", "四二", "四三", "活四", "五连" };
void ChessShape::initShape()
{
	memset(shapeTable, 0, sizeof(shapeTable));
	//初始化棋形表
	for (int i = 9; i > 0; i--)
	{
		for (int p1 = 0; p1 <256; p1++)
		{
			for (int p2 = 0; p2 <256; p2++)
			{
				if (shapeTable[p1][p2] == 0)
				{
					switch (i)
					{
					case 9:
						shapeTable[p1][p2] = checkFive(p1, p2);
						break;
					case 8:
					case 7:
						shapeTable[p1][p2] = checkShape(p1, p2, FIVE);
						break;
					default:
						shapeTable[p1][p2] = checkShape(p1, p2, i + 2);
						break;
					}
				}
			}
		}
	}
	//初始化棋型等级表
	for (int i = 0; i < 256; i++)
		for (int j = 0; j < 256; j++)
			shapePrior[i][j] = getShapePrior(i, j);
	//初始化组合棋形表
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
			for (int k = 0; k < 10; k++)
				for (int l = 0; l < 10; l++)
					fourShapeTable[i][j][k][l] = getFourShape(i, j, k, l);

}
int ChessShape::checkFive(UCHAR p1, UCHAR p2)
{
	int count = 0;
	for (UCHAR mask = 16; mask != 0 && linePiece(p1, p2, mask) == 2; mask <<= 1)
	{
		count++;
	}
	for (UCHAR mask = 8; mask != 0 && linePiece(p1, p2, mask) == 2; mask >>= 1)
	{
		count++;
	}
	return count >= 4 ? FIVE : NONE;
}
int ChessShape::checkShape(UCHAR p1, UCHAR p2, int nextShape)
{
	UCHAR temp1 = p1, temp2 = p2;
	int count = 0;
	for (UCHAR mask = 1; mask != 0; mask <<= 1)
	{
		if (linePiece(temp1, temp2, mask) == 0)
		{
			putStone(temp1, temp2, mask);
			if (shapeTable[temp1][temp2] == nextShape) count++;
			temp1 = p1, temp2 = p2;
		}
	}
	if (count > 0)
	{
		if (nextShape == FIVE)
		{
			return count >= 2 ? FLEX4 : BLOCK4;
		}
		else
		{
			return nextShape - 2;
		}
	}
	return NONE;
} 
int ChessShape::getFourShape(int s1, int s2, int s3, int s4)
{
	int n[10] = { 0 };
	n[s1]++; n[s2]++; n[s3]++; n[s4]++;

	if (n[FIVE] >= 1) return A;							// OOOO_
	if (n[FLEX4] >= 1) return B;						// OOO_
	if (n[BLOCK4] >= 2) return B;						// XOOO_ * _OOOX
	if (n[BLOCK4] >= 1 && n[FLEX3] >= 1) return C;		// XOOO_ * _OO
	if (n[BLOCK4] >= 1 && n[BLOCK3] >= 1) return D;		// XOOO_ * _OOX
	if (n[BLOCK4] >= 1 && n[FLEX2] >= 1) return D;		// XOOO_ * _O
	if (n[BLOCK4] >= 1) return E;						// XOOO_
	if (n[FLEX3] >= 2) return F;						// OO_ * _OO
	if (n[FLEX3] >= 1 && n[BLOCK3] >= 1) return G;		// OO_ * _OOX
	if (n[FLEX3] >= 1 && n[FLEX2] >= 1) return G;		// OO_ * _O
	if (n[FLEX3] >= 1) return H;						// OO_

	return 0;
}
string ChessShape::getShapeName(string shapeStr)
{
	if (shapeStr.length() != 9 || (shapeStr[4] != 'O'&&shapeStr[4] != 'X'))
	{
		return "长度不等于9或中间点不是'O'或'X'";
	}
	char who = shapeStr[4];
	UCHAR p1 = 0, p2 = 0;
	UCHAR mask = 1;
	for (size_t i = 0; i < shapeStr.length(); i++)
	{
		if (i != 4)
		{
			if (shapeStr[i] == who)
			{
				p1 |= mask;
			}
			else if (shapeStr[i] != '_')
			{
				p2 |= mask;
			}
			mask <<= 1;
		}
	}
	string shapeName[10] = { "为空", "眠一", "活一", "眠二", "活二", "眠三", "活三", "眠四", "活四", "连五" };
	int shapeIndex = shapeTable[p1][p2];

	return shapeStr + " 的棋形是：" + shapeName[shapeIndex];
}
void ChessShape::testShape()
{
	initShape();
	UCHAR p[6] = { 0xF3,0xE3,0x05,0x56,0x45,0x57 };
	for (int i = 0; i < 6; i++)
	{
		cout << (int)bitCount(p[i]) << endl;
	}
	string testData[13] = { "__OOOOO__","____OOOO_","__O_OOO__","O_O_O_O__","__OOO____","_XOOO____","X_OOO_X__","__O_O____","X__OO__X_","X_O_O_X__","__XOO____","____O____", "X___O_X__" };
	for (int i = 0; i < 13; i++)
	{
		cout << getShapeName(testData[i]) << " 棋型分值：" << testShapeScore(testData[i]) << endl;
	}
	cin >> p[0];
}
//获取p的二进制编码中1的个数
UCHAR ChessShape::bitCount(UCHAR p)
{
	p = (p & 0x55) + ((p >> 1) & 0x55);
	p = (p & 0x33) + ((p >> 2) & 0x33);
	p = (p & 0x0F) + ((p >> 4) & 0x0F);
	return p;
}
int ChessShape::testShapeScore(string shapeStr)
{
	if (shapeStr.length() != 9 || (shapeStr[4] != 'O'&&shapeStr[4] != 'X'))
	{
		return 0;
	}
	char who = shapeStr[4];
	UCHAR p1 = 0, p2 = 0;
	UCHAR mask = 1;
	for (size_t i = 0; i < shapeStr.length(); i++)
	{
		if (i != 4)
		{
			if (shapeStr[i] == who)
			{
				p1 |= mask;
			}
			else if (shapeStr[i] != '_')
			{
				p2 |= mask;
			}
			mask <<= 1;
		}
	}
	return shapePrior[p1][p2];
}
int ChessShape::getShapePrior(UCHAR p1, UCHAR p2)
{
	int score = 0;
	int val[5] = { 1,4,9,16,25 };
	UCHAR shapeMask[5] = { 0xF0,0x78,0x3C,0x1E,0x0F };
	for (int i = 0; i < 5; i++)
	{
		if ((p2&shapeMask[i]) == 0)
		{
			int cnt = bitCount(p1&shapeMask[i]);
			score += val[cnt];
		}
	}
	return score;
}
short ChessShape::calPrior(UCHAR pattern[4][2],int who)
{
	int v1= shapePrior[pattern[0][1]][pattern[0][0]] +shapePrior[pattern[1][1]][pattern[1][0]] +shapePrior[pattern[2][1]][pattern[2][0]] +shapePrior[pattern[3][1]][pattern[3][0]];
	int v0 = shapePrior[pattern[0][0]][pattern[0][1]] + shapePrior[pattern[1][0]][pattern[1][1]] + shapePrior[pattern[2][0]][pattern[2][1]] + shapePrior[pattern[3][0]][pattern[3][1]];
	return who == 0 ? (v0 << 1) + v1 : (v1 << 1) + v0;
}
