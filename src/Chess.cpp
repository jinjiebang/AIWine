#include "Chess.h"
#include "ChessShape.h"
//更新单线棋型
void Chess::update1(int k)
{
	shape[k][0] = ChessShape::shapeTable[pattern[k][0]][pattern[k][1]];
	shape[k][1] = ChessShape::shapeTable[pattern[k][1]][pattern[k][0]];
}
//更新组合棋型
void Chess::update4()
{
	shape4[0] = ChessShape::fourShapeTable[shape[0][0]][shape[1][0]][shape[2][0]][shape[3][0]];
	shape4[1] = ChessShape::fourShapeTable[shape[0][1]][shape[1][1]][shape[2][1]][shape[3][1]];
}
//获取该位置的分值
short Chess::prior(int who)
{
	return ChessShape::calPrior(pattern, who);
}
//更新单线棋型
void Chess::updateShape()
{
	update1(0); update1(1); update1(2); update1(3);
}
